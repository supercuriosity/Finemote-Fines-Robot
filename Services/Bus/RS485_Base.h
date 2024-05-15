//
// Created by 25396 on 2024/5/6.
//

#ifndef FINEMOTE_RS485_BASE_H
#define FINEMOTE_RS485_BASE_H

#include "ProjectConfig.h"

#ifdef RS485_BASE_MODULE

#include "DeviceBase.h"
#include "UART_Base.h"
#include <queue>
#include <utility>
#include <vector>
#include <list>
#include <utility>
#include <map>

template <uint8_t busID>
class RS485_Bus: public DeviceBase,public UART_Base{
    uint32_t taskSize;
    uint32_t resourceSize;
public:
    static RS485_Bus& GetInstance() { // 单例模式，提供实例获取方法
        static RS485_Bus<busID> instance;
        return instance;
    }

    // 单例模式，删除默认的拷贝构造函数
    RS485_Bus(const RS485_Bus&) = delete;
    RS485_Bus& operator=(const RS485_Bus&) = delete;

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
                RS485Ex_ReceiveToIdle_IT(rs485HandleList[busID], resourceList.front().bufPtr, resourceList.front().size);
                break;
            case WRITE:
                RS485_Transmit_IT(rs485HandleList[busID], resourceList.front().bufPtr, resourceList.front().size);
                break;
            case READ_FIXED_LENGTH:
                RS485_Receive_IT(rs485HandleList[busID], resourceList.front().bufPtr, resourceList.front().size);
                break;
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
    RS485_Bus() {
        static_assert(busID<(sizeof (rs485HandleList)/sizeof (rs485HandleList[0])),"Using Illegal UART BUS");
        HALInit::GetInstance();
        GetUartHandle_BusMap()[rs485HandleList[busID]]=this;
    }
    void RS485_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
        if(rs485TxPortList[busID] != nullptr && rs485TxPinList[busID] != NULL)
            HAL_GPIO_WritePin(rs485TxPortList[busID], rs485TxPinList[busID], GPIO_PIN_RESET);
        HAL_UART_Receive_IT(huart, pData, Size);
    }
    void RS485_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
        if (rs485TxPortList[busID] != nullptr && rs485TxPinList[busID] != NULL)
            HAL_GPIO_WritePin(rs485TxPortList[busID], rs485TxPinList[busID], GPIO_PIN_SET);
        HAL_UART_Transmit_IT(huart, pData, Size);
    }
    void RS485Ex_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
        if (rs485TxPortList[busID] != nullptr && rs485TxPinList[busID] != NULL)
            HAL_GPIO_WritePin(rs485TxPortList[busID], rs485TxPinList[busID], GPIO_PIN_RESET);
        HAL_UARTEx_ReceiveToIdle_IT(huart, pData, Size);
    }

};


template <uint8_t busID>
class RS485_Agent {
public:
    explicit RS485_Agent() : rs485BusRef(RS485_Bus<busID>::GetInstance()) {
        rs485BusRef.SetDivisionFactor(2);
    }

    // 不定长接收
    void Read(uint8_t* _bufPtr, CallbackFuncPtr callPtr = nullptr) {
        if ((rs485BusRef.taskQueue.size() + rs485BusRef.resourceList.size()) >= RS485_AGNET_TASK_MAX_NUM) return;
        UART_Task_t tmpTask;
        //tmpTask.taskID = ++uartBusRef.taskID;
        tmpTask.task = READ;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = RS485_UNFIXED_READ_MAX_LENGTH;
        tmpTask.callbackFuncPtr = std::move(callPtr);
        rs485BusRef.taskQueue.push(std::move(tmpTask));
    }

    // 定长接收
    void Read(uint8_t* _bufPtr, uint8_t _size, CallbackFuncPtr callPtr = nullptr) {
        if ((rs485BusRef.taskQueue.size() + rs485BusRef.resourceList.size()) >= RS485_AGNET_TASK_MAX_NUM) return;
        UART_Task_t tmpTask;
        //tmpTask.taskID = ++uartBusRef.taskID;
        tmpTask.task = READ_FIXED_LENGTH;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = _size;
        tmpTask.callbackFuncPtr = std::move(callPtr);
        rs485BusRef.taskQueue.push(std::move(tmpTask));
    }

    // 串口发送
    void Write(uint8_t* _bufPtr, uint8_t _size, CallbackFuncPtr callPtr = nullptr) {
        if ((rs485BusRef.taskQueue.size() + rs485BusRef.resourceList.size()) >= RS485_AGNET_TASK_MAX_NUM) return;
        UART_Task_t tmpTask;
        //tmpTask.taskID = ++uartBusRef.taskID;
        tmpTask.task = WRITE;
        tmpTask.bufPtr = _bufPtr;
        tmpTask.size = _size;
        tmpTask.callbackFuncPtr = std::move(callPtr);
        rs485BusRef.taskQueue.push(std::move(tmpTask));
    }

private:
    RS485_Bus<busID>& rs485BusRef;
};


#endif //RS485_BASE_MODULE

#endif //FINEMOTE_RS485_BASE_H
