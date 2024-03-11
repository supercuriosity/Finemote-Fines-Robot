/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_RADIOMASTER_ZORRO_H
#define FINEMOTE_RADIOMASTER_ZORRO_H

#include "ProjectConfig.h"

#ifdef RADIOMASTER_ZORRO_MODULE

#include "RemoteControl.h"

class RadioMaster_Zorro : public RemoteControl{
public:
    RadioMaster_Zorro()
    {
        HALInit::GetInstance();
    }
    ~RadioMaster_Zorro();
    void Handle(void) override;
    void Decode(UART_Task_t _data) override;
private:
    static constexpr uint32_t SBUS_RX_BUF_NUM = 50;
    static constexpr int32_t RC_CH_VALUE_OFFSET = 1024;
    uint8_t rxBuff[SBUS_RX_BUF_NUM];
    int32_t channel[16];
    UART_Agent<6> remoteUart;
};

#endif

#endif //FINEMOTE_RADIOMASTER_ZORRO_H
