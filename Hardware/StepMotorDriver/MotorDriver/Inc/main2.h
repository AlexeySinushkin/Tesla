/*
 * main2.h
 *
 *  Created on: 8 ���. 2018 �.
 *      Author: user
 */

#ifndef MAIN2_H_
#define MAIN2_H_
#include "common.h"

#define MAX_PERIOD 0xFFFF
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern void initMain();
extern void loopMain();
//extern void USR_TIM3_IRQHandler();
//extern void USR_ADC1_IRQHandler();


#endif /* MAIN2_H_ */
