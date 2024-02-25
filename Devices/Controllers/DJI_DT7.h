/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_DJI_DT7_H
#define FINEMOTE_DJI_DT7_H


#include "ProjectConfig.h"

#ifdef DJI_DT7_MODULE

#include "RemoteControl.h"
#include "SBUS.h"
#include "DBUS.h"

class DT7 : public RemoteControl, public DBUS{
public:
    DT7();
    ~DT7();
    void Handle(void) override;
    void Decode(void) override;
private:
    static constexpr int32_t RC_CH_VALUE_OFFSET = 1024;
};

#endif

#endif //FINEMOTE_DJI_DT7_H
