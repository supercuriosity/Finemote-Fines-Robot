/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_SBUS_H
#define FINEMOTE_SBUS_H

#include "ProjectConfig.h"

#ifdef SBUS_MODULE

#include "string.h"
#include "DeviceBase.h"

class SBUS : public DeviceBase{
public:
    SBUS();
    ~SBUS();
    virtual void Handle(void) = 0;
    void Init(void);
private:
    static constexpr uint32_t SBUS_RX_BUF_NUM = 50;
protected:
    volatile uint8_t rxBuff[2][SBUS_RX_BUF_NUM];
    int32_t channel[16];
};

#endif


#endif //FINEMOTE_SBUS_H
