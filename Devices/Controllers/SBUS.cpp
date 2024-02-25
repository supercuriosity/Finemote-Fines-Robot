/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "SBUS.h"

#ifdef SBUS_MODULE

SBUS::SBUS() {
	HALInit::GetInstance();
}

SBUS::~SBUS() {

}

void SBUS::Init() {
    Enable_DoubleMemBuff(&DMA_SBUS, (uint32_t)&rxBuff[0][0], (uint32_t)&rxBuff[1][0], SBUS_RX_BUF_NUM);
}

void SBUS::Handle(void) {
    if ((DMA_SBUS.Instance->CR & DMA_SxCR_CT) == RESET) { // Current memory buffer used is Memory 0, Memory 1 is modifiable
        channel[0] = (rxBuff[1][1] | (rxBuff[1][2] << 8)) & 0x07FF;
        channel[1] = ((rxBuff[1][2] >> 3) | (rxBuff[1][3] << 5)) & 0x07FF;
        channel[2] = ((rxBuff[1][3] >> 6) | (rxBuff[1][4] << 2) | (rxBuff[1][5] << 10)) & 0x07FF;
        channel[3] = ((rxBuff[1][5] >> 1) | (rxBuff[1][6] << 7)) & 0x07FF;
        channel[4] = ((rxBuff[1][6] >> 4) | (rxBuff[1][7] << 4)) & 0x07FF;
        channel[5] = ((rxBuff[1][7] >> 7) | (rxBuff[1][8] << 1) | (rxBuff[1][8] << 9)) & 0x07FF;
        channel[6] = ((rxBuff[1][9] >> 2) | (rxBuff[1][10] << 6)) & 0x07FF;
        channel[7] = ((rxBuff[1][10] >> 5) | (rxBuff[1][11] << 3)) & 0x07FF;
        channel[8] = (rxBuff[1][12] | (rxBuff[1][13] << 8)) & 0x07FF;
        channel[9] = ((rxBuff[1][13] >> 3) | (rxBuff[1][14] << 5)) & 0x07FF;
        channel[10] = ((rxBuff[1][14] >> 6) | (rxBuff[1][15] << 2) | (rxBuff[1][16] << 10)) & 0x07FF;
        channel[11] = ((rxBuff[1][16] >> 1) | (rxBuff[1][17] << 7)) & 0x07FF;
        channel[12] = ((rxBuff[1][17] >> 4) | (rxBuff[1][18] << 4)) & 0x07FF;
        channel[13] = ((rxBuff[1][18] >> 7) | (rxBuff[1][19] << 1) | (rxBuff[1][20] << 9)) & 0x07FF;
        channel[14] = ((rxBuff[1][20] >> 2) | (rxBuff[1][21] << 6)) & 0x07FF;
        channel[15] = ((rxBuff[1][21] >> 5) | (rxBuff[1][22] << 3)) & 0x07FF;
    } else { // Current memory buffer used is Memory 1, Memory 0 is modifiable
        channel[0] = (rxBuff[0][1] | (rxBuff[0][2] << 8)) & 0x07FF;
        channel[1] = ((rxBuff[0][2] >> 3) | (rxBuff[0][3] << 5)) & 0x07FF;
        channel[2] = ((rxBuff[0][3] >> 6) | (rxBuff[0][4] << 2) | (rxBuff[0][5] << 10)) & 0x07FF;
        channel[3] = ((rxBuff[0][5] >> 1) | (rxBuff[0][6] << 7)) & 0x07FF;
        channel[4] = ((rxBuff[0][6] >> 4) | (rxBuff[0][7] << 4)) & 0x07FF;
        channel[5] = ((rxBuff[0][7] >> 7) | (rxBuff[0][8] << 1) | (rxBuff[0][8] << 9)) & 0x07FF;
        channel[6] = ((rxBuff[0][9] >> 2) | (rxBuff[0][10] << 6)) & 0x07FF;
        channel[7] = ((rxBuff[0][10] >> 5) | (rxBuff[0][11] << 3)) & 0x07FF;
        channel[8] = (rxBuff[0][12] | (rxBuff[0][13] << 8)) & 0x07FF;
        channel[9] = ((rxBuff[0][13] >> 3) | (rxBuff[0][14] << 5)) & 0x07FF;
        channel[10] = ((rxBuff[0][14] >> 6) | (rxBuff[0][15] << 2) | (rxBuff[0][16] << 10)) & 0x07FF;
        channel[11] = ((rxBuff[0][16] >> 1) | (rxBuff[0][17] << 7)) & 0x07FF;
        channel[12] = ((rxBuff[0][17] >> 4) | (rxBuff[0][18] << 4)) & 0x07FF;
        channel[13] = ((rxBuff[0][18] >> 7) | (rxBuff[0][19] << 1) | (rxBuff[0][20] << 9)) & 0x07FF;
        channel[14] = ((rxBuff[0][20] >> 2) | (rxBuff[0][21] << 6)) & 0x07FF;
        channel[15] = ((rxBuff[0][21] >> 5) | (rxBuff[0][22] << 3)) & 0x07FF;
    }
}

#endif