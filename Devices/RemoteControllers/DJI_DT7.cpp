/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "DJI_DT7.h"

#ifdef DJI_DT7_MODULE

/*void DT7::Decode(UART_Task_t _data) {

    info.sLeft = (SWITCH_STATE_E)((rxBuff[5] >> 4) & 0x000C) >> 2;
    info.sRight = (SWITCH_STATE_E)(rxBuff[5] >> 4) & 0x0003;

    info.mouse.x = rxBuff[6] | (rxBuff[7] << 8); // X
    info.mouse.y = rxBuff[8] | (rxBuff[9] << 8); // Y
    info.mouse.z = rxBuff[10] | (rxBuff[11] << 8); // Z
    info.mouse.pressL = rxBuff[12]; // Left Press
    info.mouse.pressR = rxBuff[13]; // Right Press

    info.keyboard.v = rxBuff[14] | (rxBuff[15] << 8);

    info.rightRol = ((rxBuff[0] | (rxBuff[1] << 8)) & 0x07FF - RC_CH_VALUE_OFFSET) / 660.0f;
    info.rightCol = (((rxBuff[1] >> 3) | (rxBuff[2] << 5)) & 0x07FF - RC_CH_VALUE_OFFSET) / 660.0f;
    info.leftRol = (((rxBuff[2] >> 6) | (rxBuff[3] << 2) | (rxBuff[4] << 10)) & 0x07FF - RC_CH_VALUE_OFFSET) / 660.0f;
    info.leftCol = (((rxBuff[4] >> 1) | (rxBuff[5] << 7)) & 0x07FF - RC_CH_VALUE_OFFSET) / 660.0f;
    info.dialWheel = rxBuff[16] | (rxBuff[17] << 8) - RC_CH_VALUE_OFFSET;

//    HAL_IWDG_Refresh(&hiwdg);//喂狗
}*/

#endif
