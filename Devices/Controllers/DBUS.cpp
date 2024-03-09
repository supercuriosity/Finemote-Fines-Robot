/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "DBUS.h"

#ifdef DBUS_MODULE

DBUS::DBUS() {
    Enable_DoubleMemBuff(&DMA_SBUS, (uint32_t)&rxBuff[0][0], (uint32_t)&rxBuff[1][0], DBUS_RX_BUF_NUM);
}

DBUS::~DBUS() {

}

void DBUS::Receive() {
    if (usart3_RxFlag == 0) {
        return;
    }

    if ((DMA_SBUS.Instance->CR & DMA_SxCR_CT) == RESET) { // Current memory buffer used is Memory 0, Memory 1 is modifiable

        channel[0] = (rxBuff[1][0] | (rxBuff[1][1] << 8)) & 0x07FF;
        channel[1] = ((rxBuff[1][1] >> 3) | (rxBuff[1][2] << 5)) & 0x07FF;
        channel[2] = ((rxBuff[1][2] >> 6) | (rxBuff[1][3] << 2) | (rxBuff[1][4] << 10)) & 0x07FF;
        channel[3] = ((rxBuff[1][4] >> 1) | (rxBuff[1][5] << 7)) & 0x07FF;

        switches[0] = ((rxBuff[1][5] >> 4) & 0x000C) >> 2;
        switches[1] = (rxBuff[1][5] >> 4) & 0x0003;

        mouse[0] = rxBuff[1][6] | (rxBuff[1][7] << 8); // X
        mouse[1] = rxBuff[1][8] | (rxBuff[1][9] << 8); // Y
        mouse[2] = rxBuff[1][10] | (rxBuff[1][11] << 8); // Z
        mouse[3] = rxBuff[1][12]; // Left Press
        mouse[4] = rxBuff[1][13]; // Right Press

        keyboard = rxBuff[1][14] | (rxBuff[1][15] << 8);

        wheel = rxBuff[1][16] | (rxBuff[1][17] << 8);

        usart3_RxFlag = 0;
    }
    else{ // Current memory buffer used is Memory 1, Memory 0 is modifiable
        channel[0] = (rxBuff[0][0] | (rxBuff[0][1] << 8)) & 0x07FF;
        channel[1] = ((rxBuff[0][1] >> 3) | (rxBuff[0][2] << 5)) & 0x07FF;
        channel[2] = ((rxBuff[0][2] >> 6) | (rxBuff[0][3] << 2) | (rxBuff[0][4] << 10)) & 0x07FF;
        channel[3] = ((rxBuff[0][4] >> 1) | (rxBuff[0][5] << 7)) & 0x07FF;

        switches[0] = ((rxBuff[0][5] >> 4) & 0x000C) >> 2;
        switches[1] = (rxBuff[0][5] >> 4) & 0x0003;

        mouse[0] = rxBuff[0][6] | (rxBuff[0][7] << 8); // X
        mouse[1] = rxBuff[0][8] | (rxBuff[0][9] << 8); // Y
        mouse[2] = rxBuff[0][10] | (rxBuff[0][11] << 8); // Z
        mouse[3] = rxBuff[0][12]; // Left Press
        mouse[4] = rxBuff[0][13]; // Right Press

        keyboard = rxBuff[0][14] | (rxBuff[0][15] << 8);

        wheel = rxBuff[0][16] | (rxBuff[0][17] << 8);

        usart3_RxFlag = 0;
    }
}

#endif