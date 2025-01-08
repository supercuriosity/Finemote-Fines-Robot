/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "UARTBaseLite.h"

#include "UART_Base.h"

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
#ifdef __ROBOMASTER_C
    if (huart == &huart6) {
        UARTBaseLite<2>::GetInstance().RxHandle(Size);
    }
    else if (huart == &huart3) {
        UARTBaseLite<0>::GetInstance().RxHandle(Size);
    }
    else if (huart == &huart1){
        UARTBaseLite<1>::GetInstance().RxHandle(Size);
    }
#endif

#ifdef __MC_BOARD
    if (huart == &huart5) {
        UARTBaseLite<5>::GetInstance().RxHandle(Size);
    }
    else if (huart == &huart3) {
        UARTBaseLite<3>::GetInstance().RxHandle(Size);
    }
    else if (huart == &huart1){
        UARTBaseLite<1>::GetInstance().RxHandle(Size);
    }
#endif
}
