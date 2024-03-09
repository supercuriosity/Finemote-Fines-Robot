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
#include "SBUS.h"

class RadioMaster_Zorro : public RemoteControl{
public:
    RadioMaster_Zorro();
    ~RadioMaster_Zorro();
    void Handle(void) override;
    void Decode(void) override;
private:
    SBUS sbus;
    static constexpr int32_t RC_CH_VALUE_OFFSET = 1024;
};

#endif

#endif //FINEMOTE_RADIOMASTER_ZORRO_H
