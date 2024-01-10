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
private:
    inline static std::list<DeviceBase*> deviceList = {};

};


#endif //FINEMOTE_DEVICEBASE_H
