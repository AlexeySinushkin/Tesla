#ifndef __COMMON_H
#define __COMMON_H

//#include "stm32f10x.h"
//#include <CoOs.h>
#include <stdio.h>




typedef enum {FALSE = 0, TRUE = !FALSE} bool;
#define u32 uint32_t
#define s32 int32_t
#define u16 uint16_t
#define s16 int16_t
#define u8 uint8_t
#define s8 int8_t

typedef enum
{
	Step0,
	Step1,
	Step2,
	Step3,
	Step4,
	Step5
}Steps;


typedef enum
{
	CW=0,
	CCW
}Directions;

//��������� ������ ���� ��������� �� 32 ���� �� ������
typedef struct
{
	Steps	CurrentStep:8;
	bool	ChangeStep:1;
	bool	HighState:1;
	u8		tmp:6;
	bool	Enabled:8;
	Directions Direction:8;

	u32 freq:32;
	u32 modulationFreq:32;
	u16 setPinHI:16;
	u16 setPinLO:16;
	u16 setPinHIM:16;//Mirror
	u16 setPinLOM:16;//Mirror
} Env_t;




extern volatile Env_t Env;

#endif
