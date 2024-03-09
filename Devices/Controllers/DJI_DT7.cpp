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

    info.rightRol = (dbus.channel[0] - RC_CH_VALUE_OFFSET) / 660.0f;
    info.rightCol = (dbus.channel[1] - RC_CH_VALUE_OFFSET) / 660.0f;
    info.leftRol = (dbus.channel[2] - RC_CH_VALUE_OFFSET) / 660.0f;
    info.leftCol = (dbus.channel[3] - RC_CH_VALUE_OFFSET) / 660.0f;

    info.sLeft = (SWITCH_STATE_E) dbus.switches[0];
    info.sRight = (SWITCH_STATE_E) dbus.switches[1];

    info.mouse.x = dbus.mouse[0];
    info.mouse.y = dbus.mouse[1];
    info.mouse.z = dbus.mouse[2];
    info.mouse.pressL = dbus.mouse[3];
    info.mouse.pressR = dbus.mouse[4];

    info.keyboard.v = dbus.keyboard;

    info.dialWheel = dbus.wheel - RC_CH_VALUE_OFFSET;

//    HAL_IWDG_Refresh(&hiwdg);//喂狗
}

void DT7::Handle() {
    dbus.Receive();
    Decode();
}

#endif