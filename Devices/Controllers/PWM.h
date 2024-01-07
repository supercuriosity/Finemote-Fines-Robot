/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/
#ifndef FINEMOTE_PWM_H
#define FINEMOTE_PWM_H

#include "PeripheralDefination.h"

template<uint32_t deviceID>
class PWM {
private:
    TIM_HandleTypeDef* timerPtr;
    uint32_t channel;
    static inline uint32_t currentDeviceCount = 0;
    static inline uint32_t maxDeviceCount = sizeof(pwmList)/sizeof(pwmList[0]);
public:
    explicit PWM(){
            static_assert(deviceID < maxDeviceCount);
            static_assert(pwmList[deviceID].activeFlag == 0);

            currentDeviceCount++;
            timerPtr = pwmList[deviceID].timerPtr;
            channel = pwmList[deviceID].channel;
            pwmList[deviceID].activeFlag = 1;
        };
    ~PWM(){
            pwmList[deviceID].activeFlag = 0;
            currentDeviceCount--;
        };
    void SetCompare(float _compare){
        uint32_t counterPeriod = __HAL_TIM_GET_AUTORELOAD(timerPtr);
                __HAL_TIM_SetCompare(timerPtr,channel,_compare * counterPeriod);
    }
    // TODO void SetFrequency();    //同一定时器的不同通道的频率不能分开设置
    void Start(){
        HAL_TIM_PWM_Start(timerPtr,channel);
    };
    void Stop(){
        HAL_TIM_PWM_Stop(timerPtr,channel);
    };

};


#endif //FINEMOTE_PWM_H
