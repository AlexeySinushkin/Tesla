/*
 * communication.h
 *
 *  Created on: 30 ���. 2019 �.
 *      Author: User
 */
#include "common.h"

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#define COMM_IN_BUF_SIZE 20
#define COMM_OUT_BUF_SIZE 80

typedef enum
{
	WaitingStart,
	ReceivingSize,
	ReceivingPacket,
	ReceivingTimeout,
	RxChecking,
	RxProcessing,
	RxProcessed
} RxStates;

typedef enum
{
	TxIdle,
	TxSending
}TxStates;

//��������� ������ ���� ��������� �� 32 ���� �� ������
typedef struct
{
	RxStates RxState:4;
	TxStates TxState:3;
	bool	CommDriverReady:1;
	u16 rxPacketSize:12;
	u16 rxIndex:12;
	u8 softTimer:8;
} CommState_t;



extern TaskHandle_t commHandle;
extern volatile CommState_t CommState;

extern volatile u8 commInBuf[COMM_IN_BUF_SIZE];
extern volatile u8 commOutBuf[COMM_OUT_BUF_SIZE];

extern void COMM_Init(TaskHandle_t taskHandle);
extern void COMM_Task();
extern void createOutPacketAndSend(u8 command, u16 bodySize, u8* bodyData);
extern void notifyPacketProcessed();
#endif /* COMMUNICATION_H_ */