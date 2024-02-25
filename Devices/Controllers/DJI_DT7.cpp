/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "DJI_DT7.h"

#ifdef DJI_DT7_MODULE

DT7 dt7;

DT7::DT7() {

}

DT7::~DT7() {

}

void DT7::Decode(void) {

    info.rightRol = (channel[0] - RC_CH_VALUE_OFFSET) / 660.0f;
    info.rightCol = (channel[1] - RC_CH_VALUE_OFFSET) / 660.0f;
    info.leftRol = (channel[2] - RC_CH_VALUE_OFFSET) / 660.0f;
    info.leftCol = (channel[3] - RC_CH_VALUE_OFFSET) / 660.0f;

    info.sLeft = (SWITCH_STATE_E) switches[0];
    info.sRight = (SWITCH_STATE_E) switches[1];

    info.mouse.x = mouse[0];
    info.mouse.y = mouse[1];
    info.mouse.z = mouse[2];
    info.mouse.pressL = mouse[3];
    info.mouse.pressR = mouse[4];

    info.keyboard.v = keyboard;

    info.dialWheel = wheel - RC_CH_VALUE_OFFSET;

//    HAL_IWDG_Refresh(&hiwdg);//喂狗
}

void DT7::Handle() {
    DBUS::Handle();
    Decode();
}

#endif