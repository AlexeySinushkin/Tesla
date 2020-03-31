/*
 * common.c
 *
 *  Created on: 26 ���. 2019 �.
 *      Author: User
 */
#include "common.h"
#define MAX_PERIOD 0xFFFF

void calculatePeriodAndPrescaler(u32 freq, TimerConf_t* timerConf){
	if (freq % 2 != 0)
		freq--; //������ �������
	timerConf->Prescaler = 1;
	while ((SystemCoreClock / (freq * timerConf->Prescaler)) > MAX_PERIOD) {
		timerConf->Prescaler++;
	}
	int clock = SystemCoreClock / timerConf->Prescaler;
	timerConf->Period = clock / freq;
	//0 - ��� ������������, 1 - ������������ �� 2...
	timerConf->Prescaler--;
}

s16 getS16(volatile u8* buf, u8 offset){
	s16 result = *(buf+offset);
	result |= ((s16)*(buf+offset+1))<<8;
	return result;
}


void copy(void* src, void* dst, u16 count){
	int j=0;
	for (j = 0; j < count; j++) {
		*(char *) (dst +  j) =
				*(char *) (src + j);
	}
}

volatile void nop()
{
	asm("NOP");
}

volatile void deadtime()
{
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
}
