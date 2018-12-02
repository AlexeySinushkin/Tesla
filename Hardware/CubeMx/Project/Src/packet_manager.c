/*
 * packet_manager.c
 *
 *  Created on: 20 ����. 2018 �.
 *      Author: User
 */
#include "common.h"
#include "cmsis_os.h"
#include "timers.h"
#include "packet_manager.h"

extern void PM_TxComplete(DMA_HandleTypeDef * hdma);
extern void vTimerStateSend(TimerHandle_t xTimer);

#define STATE_SEND_PERIOD 200
//���� �� 0.5 ������� �� ������� �����, ���-�� �� ���
#define RECEIVE_TIMEOUT 500

void addRxByte(u8 rxByte){
    	if (Uart.rxIndex<RX_BUF_SIZE-1){
    		Uart.rxBuf[Uart.rxIndex]=rxByte;
    		Uart.rxIndex++;
    		return;
    	}
/*
    if (USART_GetFlagStatus(USART1,USART_FLAG_TXE)==SET)
	{
		if(Env.TxState==Sending){
			if (Env.txIndex<Env.txBufSize){
				USART_SendData(USART1, Env.txBuf[Env.txIndex]);
				//while (USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
				Env.txIndex++;
			}else{
				Env.txIndex=0;
				Env.TxState=TxIdle;
				USART_ClearFlag(USART1,USART_FLAG_TXE);
				USART_ClearITPendingBit(USART1, USART_IT_TXE);
				USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			}
		}
	}
    */
}
void PacketTimeOut(void)
{
	Uart.RxState=ReceivingTimeout;
}



void resetRxBuf(){
	int i=0;
	Uart.rxIndex=0;
	for(i=0;i<RX_BUF_SIZE;i++){
		Uart.rxBuf[i]=0;
	}
}

TimerHandle_t xTimerState;
TimerHandle_t xTimerTimeout;
void vTimerCallback( TimerHandle_t xTimer )
{
	Uart.RxState=ReceivingTimeout;
}


void PM_Init(){
	Uart.RxState=WaitingStart;
	xTimerTimeout = xTimerCreate
              ( /* Just a text name, not used by the RTOS
                kernel. */
                "Rx Timeout",
                /* The timer period in ticks, must be
                greater than 0. */
				pdMS_TO_TICKS(RECEIVE_TIMEOUT),
                /* The timers will auto-reload themselves
                when they expire. */
				pdTRUE,
                /* The ID is used to store a count of the
                number of times the timer has expired, which
                is initialised to 0. */
                ( void * ) 0,
                /* Each timer calls the same callback when
                it expires. */
                vTimerCallback
              );
	xTimerState=xTimerCreate("Tx State", pdMS_TO_TICKS(STATE_SEND_PERIOD), pdTRUE, ( void * ) 0,
			vTimerStateSend);
	hdma_usart1_tx.XferCpltCallback=PM_TxComplete;
}

void startTimer(){
    /* Start the timer.  No block time is specified, and
    even if one was it would be ignored because the RTOS
    scheduler has not yet been started. */
    if( xTimerStart( xTimerTimeout, 0 ) != pdPASS )
    {
        /* The timer could not be set into the Active
        state. */
    	configASSERT( xTimerTimeout );
    }
}
void stopTimer(){
	xTimerStop( xTimerTimeout,0);
}
void resetTImer(){
	xTimerReset(xTimerTimeout,0);
}
void restartTimer(){
	stopTimer();
	resetTImer();
	startTimer();
}
void create_rx_err(u8 err){
	Uart.rxIndex=0;
	Uart.RxState=WaitingStart;
	stopTimer();
	resetTImer();
	if (Uart.TxState==TxIdle){
		createOutPacketAndSend(0x02,1,&err);
	}
}
void PM_Task(){
    if( xTimerStart( xTimerState, pdMS_TO_TICKS(1000) ) != pdPASS )
    {
    	configASSERT( xTimerTimeout );
    }

	int i=0;

	while(1){

			i=0;
		if (Uart.RxState==WaitingStart){
			//���� ����� �����
			//������ ����� �������� ������ ��������
			if (Uart.rxIndex==0){
				//�������� ������ ����, ���� ��� 7-8
				osDelay(10);
				continue;
			}
			else if (Uart.rxIndex>0 && Uart.rxBuf[0]!=PACKET_START){
				Uart.rxIndex=0;
				create_rx_err(0x01);
				continue;
			}else{
				Uart.RxState=ReceivingSize;
				Uart.rxPackSize=0;
				restartTimer();
			}
		}else if (Uart.RxState==ReceivingSize){
			if (Uart.rxIndex>3){
				u16 inPacksize=Uart.rxBuf[1]+
						(Uart.rxBuf[2]*256);
				if (inPacksize<6){
					create_rx_err(0x02);
					continue;
				}
				if (inPacksize>RX_BUF_SIZE-6){
					create_rx_err(0x03);
					continue;
				}
				Uart.rxPackSize=inPacksize;
				Uart.RxState=ReceivingPacket;
			}else{
				osDelay(3);
			}
		}else if (Uart.RxState==ReceivingPacket){
			if (Uart.rxIndex>=Uart.rxPackSize){
				//����� ������. ������������� ������ � ��������� ���
				stopTimer();
				Uart.RxState=RxChecking;
			}else{
				osDelay(10);
			}
		}else if (Uart.RxState==ReceivingTimeout){
			create_rx_err(0x06);
		}else if(Uart.RxState==RxChecking){
			//������������� ������ �������� ��� ��� ������ ���� �����
			stopTimer();
			//check CRC
			u8 crc=0;
			//265-2=263
			for (i=0;i<Uart.rxPackSize-2;i++){
				crc+=Uart.rxBuf[i];
			}
			u8 xorCRC=crc^0xAA;
			//���� ����������� ����� �������
			if (crc==Uart.rxBuf[Uart.rxPackSize-2]&&
					xorCRC==Uart.rxBuf[Uart.rxPackSize-1]){
				Uart.RxState=RxProcessing;

			}else{//crc error
				create_rx_err(0x04);
			}
		}else if(Uart.RxState==RxProcessing){
			osDelay(2);
		}else if(Uart.RxState==RxProcessed){
			Uart.rxIndex=0;
			Uart.RxState=WaitingStart;
		}

		}


}


void createOutPacketAndSend(u8 command, u8 bodySize, u8* bodyData){

	if (Uart.TxState==Sending)
	{
		return;
	}



		u16 i=0;
		Uart.txBufSize=6+bodySize;
		Uart.txBuf[0]=PACKET_START;
		Uart.txBuf[1]=(u8)(Uart.txBufSize);
		Uart.txBuf[2]=(u8)(Uart.txBufSize>>8);
		Uart.txBuf[3]=command;

		if (bodySize>0){
			for (i=0;i<bodySize;i++){
				Uart.txBuf[4+i]=*(bodyData+i);
			}
		}

		u8 crc=0;
		for (i=0;i<4+bodySize;i++){
			crc+=Uart.txBuf[i];
		}
		Uart.txBuf[4+bodySize]=crc;
		Uart.txBuf[5+bodySize]=crc^0xAA;


		Uart.txIndex=0;

		HAL_UART_Transmit_DMA(&huart1, &Uart.txBuf, Uart.txBufSize);
		//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}
u8 sentTimes=0;
void vTimerStateSend(TimerHandle_t xTimer )
{
	createOutPacketAndSend(0x01, 8, &State);
	sentTimes++;
	if (sentTimes>2){
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12, GPIO_PIN_RESET);
		State.LedLight=FALSE;
		//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_12);
	}else{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12, GPIO_PIN_SET);
		State.LedLight=TRUE;
	}
	if (sentTimes>5){
		sentTimes=0;
	}
}
void PM_TxComplete(DMA_HandleTypeDef * hdma){
	Uart.TxState=TxIdle;
}
