/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "UART_Base.h"

#include "UARTBaseLite.h"

#ifdef UART_BASE_MODULE


std::map<UART_HandleTypeDef*,UART_Base*>& GetUartHandle_BusMap() {
    static std::map<UART_HandleTypeDef*,UART_Base*> uartHandle_BusMap;
    return uartHandle_BusMap;
}

#ifdef __cplusplus
extern "C" {
#endif

// 发送完成中断回调函数
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if(huart == &huart5) {
        UARTBaseLite<5>::GetInstance().TxLoader();
    }
    GetUartHandle_BusMap()[huart]->CallbackHandle(UART_Base::Callback_e::WRITE);
    //UART_Bus<0>::GetInstance().CallbackHandle(UART_Bus<0>::Callback_e::WRITE);
}
// 接收中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    GetUartHandle_BusMap()[huart]->CallbackHandle(UART_Base::Callback_e::READ);
    //UART_Bus<0>::GetInstance().CallbackHandle(UART_Bus<0>::Callback_e::READ);
}

// 出错中断回调函数
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    GetUartHandle_BusMap()[huart]->CallbackHandle(UART_Base::Callback_e::ERROR_CALL);
}

#ifdef __cplusplus
}
#endif

#endif //UART_BASE_MODULE