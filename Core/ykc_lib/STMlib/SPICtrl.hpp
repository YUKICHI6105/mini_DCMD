/*
 *  SPICtrl.hpp
 *
 *  Created on: 2023/04/13
 *      Author: ykc
 */

#include <main.h>
#include <stm32f1xx_hal_spi.h>

extern "C"{
    extern SPI_HandleTypeDef hspi1;
}

bool MPU9250(uint8_t Rdata[2],uint8_t reg/*レジスタの番号を書くとその情報がRdataに帰ってくる*/){
	uint8_t rx_data[2];
	uint8_t tx_data[2];

	tx_data[0] = reg | 0x80;
	tx_data[1] = 0x00;

	HAL_GPIO_WritePin(GPIOB,SPI_CS_Pin,GPIO_PIN_RESET);
	if (HAL_SPI_TransmitReceive(&hspi1,tx_data,rx_data,2,1) == HAL_OK){
		Rdata[0] = rx_data[0];
		Rdata[1] = rx_data[1];
		HAL_GPIO_WritePin(GPIOB,SPI_CS_Pin,GPIO_PIN_SET);
		return true;
	}
	else{
		HAL_GPIO_WritePin(GPIOB,SPI_CS_Pin,GPIO_PIN_SET);
		return false;
	}
}
