#include "common.h"

#define SOFT_TIMER_MS_PER_TICK 5
#define SOFT_TIMERS 7

struct soft_timer_t
{
	u32 period;
	u32 next;
	bool unstop;
	bool used;	//������ ������������
	void (*funcPtr)(); //��������� �� �������
};

extern volatile u32 timeStampCounter;
//��������� ������ � ����� �� ��������� ���
//���������� ����� ������� ��� ���������� (������ ����)
extern u8 addTimer(u32 period, bool unstop, void (*funcPtr));

//������� ������.
extern void removeTimer(u8 timer_number);

//������� ������� � �������� �������
extern void restartTimer(u8 timer_number);

extern void hardwareTimerInvoke();
