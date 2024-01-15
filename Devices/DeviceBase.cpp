/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "DeviceBase.h"

std::list<DeviceBase*> DeviceBase::deviceList={};

void DeviceBase::DevicesHandle() {
    auto tmp = deviceList.size();
    for (DeviceBase* devicePtr :deviceList){
        if (++(devicePtr->cnt)>= devicePtr->divisionFactor) {
            devicePtr->Handle();
            devicePtr->cnt = 0;
        }
    }
}

DeviceBase::DeviceBase() {
    auto tmp = deviceList.size();
    deviceList.push_back(this);
}

DeviceBase::~DeviceBase() {
    auto tmp = deviceList.size();
    deviceList.remove(this);
}

void DeviceBase::SetDivisionFactor(uint32_t divisionFactor) {
    DeviceBase::divisionFactor = divisionFactor;
}
