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

extern uint8_t usart3_RxFlag;

class SBUS{
public:
    SBUS();
    ~SBUS();
    void Receive(void);
    int32_t channel[16];
private:
    static constexpr uint32_t SBUS_RX_BUF_NUM = 50;
    volatile uint8_t rxBuff[2][SBUS_RX_BUF_NUM];
};

#endif


#endif //FINEMOTE_SBUS_H
