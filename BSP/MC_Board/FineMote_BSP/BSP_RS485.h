/*******************************************************************************
 * Copyright (c) 2025.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_BSP_RS485_H
#define FINEMOTE_BSP_RS485_H

#include "Board.h"

template <size_t ID>
class RS485FlowControl {
public:
    static void Switch2Rx() {
        HAL_GPIO_WritePin(BSP_RS485FlowCtrlPortList[ID], BSP_RS485FlowCtrlPinList[ID], GPIO_PIN_RESET);
    }

    static void Switch2Tx() {
        HAL_GPIO_WritePin(BSP_RS485FlowCtrlPortList[ID], BSP_RS485FlowCtrlPinList[ID], GPIO_PIN_SET);
    }
};

#endif
