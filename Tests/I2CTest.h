
/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_I2CTEST_H
#define FINEMOTE_I2CTEST_H
#include "ProjectConfig.h"
#ifdef I2CTEST_COMPONENTS
#include "Bus/I2C_Base.h"
/**
 * I2CBase类的示例用法
 */
class I2CTest {
    I2C_Agent<2> sensorI2C;
    uint8_t recievedMessage[2]{};
public:
    void recievedCallback(I2C_Task_t _data){
        uint16_t tmp;
        tmp = recievedMessage[0] +recievedMessage[1]<<8;
    };
    I2CTest():sensorI2C(0x40){
        //直接使用vector类型，发送小数据
        sensorI2C.Write({0x02,3});
        //大批数据或重复更新数据，需保证数据生命周期
        static uint8_t tmp[2];
        sensorI2C.Write(tmp,2);
        //读取数据，读取成功后调用回调函数
        auto recievedCallbackPack = [this](I2C_Task_t a){this->recievedCallback(a);};
        sensorI2C.Read(recievedMessage,2,recievedCallbackPack);
        //总线停顿,用于等待器件处理等场景
        sensorI2C.Delay(2);
    };
};

#endif
#endif //FINEMOTE_I2CTEST_H
