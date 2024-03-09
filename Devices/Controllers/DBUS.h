/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_DBUS_H
#define FINEMOTE_DBUS_H

#include "ProjectConfig.h"

#ifdef DBUS_MODULE

#include "DeviceBase.h"

extern volatile uint8_t usart3_RxFlag;

class DBUS{
public:
    DBUS();
    ~DBUS();
    void Receive(void);
    int32_t channel[4];
    uint32_t switches[2];
    int32_t mouse[5];
    uint32_t keyboard;
    int16_t wheel;
private:
    static constexpr uint32_t DBUS_RX_BUF_NUM = 36;
    volatile uint8_t rxBuff[2][DBUS_RX_BUF_NUM];

};

#endif

#endif //FINEMOTE_DBUS_H
