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
