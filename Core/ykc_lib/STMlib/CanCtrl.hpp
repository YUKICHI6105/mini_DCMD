/*
 *  CanCtrlPro.hpp
 *
 *  Created on: 2023/04/13
 *      Author: ykc
 */

#include <cstdint>
#include <array>
#include <optional>
#include <main.h>
#include <stm32f1xx_hal_can.h>
#include <stm32f1xx_hal_gpio.h>

extern "C"{
	extern CAN_HandleTypeDef hcan;
}

class CanCtrl {
private:
	uint32_t BID = 0x7ff;
	uint8_t TxData[8];//HAL関数用挿入口
	uint8_t RxData[8];//HAL関数用受け取り口
	CAN_TxHeaderTypeDef TxHeader;//送信用フレーム設定？
	CAN_RxHeaderTypeDef RxHeader;//受信用フレーム設定？
	CAN_FilterTypeDef filter;//受信時に中身を仕分けるためのパラメーター設定？
public:
	using DataType = std::array<std::uint8_t, 8>;
	CanCtrl();
	void SetBID(uint32_t inBID);
	uint32_t GetBID();
	bool Cantx(uint32_t TID,uint8_t data[8]);//送信関数
	auto Canrx() -> std::optional<DataType>;//受信関数
	bool Canrx(uint32_t& RID,uint8_t data[8]);//受信関数(エラー判定のみ)内容は引数に入れ込む。
	void Canrx(uint8_t data[8],bool& RxError);
};

CanCtrl::CanCtrl(){
	TxHeader.RTR = CAN_RTR_DATA;            // フレームタイプはデータフレーム
	TxHeader.IDE = CAN_ID_STD;              // 標準ID(11ﾋﾞｯﾄ)
	TxHeader.DLC = 8;                       // データ長は8バイトに
	TxHeader.TransmitGlobalTime = DISABLE;  // ???
	filter.FilterIdHigh         = BID << 5;                 // フィルターIDの上位16ビット
	filter.FilterIdLow          = 0;                        // フィルターIDの下位16ビット
	filter.FilterMaskIdHigh     = 0x7fc << 5;               // フィルターマスクの上位16ビット
	filter.FilterMaskIdLow      = 0b110;                    // フィルターマスクの下位16ビット
	//フィルターマスクは1が確定させたいところで0が無視したいところ
	filter.FilterScale          = CAN_FILTERSCALE_32BIT;    // フィルタースケール
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;         // フィルターに割り当てるFIFO
	filter.FilterBank           = 0;                        // フィルターバンクNo
	filter.FilterMode           = CAN_FILTERMODE_IDMASK;    // フィルターモード
	filter.SlaveStartFilterBank = 14;                       // スレーブCANの開始フィルターバンクNo
	filter.FilterActivation     = ENABLE;                   // フィルター無効／有効
	HAL_CAN_ConfigFilter(&hcan, &filter);
}

void CanCtrl::SetBID(uint32_t inBID){
	BID = inBID;
	filter.FilterIdHigh         = BID << 5;                 // フィルターIDの上位16ビット
	HAL_CAN_ConfigFilter(&hcan, &filter);
}

uint32_t CanCtrl::GetBID(){
	return BID;
}

bool CanCtrl::Cantx(uint32_t TID/*送信用ID*/,uint8_t data[8]/*送信内容*/){
	TxHeader.StdId = TID;
	if(0 < HAL_CAN_GetTxMailboxesFreeLevel(&hcan)){
		TxData[0] = data[0];
		TxData[1] = data[1];
		TxData[2] = data[2];
		TxData[3] = data[3];
		TxData[4] = data[4];
		TxData[5] = data[5];
		TxData[6] = data[6];
		TxData[7] = data[7];

		uint32_t TxMailbox;//ボックス番号(ここに返ってくる)
		if (HAL_CAN_AddTxMessage(&hcan,&TxHeader,TxData,&TxMailbox) == HAL_OK){
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
			HAL_Delay(1000);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

auto CanCtrl::Canrx() -> std::optional<DataType>{
	 std::optional<DataType> rx_data{};
	if (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, rx_data->data()) == HAL_OK){
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
		return rx_data;
	}
	else
	{
		return std::nullopt;
	}
}

bool CanCtrl::Canrx(uint32_t& RID,uint8_t data[8]){
	if (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, data) == HAL_OK){
		RID = RxHeader.StdId;
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
		return false;
	}
	else{
		return true;
	}
}

void CanCtrl::Canrx(uint8_t data[8],bool& RxError){
	if (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, data) == HAL_OK){
		RxError = false;
	}
	else{
		RxError = true;
	}
}

