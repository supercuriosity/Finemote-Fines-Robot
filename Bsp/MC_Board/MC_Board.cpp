
/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "MC_Board.h"

UART_HandleTypeDef *uartHandleList[4] = {&huart3, &huart1, &huart2, &huart5};
GPIO_TypeDef *uartTxPortList[4] = {nullptr, GPIOC, GPIOB, nullptr};
uint16_t uartTxPinList[4] = {NULL, GPIO_PIN_15, GPIO_PIN_3, NULL};
