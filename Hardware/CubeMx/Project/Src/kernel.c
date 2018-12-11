#include "kernel.h"
#include "stm32f1xx_hal_tim.h"



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

#define	FEATURE_CARRIER 1


volatile void setFeatureState(u8 num, bool state){

	if (num==1){
		State.F1=state;
		if (state==TRUE){
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
		}else{
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
		}
	}else if (num==2){
		State.F2=state;
	}else if (num==3){
		State.F3=state;
	}else if (num==4){
		State.F4=state;
	}

}

bool getFeatureState(num){
	if (num==1){
		return State.F1;
	}else if (num==2){
		return State.F2;
	}else if (num==3){
		return State.F3;
	}else if (num==4){
		return State.F4;
	}
	return FALSE;
}




void packet_02_feature_change(u8* body, u16 bodySize){
	u8 num = *(body);
	bool enable = getBool(*(body+1));
	setFeatureState(num, enable);
}
void packet_04_timer_config(u8* body, u16 bodySize){
	u8 num = *(body);
	u8 prescaler = *(body+1);
	u16 period = getU16(body+2);
	u16 duty = getU16(body+4);
/*
	TIM_HandleTypeDef* t = getTimer(num);
	bool timerState=getFeatureState(num);
	//���� ������ � ������� ���������, ������������� ���
	if (timerState==TRUE){
		HAL_TIM_Base_Stop(t);
	}

	//HAL_TIM_Base_Init()
	TIM_TypeDef* timerInstance=htim1.Instance;

	timerInstance->ARR = period;
	timerInstance->CCR1 = duty;


	t->Init.Prescaler=prescaler;
	t->Init.Period=period;
	HAL_TIM_Base_Init(t);


	if (num==1){

		TIM_OC_InitTypeDef sConfigOC;
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = duty;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

		if (HAL_TIM_PWM_ConfigChannel(t, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
	}


	//���� ������ ��� � ������� ���������,
	//����� ��������� ���
	if (timerState==TRUE){
		HAL_TIM_Base_Start(t);
	}

*/

}
