/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "DeviceBase.h"


void DeviceBase::DevicesHandle() {
    auto tmp = getDeviceList().size();
    for (DeviceBase* devicePtr :getDeviceList()){
        if (++(devicePtr->cnt)>= devicePtr->divisionFactor) {
            devicePtr->Handle();
            devicePtr->cnt = 0;
        }
    }
}

DeviceBase::DeviceBase() {
    getDeviceList().push_back(this);
	auto tmp = getDeviceList().size();
}

DeviceBase::~DeviceBase() {
    auto tmp = getDeviceList().size();
    getDeviceList().remove(this);
}

void DeviceBase::SetDivisionFactor(uint32_t divisionFactor) {
    DeviceBase::divisionFactor = divisionFactor;
}
