/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "DeviceBase.h"


void DeviceBase::DevicesHandle() {
    for (DeviceBase* devicePtr :deviceList){
        devicePtr->Handle();
    }
}

DeviceBase::DeviceBase() {
    deviceList.push_back(this);
}

DeviceBase::~DeviceBase() {
    deviceList.remove(this);
}
