/*******************************************************************************
* Copyright (c) 2025.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "Bus/UART_Base.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// 发送完成中断回调函数
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    FineMoteAux_UART<>::OnTxComplete(huart);
}

// 接收中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    uint16_t receivedSize = huart->RxXferSize;
    FineMoteAux_UART<>::OnRxComplete(huart, receivedSize);
}

// 出错中断回调函数
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    FineMoteAux_UART<>::OnRxComplete(huart, 0);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
    FineMoteAux_UART<>::OnRxComplete(huart, size);
}

#ifdef __cplusplus
}
#endif
