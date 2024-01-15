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
     * 设置分频系数，使设备的执行频率动态可调
     * @param divisionFactor
     */
    void SetDivisionFactor(uint32_t divisionFactor);
private:
    uint32_t cnt{0};
    uint32_t divisionFactor{1};

    static std::list<DeviceBase*> deviceList;
};


#endif //FINEMOTE_DEVICEBASE_H
