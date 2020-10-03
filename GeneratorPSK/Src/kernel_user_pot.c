#include "kernel.h"
#include "kernel_user_pot.h"


volatile u16 ADC_Buf[5];


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* p_hdma) {

	if (configuration.reading == TRUE)
	{
		return;
	}

	u16 value = ADC_Buf[0];
	//���� ��� �����
	if (value >= 0 && value < ADC_MAX) {
		configuration.accumulationCount = MIN_ACCUMULATION_COUNT
				+ ((MAX_ACCUMULATION_COUNT - MIN_ACCUMULATION_COUNT) * value
						/ ADC_MAX);
	}

	//���� �����
	value = ADC_Buf[1];
	if (value >= 0 && value < ADC_MAX) {
		configuration.pauseCount = MIN_PAUSE_COUNT
				+ ((MAX_PAUSE_COUNT - MIN_PAUSE_COUNT) * value / ADC_MAX);
	}

	//���� ������������� ������
	value = ADC_Buf[2];
	if (value >= 0 && value < ADC_MAX) {
		configuration.twoWavesCount =
				MIN_TWO_WAVES_COUNT
						+ ((MAX_TWO_WAVES_COUNT - MIN_TWO_WAVES_COUNT) * value
								/ ADC_MAX);
	}

	//����
	value = ADC_Buf[3];
	if (value >= 0 && value < ADC_MAX) {
		if (value >= ADC_MIDDLE) {
			value -= ADC_MIDDLE;
			//�������� MAX_SHIFT �� ���������� �����������
			configuration.phaseShift = MAX_SHIFT * value / ADC_MIDDLE;
		} else {
			value = ADC_MIDDLE - value;
			configuration.phaseShift = (-1) * MAX_SHIFT * value / ADC_MIDDLE;
		}
	}

}
