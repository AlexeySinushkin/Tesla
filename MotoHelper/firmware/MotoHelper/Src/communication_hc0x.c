#include "communication_hc0x.h"
#include "communication_manager.h"

UART_HandleTypeDef* uart;
DMA_HandleTypeDef* dma_usart_tx;
TaskHandle_t commHandle;
TIM_HandleTypeDef* communicationTimer;

//333ms Timer
#define AT_TIMEOUT 1000/300
volatile u8 rxByte;
volatile HCModule_t HCState;

const u8 textAT[] = { "AT" };
const u8 textATOK[] = { "OK" };
const u8 textATName[] = { "AT+NAMEMotoHelpeR" }; //
const u8 textATNameOK[] = { "+NAME=MotoHelpeR" };
const u8 textATPin[] = { "AT+PIN2020" };
const u8 textATPinOK[] = { "+PIN=2020" };
const u8 textATSpeed[] = { "AT+UART=115200,0,0" };


extern void COMM_TxComplete(DMA_HandleTypeDef * hdma);
extern void HC_Configure();

void COMM_Configure_Driver(UART_HandleTypeDef* uart_,
		DMA_HandleTypeDef* hdma_usart_, TIM_HandleTypeDef* timer,
		TaskHandle_t taskHandle) {

	communicationTimer = timer;
	commHandle = taskHandle;
	uart = uart_;
	dma_usart_tx = hdma_usart_;
	dma_usart_tx->XferCpltCallback = COMM_TxComplete;
	HAL_UART_Receive_IT(uart, &rxByte, 1);

	HC_Configure();

}

void COMM_SendData(u16 size) {
	HAL_UART_Transmit_DMA(uart, &commOutBuf, size);
	CommState.TxState = TxSending;
}

void COMM_TxComplete(DMA_HandleTypeDef * hdma) {
	CommState.TxState = TxIdle;
	xTaskResumeFromISR(commHandle);
}

void COMM_RxCallback() {
	if (CommState.rxIndex < COMM_IN_BUF_SIZE - 1) {
		commInBuf[CommState.rxIndex] = rxByte;
		CommState.rxIndex++;
	}
	HAL_UART_Receive_IT(uart, &rxByte, 1);
	xTaskResumeFromISR(commHandle);
}


void COMM_DRIVER_PeriodElapsedCallback()
{
	HCState.ATState = ATInitFail;
	xTaskResumeFromISR(commHandle);
}


void HC_StartTimer() {
	HAL_TIM_Base_Start_IT(communicationTimer);
}

void HC_StopTimer() {
	HAL_TIM_Base_Stop(communicationTimer);
	communicationTimer->Instance->CNT = 0;
}

//delay between commands
void HC_Delay() {
	HC_Delay();
}

//as a rule not enough amount of bytes received
//await rest
void HC_Suspend() {
	vTaskSuspend(commHandle);
}

bool hasText(const u8* searchText, u8 length) {
	int iterator = 0;
	volatile u8* bigBuf = &commInBuf[0];
	for (iterator = 0; iterator < length; iterator++) {
		if (*bigBuf++ != *searchText++)
			return FALSE;
	}
	return TRUE;
}

bool configSendCommand(const u8* text, u8 length) {
	if (CommState.TxState == TxIdle) {

		CommState.TxState = TxSending;

		u8 i = 0;
		if (length > 0) {
			for (i = 0; i < length; i++) {
				commOutBuf[i] = *(text + i);
			}
			commOutBuf[i] = '\r';
			commOutBuf[i + 1] = '\n';
			COMM_SendData(length + 2);
		}

		HC_StartTimer();
		return TRUE;
	} else {
		return FALSE;
	}
}

void configModuleAT() {
	//Test connection Send AT - OK
	//Cheching name
	//Setting up Name AT+NAME=MotoHelpeR\r\n
	//Check pin and set it if nessesary
	//Set up the PIN AT+PIN2020

	//AT-OK
	if (HCState.ATState == AT) {
		if (configSendCommand(&textAT[0], sizeof(textAT)) == TRUE) {
			resetRxBuf();
			HCState.ATState = ATAnswerWait;
		}
	} else if (HCState.ATState == ATAnswerWait && CommState.TxState == TxIdle) { //Tx Idle means everything sent
		if (CommState.rxIndex > 2) {
			if (hasText(&textATOK[0], 2) == TRUE) {
				stopTimer();
				HCState.ATState = ATNameGet;
				//����� ��������� ����� ������ ����
				HC_Delay();
			} else {
				//no text
				HCState.ATState = AT;
			}
		} else {
			HC_Suspend();
		}
	}

	//Name check
	if (HCState.ATState == ATNameGet) {
		if (configSendCommand(&textATName[0], 7) == TRUE) {
			resetRxBuf();
			HCState.ATState = ATNameGetAnswerWait;
		}
	} else if (HCState.ATState == ATNameGetAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex > 10) {
			if (hasText(&textATNameOK[0], 10) == TRUE) {
				//��� ���� ����������� ���
				HCState.ATState = ATPinGet;
			} else {
				HCState.ATState = ATName;
			}
			HC_StopTimer();
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}

	//Name set
	if (HCState.ATState == ATName) {
		if (configSendCommand(&textATName[0], sizeof(textATName)) == TRUE) {
			resetRxBuf();
			HCState.ATState = ATNameAnswerWait;
		}
	} else if (HCState.ATState == ATNameAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex > 5) {
			if (hasText(&textATNameOK[0], 5) == TRUE) {
				HCState.ATState = ATPinGet;
			} else {
				//no text
				HCState.ATState = ATName;
			}
			HC_StopTimer();
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}

	if (HCState.ATState == ATPinGet) {
		if (configSendCommand(&textATPin[0], 6) == TRUE) {
			resetRxBuf();
			HCState.ATState = ATPinGetAnswerWait;
		}
	} else if (HCState.ATState == ATPinGetAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex >= 9) {
			if (hasText(&textATPinOK[0], sizeof(textATPinOK)) == TRUE) {
				HCState.ATState = ATSpeed;
			} else {
				HCState.ATState = ATPin;
			}
			HC_StopTimer();
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}

	//Set pin
	if (HCState.ATState == ATPin) {
		if (configSendCommand(&textATPin[0], sizeof(textATPin)) == TRUE) {
			resetRxBuf();
			HCState.ATState = ATPinAnswerWait;
		}
	} else if (HCState.ATState == ATPinAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex > 4) {
			if (hasText(&textATPinOK[0], 4) == TRUE) {
				HCState.ATState = ATSpeed;
			} else {
				HCState.ATState = ATPin;
			}
			HC_StopTimer();
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}


	if (HCState.ATState == ATSpeed) {
		if (configSendCommand(&textATSpeed[0], sizeof(textATSpeed)) == TRUE) {
			resetRxBuf();
			HC_StopTimer();
			uart->Init.BaudRate=115200;
			HAL_UART_Init(uart);
			HCState.ATState = ATSpeedAT2;
			HC_Delay();
		}
	}

	if (HCState.ATState == ATSpeedAT2) {
		if (configSendCommand(&textAT[0], sizeof(textAT)) == TRUE) {
			resetRxBuf();
			HCState.ATState = ATSpeedAT2AnswerWait;
		}
	} else if (HCState.ATState == ATSpeedAT2AnswerWait && CommState.TxState == TxIdle) {
		if (CommState.rxIndex > 2) {
			if (hasText(&textATOK[0], 2) == TRUE) {
				stopTimer();
				HCState.ATState = ATWaitStream;
			} else {
				//no text
				HCState.ATState = ATInitFail;
			}
		} else {
			HC_Suspend();
		}
	}

	if (HCState.ATState == ATWaitStream ||
			HCState.ATState == ATInitFail) {
		osDelay(1);
	}

}

//setup pin, speed, etc
void HC_Configure() {
configure:
	stopTimer();
	TimerConf_t result = calculatePeriodAndPrescaler(AT_TIMEOUT);
	communicationTimer->Instance->PSC = result.Prescaler;
	communicationTimer->Instance->ARR = result.Period;

	while(TRUE){
		configModuleAT();
		if (HCState.ATState == ATWaitStream)
		{
			CommState.CommDriverReady = TRUE;
			return;
		}
		else if (HCState.ATState == ATInitFail) {
			CommState.CommDriverReady = FALSE;
			goto configure;
		}
	}
}

