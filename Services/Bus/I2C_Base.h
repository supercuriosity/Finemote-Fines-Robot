/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_I2C_BASE_H
#define FINEMOTE_I2C_BASE_H

#include "ProjectConfig.h"


#ifdef I2C_BASE_MODULE

#include <queue>
#include <list>
#include <utility>
#include <vector>

#include "DeviceBase.h"



using ByteVector = std::vector<uint8_t>;

typedef enum : uint8_t {
    I2CREAD =0,
    I2CWRITE,
    WRITE_READ,
    READ_VECTOR,
    WRITE_VECTOR,
    WRITE_READ_VECTOR,
    DELAY,
} I2C_Task_e;

typedef struct I2C_Task_t{
    uint8_t taskID;
    uint8_t addr;
    I2C_Task_e task;
    uint8_t reg;
    uint8_t * bufPtr;
    uint32_t size;
    ByteVector data;
    std::function<void(I2C_Task_t)> callbackFuncPtr = nullptr;

} I2C_Task_t;

using I2C_CallbackFuncPtr = std::function<void(I2C_Task_t)>;



template <uint8_t busID>
class I2C_Bus :public DeviceBase{//TODO 防止一条无效指令卡死总线，设置超时功能
public:
    static I2C_Bus& GetInstance() {
        static I2C_Bus<busID> instance;
        return instance;
    }
    I2C_Bus(const I2C_Bus &) = delete;
    I2C_Bus& operator=(const I2C_Bus&) = delete;
    void Handle() override{
        enum class Handle_State_e{
            READY,
            RETRYING,
        };
        static Handle_State_e handleStateE = Handle_State_e::READY;

        uint32_t taskSize =taskQueue.size();
        uint32_t resourceSize = resourceList.size();
		//此处涉及三个状态量：两个容器是否为空，以及上次发送是否成功。对应到三种处理：直接重发，资源入队后重发，直接退出
        if( handleStateE == Handle_State_e::READY && taskSize == 0)return;//上一次发送成功说明无待重发，任务列表空说明无待发
        HAL_StatusTypeDef status = HAL_OK;
        if(handleStateE == Handle_State_e::READY ){//上一次发送成功且任务列表非空，进行资源入队
            I2C_Task_t tmpTask = taskQueue.front();
            resourceList.push_back(std::move(tmpTask));
        }
        auto tmpResource = resourceList.back();
        uint8_t tmpVector[8];
        std::copy(tmpResource.data.begin(),tmpResource.data.end(),tmpVector);
        switch (resourceList.back().task) {
            case I2CREAD:
                status =  HAL_I2C_Master_Receive_IT(&USER_I2C, resourceList.back().addr << 1,
                                                    resourceList.back().bufPtr, resourceList.back().size);
                break;
            case READ_VECTOR:
                status =  HAL_I2C_Master_Receive_IT(&USER_I2C, resourceList.back().addr << 1,
                                                    resourceList.back().data.data(), resourceList.back().data.size());
                break;//TODO 维护多个可用的I2C总线
                status = HAL_I2C_Master_Transmit_IT(&USER_I2C, resourceList.back().addr << 1 | 0x01,
                                                    resourceList.back().bufPtr, resourceList.back().size);
            case I2CWRITE:
                break;
            case WRITE_VECTOR:
                status = HAL_I2C_Master_Transmit_IT(&USER_I2C, resourceList.back().addr << 1 | 0x01,
                                                    resourceList.back().data.data(), resourceList.back().data.size());
                break;
            case WRITE_READ:
                status = HAL_I2C_Mem_Read_IT(&USER_I2C, resourceList.back().addr << 1, resourceList.back().reg,
                                             I2C_MEMADD_SIZE_8BIT, resourceList.back().bufPtr,
                                             resourceList.back().size);
                break;
            case WRITE_READ_VECTOR:
                status = HAL_I2C_Mem_Read_IT(&USER_I2C, resourceList.back().addr << 1, resourceList.back().reg,
                                             I2C_MEMADD_SIZE_8BIT, resourceList.back().data.data(),
                                             resourceList.back().data.size());
                break;
            case DELAY:
                if (handleStateE == Handle_State_e::READY) {
                    status = HAL_BUSY;
                    resourceList.back().size = resourceList.back().size + HAL_GetTick();
                }else{
                    if (HAL_GetTick()>resourceList.back().size){
                        status = HAL_OK;
                        resourceList.pop_back();
                    }else{
                        status = HAL_BUSY;
                    }
                }
                break;
        }
        if(status != HAL_OK) {
            handleStateE = Handle_State_e::RETRYING;
        }else{
			taskQueue.pop();
            handleStateE = Handle_State_e::READY;
        }
    }
    enum class Callback_e{
        MASTER_TX,
        MASTER_RX,
        MEM_READ,
        ERROR_CALL
    };
    void CallbackHandle(Callback_e callbackE){
        switch (callbackE) {
            case Callback_e::MASTER_TX:
                break;
            case Callback_e::MASTER_RX:
            case Callback_e::MEM_READ:
                if (resourceList.front().callbackFuncPtr != nullptr){
                    resourceList.front().callbackFuncPtr(resourceList.front());
                }
                break;
            case Callback_e::ERROR_CALL:
                break;
        }
        resourceList.pop_front();
    }

    std::queue<I2C_Task_t> taskQueue;
    std::list<I2C_Task_t> resourceList;
    uint8_t taskID = 0;

private:
    I2C_Bus() {
        HALInit::GetInstance();
    }
    bool isFree;
};

/**
 * I2C代理
 * @tparam busID //TODO 当不同BSP支持不同数目，不同标号的I2C总线时，busID可能出现混淆定义
 */
template <uint8_t busID/*, uint8_t bufferSize*/>
class I2C_Agent {
public:
    /**
     *
     * @param _addr 这里需要的是不带读写位的地址，即实际为6位地址
     * @param i2CBusIstance
     */
    explicit I2C_Agent(uint8_t _addr) : addr(_addr),i2CBusRef(I2C_Bus<busID>::GetInstance()) {
        //I2C_Bus<busID>::Init();
        //HAL_I2C_IsDeviceReady(&hi2c2, addr << 1, 10, 1000);
    }

    void Read(uint8_t * _bufPtr,uint8_t _size,I2C_CallbackFuncPtr callPtr = nullptr){
        if (i2CBusRef.taskQueue.size() >= I2C_AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = I2CREAD;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = _size;
        tmpTask.callbackFuncPtr = callPtr;
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }


    void Read(ByteVector dataVector,I2C_CallbackFuncPtr callPtr = nullptr){
        if (i2CBusRef.taskQueue.size() >= I2C_AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = READ_VECTOR;
        tmpTask.data = std::move(dataVector);
        tmpTask.callbackFuncPtr = callPtr;
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }

   /* void Write(uint8_t * _bufPtr,uint8_t _size,I2C_CallbackFuncPtr callPtr = nullptr){
        if (i2CBusRef.taskQueue.size() >= I2C_AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = I2CWRITE;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = _size;
        tmpTask.callbackFuncPtr = callPtr;
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }

    void Write(ByteVector dataVector){
        if (i2CBusRef.taskQueue.size() >= I2C_AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = WRITE_VECTOR;
        tmpTask.data = std::move(dataVector);
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }

    void WriteRead(uint8_t _reg,uint8_t * _bufPtr,uint8_t _size,I2C_CallbackFuncPtr callPtr = nullptr){
        if (i2CBusRef.taskQueue.size() >= I2C_AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = WRITE_READ;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = _size;
        tmpTask.reg = _reg;
        tmpTask.callbackFuncPtr = std::move(callPtr);
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }

    void WriteRead(uint8_t reg,ByteVector dataVector,I2C_CallbackFuncPtr callPtr = nullptr){
        if (i2CBusRef.taskQueue.size() >= I2C_AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = WRITE_READ_VECTOR;
        tmpTask.data = std::move(dataVector);
        tmpTask.reg = reg;
        tmpTask.callbackFuncPtr = std::move(callPtr);
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }
/**
 * 将总线完全静默一段时间 //todo 实际需求大部分是对同一器件的操作挂起
 * @param _ms
 */
    void Delay(uint32_t _ms){
        if (i2CBusRef.taskQueue.size() >= I2C_AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = DELAY;
        tmpTask.size = _ms;
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }

private:
    uint8_t addr;
    //uint8_t buf[bufferSize] = {};
    I2C_Bus<busID>& i2CBusRef;

};

template class I2C_Bus<2>;//显式实例化能解决部分奇怪的链接时报错

#endif //I2C_Base_Module

#endif //FINEMOTE_I2C_BASE_H
