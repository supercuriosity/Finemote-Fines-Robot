/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_REMOTECONTROL_H
#define FINEMOTE_REMOTECONTROL_H

#include "ProjectConfig.h"

#ifdef REMOTECONTROL_MODULE

#include "DeviceBase.h"
#include "Bus/UART_Base.h"

class RemoteControl {
public:
    enum SWITCH_STATE_E{
        UP_POS = 1u, MID_POS = 3u, DOWN_POS = 2u
    };
    struct Mouse_t{
        int32_t x, y, z, pressL, pressR;
    };
    struct Keyboard_t{
        uint32_t v;
    };
    struct RemoteControlData_t {
        float leftCol, leftRol,lC,lR;
        float rightCol, rightRol,rC,rR;
        float dialWheel;
        uint32_t pat;
        uint32_t sRight, sLeft;
        uint32_t sA,sB,sC,sD;
        uint32_t buttonA, buttonD;
        Mouse_t mouse;
        Keyboard_t keyboard;
    };

    virtual void Decode(uint8_t* data, uint16_t length) = 0;

    const RemoteControlData_t& GetInfo() {
        return info;
    };

protected:
    RemoteControlData_t info = {};
};

#endif

#endif //FINEMOTE_REMOTECONTROL_H
