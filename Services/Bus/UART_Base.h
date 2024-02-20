/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/


#ifndef FINEMOTE_UART_BASE_H
#define FINEMOTE_UART_BASE_H

#include "ProjectConfig.h"

#ifdef UART_BASE_MODULE

#include "DeviceBase.h"
#include <queue>
#include <vector>
#include <list>
#include <utility>

using ByteVector = std::vector<uint8_t>;

typedef enum : uint8_t {
    READ = 0,
    WRITE,
    READ_VECTOR,
    WRITE_VECTOR,
    DELAY,
} UART_Task_e;

typedef struct UART_Task_t{
    uint8_t taskID;
    UART_Task_e task;
    uint8_t * bufPtr;
    uint32_t size;
    ByteVector data;
    std::function<void(UART_Task_t)> callbackFuncPtr = nullptr;
} UART_Task_t;

using CallbackFuncPtr = std::function<void(UART_Task_t)>;

template <uint8_t busID>
class UART_Bus: public DeviceBase{
public:
    static UART_Bus& GetInstance() { // 单例模式，提供实例获取方法
        static UART_Bus<busID> instance;
        return instance;
    }

    // 单例模式，删除默认的拷贝构造函数
    UART_Bus(const UART_Bus&) = delete;
    UART_Bus& operator=(const UART_Bus&) = delete;

    // 串口任务处理
    void Handle() override{
        // 初始化
        enum class Handle_State_e{
            READY,
            RETRYING,
        };
        static Handle_State_e handleStateE = Handle_State_e::READY;
        uint32_t taskSize = taskQueue.size();
        uint32_t resourceSize = resourceList.size();

        // 处理，涉及三个状态量：两个容器是否为空，以及上次发送是否成功
        // 对应到三种处理：直接重发，资源入队后发送，直接退出
        if (handleStateE == Handle_State_e::READY && taskSize == 0)
            return; //上一次发送成功说明无待重发，任务列表空说明无待发，直接退出

        HAL_StatusTypeDef status = HAL_OK;
        if (handleStateE == Handle_State_e::READY) { //上一次发送成功且任务列表非空，进行资源入队
            UART_Task_t tmpTask = taskQueue.front();
            resourceList.push_back(std::move(tmpTask));
        }

        switch (resourceList.front().task) {
            case READ: 
                HAL_UARTEx_ReceiveToIdle_IT(&User_UART, resourceList.front().bufPtr, resourceList.front().size);
                break;
            case WRITE:
                HAL_UART_Transmit_IT(&User_UART, resourceList.front().bufPtr, resourceList.front().size);
                break;
            case READ_VECTOR:
                HAL_UART_Receive_IT(&User_UART, resourceList.front().data.data(), resourceList.front().data.size());
                break;
            case WRITE_VECTOR:
                HAL_UART_Transmit_IT(&User_UART, resourceList.front().data.data(),resourceList.front().data.size());
                break;
            case DELAY:
                if (handleStateE == Handle_State_e::READY) {
                    status = HAL_BUSY;
                    resourceList.front().size = resourceList.front().size + HAL_GetTick();
                } else {
                    if(HAL_GetTick() > resourceList.front().size) {
                        status = HAL_OK;
                        resourceList.pop_front();
                    } else {
                        status = HAL_BUSY;
                    }
                }
                break;
        }
        if (status != HAL_OK) {
            handleStateE = Handle_State_e::RETRYING;
        } else {
            taskQueue.pop();
            handleStateE = Handle_State_e::READY;
        }
    }

    // 回调函数处理
    enum class Callback_e {
        READ,
        WRITE,
        ERROR_CALL,
    };
    void CallbackHandle(Callback_e callbackE) {
        switch (callbackE) {
            case Callback_e::READ:
                if (resourceList.front().callbackFuncPtr != nullptr) {
                    resourceList.front().callbackFuncPtr(resourceList.front());
                }
                break;
            case Callback_e::WRITE:
                break;
            case Callback_e::ERROR_CALL:
                break;
        }
        resourceList.pop_front();
    }

    std::queue<UART_Task_t> taskQueue; // 任务调度队列
    std::list<UART_Task_t> resourceList; // 任务执行内存
    uint8_t taskID = 0;

private:
    UART_Bus() {
        HALInit::GetInstance();
    }
};

template <uint8_t busID>
class UART_Agent {
public:
    explicit UART_Agent(UART_Bus<busID>& UartBusInstance) : uartBusRef(UartBusInstance) {}

    // 不定长接收
    void Read(uint8_t* _bufPtr, uint8_t _size, CallbackFuncPtr callPtr = nullptr) {
        if (uartBusRef.taskQueue.size() >= AGNET_TASK_MAX_NUM) return;
        UART_Task_t tmpTask;
        tmpTask.taskID = ++uartBusRef.taskID;
        tmpTask.task = READ;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = _size;
        tmpTask.callbackFuncPtr = callPtr;
        uartBusRef.taskQueue.push(std::move(tmpTask));
    }

    // 定长接收(ByteVector)
    void Read(ByteVector dataVector, CallbackFuncPtr callPtr = nullptr) {
        if (uartBusRef.taskQueue.size() >= AGNET_TASK_MAX_NUM) return;
        UART_Task_t tmpTask;
        tmpTask.taskID = ++uartBusRef.taskID;
        tmpTask.task = READ_VECTOR;
        tmpTask.data = std::move(dataVector);
        tmpTask.callbackFuncPtr = callPtr;
        uartBusRef.taskQueue.push(std::move(tmpTask));
    }

    // 串口发送
    void Write(uint8_t* _bufPtr, uint8_t _size, CallbackFuncPtr callPtr = nullptr) {
        if (uartBusRef.taskQueue.size() >= AGNET_TASK_MAX_NUM) return;
        UART_Task_t tmpTask;
        tmpTask.taskID = ++uartBusRef.taskID;
        tmpTask.task = WRITE;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = _size;
        tmpTask.callbackFuncPtr = callPtr;
        uartBusRef.taskQueue.push(std::move(tmpTask));
    }

    // 小批量发送(ByteVector)
    void Write(ByteVector dataVector, CallbackFuncPtr callPtr = nullptr) {
        if (uartBusRef.taskQueue.size() >= AGNET_TASK_MAX_NUM) return;
        UART_Task_t tmpTask;
        tmpTask.taskID = ++uartBusRef.taskID;
        tmpTask.task = WRITE_VECTOR;
        tmpTask.data = std::move(dataVector);
        tmpTask.callbackFuncPtr = callPtr;
        uartBusRef.taskQueue.push(std::move(tmpTask));
    }
    
private:
    UART_Bus<busID>& uartBusRef;
};

template class UART_Bus<6>; // 显式实例化能解决部分奇怪的链接时报错

#endif //UART_BASE_MODULE

#endif //FINEMOTE_UART_BASE_H
