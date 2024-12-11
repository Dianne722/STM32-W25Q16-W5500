#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "main.h"
#include "stdbool.h"

void HAL_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

struct keys
{
	uchar judge_sta;
	bool key_sta;
	bool single_flag;
	bool long_flag;
	uint key_time;
};



#endif
