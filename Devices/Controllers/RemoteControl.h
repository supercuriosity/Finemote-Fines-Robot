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

class RemoteControl : public DeviceBase{
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
        float leftCol, leftRol;
        float rightCol, rightRol;
        float dialWheel;
        uint32_t pat;
        uint32_t sRight, sLeft;
        uint32_t sB, sC, sE, sF;
        uint32_t buttonA, buttonD;
        Mouse_t mouse;
        Keyboard_t keyboard;
    };
protected:
    RemoteControlData_t info;
public:
    RemoteControl();
    ~RemoteControl();
    void Handle(void) override;
    virtual void Decode(UART_Task_t _data) = 0;
    inline const RemoteControlData_t& GetInfo() {
        return info;
    };
};

#endif

#endif //FINEMOTE_REMOTECONTROL_H
