#include <main.h>
#include "stm32f1xx_hal_tim.h"

extern "C"{
	extern TIM_HandleTypeDef htim1;
	extern TIM_HandleTypeDef htim3;
}

void main_cpp(/* いらないvoid*/)
{
	HAL_TIM_PWM_Start (&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start (&htim3, TIM_CHANNEL_1);
	HAL_Delay(1000);
	HAL_TIM_PWM_Stop (&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop (&htim3, TIM_CHANNEL_1);
    HAL_Delay(1000);
}
