/*
 * common.c
 *
 *  Created on: 26 ���. 2019 �.
 *      Author: User
 */
#include "common.h"
#define MAX_PERIOD 0xFFFF

TimerConf_t calculatePeriodAndPrescaler(u32 freq){
	TimerConf_t result;
	if (freq % 2 != 0)
		freq--; //������ �������
	result.Prescaler = 1;
	while ((SystemCoreClock / (freq * result.Prescaler)) > MAX_PERIOD) {
		result.Prescaler++;
	}
	int clock = SystemCoreClock / result.Prescaler;
	result.Period = clock / freq;
	//0 - ��� ������������, 1 - ������������ �� 2...
	result.Prescaler--;
	return result;
}

s16 getS16(volatile u8* buf, u8 offset){
	return (s16)(*(buf+offset) | (*(buf+offset+1))<<8);
}
