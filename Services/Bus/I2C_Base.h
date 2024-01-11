/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_I2C_BASE_H
#define FINEMOTE_I2C_BASE_H

#include <queue>
#include <list>
#include <utility>
#include <vector>
#include "PeripheralDefination.h"

#define AGNET_TASK_MAX_NUM 20

using ByteVector = std::vector<uint8_t>;

typedef enum : uint8_t {
    READ = 0,
    WRITE,
    WRITE_READ,
    READ_VECTOR,
    WRITE_VECTOR,
    WRITE_READ_VECTOR,
} I2C_Task_e;

typedef struct I2C_Task_t{
    uint8_t taskID;
    uint8_t addr;
    I2C_Task_e task;
    uint8_t reg;
    uint8_t * bufPtr;
    uint16_t size;
    ByteVector data;
    std::function<void(I2C_Task_t)> callbackFuncPtr = nullptr;

} I2C_Task_t __packed;

using CallbackFuncPtr = std::function<void(I2C_Task_t)>;



template <uint8_t busID>
class I2C_Bus {
public:
    static I2C_Bus& GetInstance() {
        static I2C_Bus<busID> instance;
        return instance;
    }
    I2C_Bus(const I2C_Bus &) = delete;
    I2C_Bus& operator=(const I2C_Bus&) = delete;

    static void RTHandle() {
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
            case READ:
                status =  HAL_I2C_Master_Receive_IT(&USER_I2C, resourceList.back().addr << 1,
                                                    resourceList.back().bufPtr, resourceList.back().size);
                break;
            case READ_VECTOR:
                status =  HAL_I2C_Master_Receive_IT(&USER_I2C, resourceList.back().addr << 1,
                                                    resourceList.back().data.data(), resourceList.back().data.size());
                break;//TODO 维护多个可用的I2C总线
            case WRITE:
                status = HAL_I2C_Master_Transmit_IT(&USER_I2C, resourceList.back().addr << 1 | 0x01,
                                                    resourceList.back().bufPtr, resourceList.back().size);
                break;
            case WRITE_VECTOR:
                status = HAL_I2C_Master_Transmit_IT(&USER_I2C, resourceList.back().addr << 1 | 0x01,
                                                    resourceList.back().data.data(), resourceList.back().data.size());
                break;
            case WRITE_READ:
                status = HAL_I2C_Mem_Read_IT(&USER_I2C, resourceList.back().addr, resourceList.back().reg,
                                             I2C_MEMADD_SIZE_8BIT, resourceList.back().bufPtr,
                                             resourceList.back().size);
                break;
            case WRITE_READ_VECTOR:
                status = HAL_I2C_Mem_Read_IT(&USER_I2C, resourceList.back().addr, resourceList.back().reg,
                                             I2C_MEMADD_SIZE_8BIT, resourceList.back().data.data(),
                                             resourceList.back().data.size());
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
    static void CallbackHandle(Callback_e callbackE){
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

    static std::queue<I2C_Task_t> taskQueue;
    static std::list<I2C_Task_t> resourceList;
    inline static uint8_t taskID = 0;

private:
    I2C_Bus() {
        instanceVector.push_back(busID);
    }

    inline static std::vector<uint8_t> instanceVector= {};
    static bool isFree;
};

template <uint8_t busID/*, uint8_t bufferSize*/>
class I2C_Agent {
public:
    explicit I2C_Agent(uint8_t _addr,I2C_Bus<busID>& i2CBusIstance) : addr(_addr),i2CBusRef(i2CBusIstance) {
        //I2C_Bus<busID>::Init();
        //HAL_I2C_IsDeviceReady(&hi2c2, addr << 1, 10, 1000);
    }

    void Read(uint8_t * _bufPtr,uint8_t _size,CallbackFuncPtr callPtr = nullptr){
        if (i2CBusRef.taskQueue.size() >= AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = READ;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = _size;
        tmpTask.callbackFuncPtr = callPtr;
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }


    void Read(ByteVector dataVector,CallbackFuncPtr callPtr = nullptr){
        if (i2CBusRef.taskQueue.size() >= AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = READ_VECTOR;
        tmpTask.data = std::move(dataVector);
        tmpTask.callbackFuncPtr = callPtr;
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }

    void Write(uint8_t * _bufPtr,uint8_t _size,CallbackFuncPtr callPtr = nullptr){
        if (i2CBusRef.taskQueue.size() >= AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = WRITE;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = _size;
        tmpTask.callbackFuncPtr = callPtr;
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }

    void Write(ByteVector dataVector){
        if (i2CBusRef.taskQueue.size() >= AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = WRITE_VECTOR;
        tmpTask.data = std::move(dataVector);
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }

    void WriteRead(uint8_t _reg,uint8_t * _bufPtr,uint8_t _size,CallbackFuncPtr callPtr = nullptr){
        if (i2CBusRef.taskQueue.size() >= AGNET_TASK_MAX_NUM) return;
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

    void WriteRead(uint8_t reg,ByteVector dataVector,CallbackFuncPtr callPtr = nullptr){
        if (i2CBusRef.taskQueue.size() >= AGNET_TASK_MAX_NUM) return;
        I2C_Task_t tmpTask;
        tmpTask.taskID = ++i2CBusRef.taskID;
        tmpTask.addr = addr;
        tmpTask.task = WRITE_READ_VECTOR;
        tmpTask.data = std::move(dataVector);
        tmpTask.reg = reg;
        tmpTask.callbackFuncPtr = std::move(callPtr);
        i2CBusRef.taskQueue.push( std::move(tmpTask));
    }

private:
    uint8_t addr;
    //uint8_t buf[bufferSize] = {};
    I2C_Bus<busID>& i2CBusRef;

};

template class I2C_Bus<2>;//显式实例化能解决部分奇怪的链接时报错

#endif //FINEMOTE_I2C_BASE_H
