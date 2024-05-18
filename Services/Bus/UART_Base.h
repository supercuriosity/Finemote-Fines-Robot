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
#include <utility>
#include <vector>
#include <list>
#include <map>

using ByteVector = std::vector<uint8_t>;

typedef enum : uint32_t {
    READ = 0,
    WRITE,
    //READ_VECTOR,
    //WRITE_VECTOR,
    READ_FIXED_LENGTH,
    //DELAY,
    ERROR_CALLBACK
} UART_Task_e;

typedef struct UART_Task_t{
    //uint8_t taskID;
    UART_Task_e task;
    uint8_t * bufPtr;
    uint32_t size;
    //ByteVector data;
    std::function<void(UART_Task_t)> callbackFuncPtr;
} UART_Task_t;

using CallbackFuncPtr = std::function<void(UART_Task_t)>;

/**
 * 无模板参数的基类，用于提供同一调用接口
 */
class UART_Base{
public:
    // 回调函数处理
    enum class Callback_e {
        READ,
        WRITE,
        ERROR_CALL,
    };
    void CallbackHandle(Callback_e callbackE) {
        switch (callbackE) {
            case Callback_e::READ:
                if (resourceList.front().callbackFuncPtr) {
                    resourceList.front().callbackFuncPtr(resourceList.front());
                }
                break;
            case Callback_e::WRITE:
                if (resourceList.front().callbackFuncPtr) {
                    resourceList.front().callbackFuncPtr(resourceList.front());
                }
                break;
            case Callback_e::ERROR_CALL:
                resourceList.front().task = ERROR_CALLBACK;
                if (resourceList.front().callbackFuncPtr) {
                    resourceList.front().callbackFuncPtr(resourceList.front());
                }
                break;
        }
        if(!resourceList.empty() ) resourceList.pop_front();
    }

    std::queue<UART_Task_t> taskQueue; // 任务调度队列
    std::list<UART_Task_t> resourceList; // 任务执行内存
};

extern std::map<UART_HandleTypeDef*,UART_Base*>& GetUartHandle_BusMap();

template <uint8_t busID>
class UART_Bus: public DeviceBase,public UART_Base{
    uint32_t taskSize;
    uint32_t resourceSize;
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
        taskSize = taskQueue.size();
        resourceSize = resourceList.size();

        // 处理，涉及三个状态量：两个容器是否为空，以及上次发送是否成功
        // 对应到三种处理：直接重发，资源入队后发送，直接退出
        if (handleStateE == Handle_State_e::READY && taskSize == 0)
            return; //上一次发送成功说明无待重发，任务列表空说明无待发，直接退出

        HAL_StatusTypeDef status = HAL_OK;
        if (handleStateE == Handle_State_e::READY) { //上一次发送成功且任务列表非空，进行资源入队
            //UART_Task_t tmpTask = ;
            resourceList.emplace_back(taskQueue.front());
        }

        switch (resourceList.front().task) {
            case READ:
                UARTEx_ReceiveToIdle_IT(uartHandleList[busID], resourceList.front().bufPtr, resourceList.front().size);
                break;
            case WRITE:
                UART_Transmit_IT(uartHandleList[busID], resourceList.front().bufPtr, resourceList.front().size);
                break;
/*            case READ_VECTOR:
                UART_Receive_IT(uartHandleList[busID], resourceList.front().data.data(), resourceList.front().data.size());
                break;
            case WRITE_VECTOR:
                UART_Transmit_IT(uartHandleList[busID], resourceList.front().data.data(),resourceList.front().data.size());
                break;*/
            case READ_FIXED_LENGTH:
                UART_Receive_IT(uartHandleList[busID], resourceList.front().bufPtr, resourceList.front().size);
                break;
/*            case DELAY:
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
                break;*/
			case ERROR_CALLBACK:
				break;
        }
        if (status != HAL_OK) {
            handleStateE = Handle_State_e::RETRYING;
        } else {
            taskQueue.pop();
            handleStateE = Handle_State_e::READY;
        }

    }

    uint8_t taskID = 0;

private:
    UART_Bus() {
        static_assert(busID<(sizeof (uartHandleList)/sizeof (uartHandleList[0])),"Using Illegal UART BUS");
        HALInit::GetInstance();
        GetUartHandle_BusMap()[uartHandleList[busID]]=this;
    }
    void UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
        HAL_UART_Receive_IT(huart, pData, Size);
    }
    void UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
        HAL_UART_Transmit_IT(huart, pData, Size);
    }
    void UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
        HAL_UARTEx_ReceiveToIdle_IT(huart, pData, Size);
    }

};



template <uint8_t busID>
class UART_Agent {
public:
    explicit UART_Agent() : uartBusRef(UART_Bus<busID>::GetInstance()) {}

    // 不定长接收
    void Read(uint8_t* _bufPtr, CallbackFuncPtr callPtr = nullptr) {
        if ((uartBusRef.taskQueue.size() + uartBusRef.resourceList.size()) >= UART_AGNET_TASK_MAX_NUM) return;
        UART_Task_t tmpTask;
        //tmpTask.taskID = ++uartBusRef.taskID;
        tmpTask.task = READ;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = UNFIXED_READ_MAX_LENGTH;
        tmpTask.callbackFuncPtr = std::move(callPtr);
        uartBusRef.taskQueue.push(std::move(tmpTask));
    }

    // 定长接收
    void Read(uint8_t* _bufPtr, uint8_t _size, CallbackFuncPtr callPtr = nullptr) {
        if ((uartBusRef.taskQueue.size() + uartBusRef.resourceList.size()) >= UART_AGNET_TASK_MAX_NUM) return;
        UART_Task_t tmpTask;
        //tmpTask.taskID = ++uartBusRef.taskID;
        tmpTask.task = READ_FIXED_LENGTH;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = _size;
        tmpTask.callbackFuncPtr = std::move(callPtr);
        uartBusRef.taskQueue.push(std::move(tmpTask));
    }

//    // 定长接收(ByteVector)
//    void Read(ByteVector dataVector, CallbackFuncPtr callPtr = nullptr) {
//        if (uartBusRef.taskQueue.size() >= UART_AGNET_TASK_MAX_NUM) return;
//        UART_Task_t tmpTask;
//        tmpTask.taskID = ++uartBusRef.taskID;
//        tmpTask.task = READ_VECTOR;
//        tmpTask.data = std::move(dataVector);
//        tmpTask.callbackFuncPtr = std::move(callPtr);
//        uartBusRef.taskQueue.push(std::move(tmpTask));
//    }


    // 串口发送
    void Write(uint8_t* _bufPtr, uint8_t _size, CallbackFuncPtr callPtr = nullptr) {
        if ((uartBusRef.taskQueue.size() + uartBusRef.resourceList.size()) >= UART_AGNET_TASK_MAX_NUM) return;
        UART_Task_t tmpTask;
        //tmpTask.taskID = ++uartBusRef.taskID;
        tmpTask.task = WRITE;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = _size;
        tmpTask.callbackFuncPtr = std::move(callPtr);
        uartBusRef.taskQueue.push(std::move(tmpTask));
    }

/*    // 小批量发送(ByteVector)
    void Write(ByteVector dataVector, CallbackFuncPtr callPtr = nullptr) {
        if (uartBusRef.taskQueue.size() >= UART_AGNET_TASK_MAX_NUM) return;
        UART_Task_t tmpTask;
        tmpTask.taskID = ++uartBusRef.taskID;
        tmpTask.task = WRITE_VECTOR;
        tmpTask.data = std::move(dataVector);
        tmpTask.callbackFuncPtr = std::move(callPtr);
        uartBusRef.taskQueue.push(std::move(tmpTask));
    }*/

//    void Delay(uint32_t _ms) {
//        if (uartBusRef.taskQueue.size() >= UART_AGNET_TASK_MAX_NUM) return;
//        UART_Task_t tmpTask;
//        //tmpTask.taskID = ++uartBusRef.taskID;
//        tmpTask.task = DELAY;
//        tmpTask.size = _ms;
//        uartBusRef.taskQueue.push(std::move(tmpTask));
//    }
//
protected:
    UART_Bus<busID>& uartBusRef;
};

#endif //UART_BASE_MODULE

#endif //FINEMOTE_UART_BASE_H
