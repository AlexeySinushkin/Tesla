#include "kernel.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_tim_ex.h"
#include "timers.h"

bool getBool(u8* ptr){
	bool result=FALSE;
	if (*ptr>0){
		result=TRUE;
	}
	return result;
}

u16 getU16(u8* ptr){
	u16 result=0;
	result |= *ptr;
	result |= (*(ptr+1))<<8;
	return result;
}

void resetTimerCounters(){
	htim4.Instance->CNT=0;
	htim2.Instance->CNT=0;
	htim1.Instance->CNT=0;
	htim3.Instance->CNT=0;
	htim15.Instance->CNT=0;
	htim16.Instance->CNT=0;
}
void stopTimers(){
  HAL_TIM_Base_Stop(&htim2);
  HAL_TIM_Base_Stop(&htim3);
  HAL_TIM_Base_Stop(&htim4);
  HAL_TIM_Base_Stop(&htim15);
  HAL_TIM_Base_Stop(&htim16);
  HAL_TIM_Base_Stop(&htim1);
}
void startTimers(){
  //HAL_TIM_Base_Start(&htim2);
  //HAL_TIM_Base_Start(&htim3);
  //HAL_TIM_Base_Start(&htim4);

  HAL_TIM_Base_Start(&htim1);
//HAL_TIM_Base_Start(&htim16);
}


#define	FEATURE_CARRIER 		1
#define	FEATURE_BUNCH 			2
#define	FEATURE_GAP				3
#define	FEATURE_GAP_INDENT		6
#define	FEATURE_SKIP_HIGH		4
#define	FEATURE_SKIP_LOW		5
#define FEATURE_PWR				10

volatile void setFeatureState(u8 feature, bool state){
	stopTimers();

	if (feature==FEATURE_CARRIER){
		State.F1=state;
		if (state==TRUE){


			//��������������� ������ �������
			if (State.F10==TRUE){
				HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
				HAL_TIMEx_PWMN_Start(&htim16, TIM_CHANNEL_1);
			}

			if (State.F2==TRUE){
				//HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
			}
			if (State.F3==TRUE){
				//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
				//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
			}
			if (State.F4==TRUE){
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
			}
			if (State.F5==TRUE){
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
			}
			if (State.F6==TRUE){
				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
			}

			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIM_Base_Start_IT(&htim1);

		}else{
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
			//��� ���������� ���������, �������� ��
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
			//HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);

			HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim16, TIM_CHANNEL_1);
		}
	/*}else if (feature==FEATURE_BUNCH){
		State.F2=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
		}else{
			HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_1);
		}*/
	}else if (feature==FEATURE_GAP){
		State.F3=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
		}else{
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
		}
	}else if (feature==FEATURE_GAP_INDENT){
		State.F6=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
		}else{
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
		}
	}else if (feature==FEATURE_SKIP_HIGH){
		State.F4=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		}else{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
		}
	}else if (feature==FEATURE_SKIP_LOW){
		State.F5=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		}else{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
		}
	}else if (feature==FEATURE_PWR){
		State.F10=state;
		if (state==TRUE){
			//start with timer 1
			//HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
			//HAL_TIMEx_PWMN_Start(&htim16, TIM_CHANNEL_1);
		}else{
			HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim16, TIM_CHANNEL_1);
		}
	}

	if (State.F1==TRUE){
		startTimers();
	}

/*
	u16 cntT1=htim1.Instance->CNT;
	u16 cntT2=htim2.Instance->CNT;
	u16 cntT3=htim3.Instance->CNT;
	u16 cntT4=htim4.Instance->CNT;
	ITM_SendChar('A');
*/
	stopTimers();
	resetTimerCounters();
	startTimers();
}




void packet_02_feature_change(u8* body, u16 bodySize){
	u8 num = *(body);
	bool enable = getBool(*(body+1));
	setFeatureState(num, enable);
}
void packet_04_timer_config(u8* body, u16 bodySize){
	stopTimers();
	resetTimerCounters();
	/*
������ �������
������ ��������
����� �������
���� �������
������ �����
���������� �����
������ ������� � ������� ������� �����
���� ������� � ������� ������� �����
������ ������� ������� �����
���� �������� �������� �����
������ ������� ������ �����
���� �������� ������ �����
	*/
	u8* p=body;
	htim1.Instance->ARR=getU16(p);
	p+=2;
	htim1.Instance->CCR1=getU16(p);
	htim3.Instance->ARR=getU16(p);
	p+=2;
	htim3.Instance->CCR3=getU16(p);
	p+=2;
	htim3.Instance->CCR4=getU16(p);
	p+=2;
	htim2.Instance->ARR=getU16(p);//������ �����
	htim4.Instance->ARR=getU16(p);
	htim15.Instance->ARR=getU16(p);
	p+=2;
	htim15.Instance->CCR1=getU16(p);//���������� �����
	p+=2;
	htim4.Instance->CCR2=getU16(p);
	p+=2;
	htim4.Instance->CCR4=getU16(p);
	p+=2;
	htim2.Instance->CCR3=getU16(p);//������ ������� ������� �����
	p+=2;
	htim2.Instance->CCR4=getU16(p);
	p+=2;
	htim2.Instance->CCR1=getU16(p);
	p+=2;
	htim2.Instance->CCR2=getU16(p);

	startTimers();
}


volatile SearchEnv_t SearchState;
TimerHandle_t xTimerSearch;
extern void vTimerSearcher(TimerHandle_t xTimer );

void stopSearchSoftTimer(){
	if (State.SearchTimerEnabled==TRUE){
		xTimerStop(xTimerSearch, 100);
	}
	State.SearchTimerEnabled=FALSE;
}
void startSearchSoftTimer(u16 delay){
	if (xTimerSearch==0){
		xTimerSearch=xTimerCreate("Searcher", pdMS_TO_TICKS(delay), pdTRUE, ( void * ) 0,
				vTimerSearcher);
	}else{
		xTimerChangePeriod(xTimerSearch, pdMS_TO_TICKS(delay), 200);
	}
	State.SearchTimerEnabled=TRUE;
}


void vTimerSearcher(TimerHandle_t xTimer )
{
	if (State.SearcherState==Searching){
		u32 current=htim16.Instance->ARR;
		bool continueSearch=FALSE;
		if (current<SearchState.SearchTo &&
				State.SearchDirection==SearchPeriodIncrease){
			current++;
			continueSearch=TRUE;
		}
		if (current>SearchState.SearchTo &&
				State.SearchDirection==SearchPeriodDecrease){
			current--;
			continueSearch=TRUE;
		}

		if (continueSearch==TRUE){
			htim16.Instance->ARR=current;
			htim16.Instance->CCR1=current/2;
		}else{
			HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim16, TIM_CHANNEL_1);
			State.SearcherState=SearchIdle;
			stopSearchSoftTimer();
		}
	}
}


void packet_06_search(u8* body, u16 bodySize){
	u8* p=body;
	State.SearcherState=SearchIdle;

	SearchState.SearchFrom=getU16(p);
	p+=2;
	SearchState.SearchTo=getU16(p);
	p+=2;
	u16 delay=getU16(p);
	if (SearchState.SearchFrom<SearchState.SearchTo){
		State.SearchDirection=SearchPeriodIncrease;
	}else{
		State.SearchDirection=SearchPeriodDecrease;
	}



	htim16.Instance->ARR=SearchState.SearchFrom;
	htim16.Instance->CCR1=SearchState.SearchFrom/2;
	HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim16, TIM_CHANNEL_1);

	State.SearcherState=Searching;
	startSearchSoftTimer(delay);

}
void packet_08_search_stop(u8* body, u16 bodySize){
	HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim16, TIM_CHANNEL_1);
	State.SearcherState=SearchIdle;
	stopSearchSoftTimer();
}

void packet_0A_just_generate(u8* body, u16 bodySize){
	u8* p=body;
	u16 period=getU16(p);
	p+=2;
	u16 duty=getU16(p);
	p+=2;
	u8 feature = *p;


/*
	if (State.SearcherState!=Generating){
		stopSearchSoftTimer();
		HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
		HAL_TIMEx_PWMN_Start(&htim16, TIM_CHANNEL_1);
		State.SearcherState=Generating;
	}
*/


	if (feature==FEATURE_CARRIER){
		htim1.Instance->ARR=period;
		htim1.Instance->CCR1=period/2;//duty;
		htim16.Instance->ARR=(period*2)+1;
		htim16.Instance->CCR1=period;//duty;
		resetTimerCounters();
	}else if (feature==FEATURE_PWR){
		//htim16.Instance->ARR=period;
		//htim16.Instance->CCR1=duty;
	}
}

#define t1SmokeTo 12 		//1030;
#define t1Overflow 8		//1010;
#define t1Switch 4			//1000;
#define t1PeriodHalfWave 42
#define htimc htim3
#define TIMC TIM3

void KERNEL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == TIM1){
		if (State.ModeState==ModeHalfWave ||
				State.ModeState==ModeSmoking){
			htim1.Instance->ARR=t1PeriodHalfWave;
			htim1.Instance->CCR1=t1PeriodHalfWave/2;
		}else if (State.ModeState==ModeQuarterWave){
			htim1.Instance->ARR=t1PeriodHalfWave*2;
			htim1.Instance->CCR1=t1PeriodHalfWave;
		}
	}
	if (htim->Instance == TIMC){

		if (State.ModeState!=ModeIdle){
			State.ModeState=ModeHalfWave;

			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
		}
	}
}


void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){

	if (htim->Instance == TIMC){
        if (__HAL_TIM_GET_FLAG(&htimc, TIM_FLAG_CC1) == SET)
        {
            __HAL_TIM_CLEAR_FLAG(&htimc, TIM_FLAG_CC1);
			State.ModeState=ModeQuarterWave;
        }
        if (__HAL_TIM_GET_FLAG(&htimc, TIM_FLAG_CC2) == SET)
        {
            __HAL_TIM_CLEAR_FLAG(&htimc, TIM_FLAG_CC2);
			State.ModeState=ModeSmoking;
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
        }
	}
}


void startGeneration(){
	HAL_TIM_Base_Start_IT(&htimc);
	HAL_TIM_Base_Start_IT(&htim1);

	htim1.Instance->ARR=t1PeriodHalfWave;
	htim1.Instance->CCR1=t1PeriodHalfWave/2;

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
}

void KERNEL_Init(){
	State.ModeState=ModeIdle;



	htim1.Instance->ARR=t1PeriodHalfWave;
	htim1.Instance->CCR1=t1PeriodHalfWave/2;

	htimc.Instance->PSC=t1PeriodHalfWave;
	htimc.Instance->ARR=t1SmokeTo;
	htimc.Instance->CCR1=t1Switch;
	htimc.Instance->CCR2=t1Overflow;


	__HAL_TIM_ENABLE_IT(&htimc, TIM_IT_CC1 );
	__HAL_TIM_ENABLE_IT(&htimc, TIM_IT_CC2 );
	HAL_TIM_OC_Start(&htimc, TIM_CHANNEL_1);
	HAL_TIM_OC_Start(&htimc, TIM_CHANNEL_2);
	State.ModeState=ModeHalfWave;
}
extern void KERNEL_Task(){
	osDelay(100);
	resetTimerCounters();
	startGeneration();
	while(1){
		osDelay(10000);
	}
}


