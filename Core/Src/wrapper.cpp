#include <string.h>
#include <main.h>
#include <../ykc_lib/STMlib/CanCtrl.hpp>
#include <../ykc_lib/STMlib/SPICtrl.hpp>
#include <stm32f1xx_hal_tim.h>
#include <stm32f1xx_hal_can.h>
#include <stm32f1xx_hal_uart.h>

extern "C"{
	extern TIM_HandleTypeDef htim1;
	extern TIM_HandleTypeDef htim3;
	extern UART_HandleTypeDef huart2;
}


uint8_t srxdata[8];
uint32_t sRID;
uint8_t msg[] = "hello world!\n\r";
uint8_t rmsg[]= "0b10";

void main_cpp(){
	CanCtrl C;
	HAL_CAN_Start(&hcan);
	uint8_t stxdata[8];
	stxdata[0] = 0x00;
	stxdata[1] = 0x11;
	stxdata[2] = 0x22;
	stxdata[3] = 0x33;
	stxdata[4] = 0x44;
	stxdata[5] = 0x55;
	stxdata[6] = 0x66;
	stxdata[7] = 0x77;
	uint8_t spidata[2];
	HAL_TIM_PWM_Start (&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start (&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start (&htim3, TIM_CHANNEL_1);
	while(1){
		C.Cantx(0x100,stxdata);
		C.Canrx(sRID,srxdata);
		HAL_UART_Transmit(&huart2, msg, strlen((char*)msg), 100);//(レジスタモジュール,データ,文字数,タイムアウト(つまりその時間経過したらエラー終了))
		HAL_UART_Receive(&huart2, rmsg, strlen((char*)rmsg), 100);//(レジスタモジュール,データ,文字数,タイムアウト(つまりその時間経過したらエラー終了))
		MPU9250(spidata,0x75);
		HAL_Delay(1000);
	}
}
