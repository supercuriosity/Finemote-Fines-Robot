/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_DEVICEBASE_H
#define FINEMOTE_DEVICEBASE_H

#include <list>

class DeviceBase {
public:
    virtual void Handle() = 0;
    static void DevicesHandle();

    DeviceBase();
    ~DeviceBase();
    /**
     * 惰性初始化，避免静态变量初始化顺序问题
     * @return
     */
    static std::list<DeviceBase*>& getDeviceList() {
        static std::list<DeviceBase*> deviceList;
        return deviceList;
    }
    /**
     * 设置分频系数，使设备的执行频率动态可调
     * @param divisionFactor
     */
    void SetDivisionFactor(uint32_t divisionFactor);
private:
    uint32_t cnt{0};
    uint32_t divisionFactor{1};


};


#endif //FINEMOTE_DEVICEBASE_H
