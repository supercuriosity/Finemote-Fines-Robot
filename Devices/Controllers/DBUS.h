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

class DBUS : public DeviceBase{
public:
    DBUS();
    ~DBUS();
    virtual void Handle(void) = 0;
    void Init(void);
private:
    static constexpr uint32_t DBUS_RX_BUF_NUM = 36;
protected:
    volatile uint8_t rxBuff[2][DBUS_RX_BUF_NUM];
    int32_t channel[4];
    uint32_t switches[2];
    int32_t mouse[5];
    uint32_t keyboard;
    int16_t wheel;
};

#endif

#endif //FINEMOTE_DBUS_H
