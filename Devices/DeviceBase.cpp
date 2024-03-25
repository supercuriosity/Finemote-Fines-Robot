/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "DeviceBase.h"
#include "ProjectConfig.h"

uint32_t DeviceBase::baseFre = 0;

void DeviceBase::DevicesHandle() {
    //auto tmp = getDeviceList().size()
    static uint32_t stamp = HAL_GetTick();
    static uint32_t cnt = 0;
    uint32_t diff = HAL_GetTick()-stamp;
    cnt++;
    if (diff>=1000){
        baseFre = cnt;
        stamp = HAL_GetTick();
        cnt = 0;
    }
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
