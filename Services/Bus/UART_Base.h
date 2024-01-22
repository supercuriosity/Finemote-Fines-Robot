/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/


#ifndef FINEMOTE_UART_BASE_H
#define FINEMOTE_UART_BASE_H

#include "DeviceBase.h"
#include "PeripheralDefination.h"

template <int busID>
class UART_Bus: public DeviceBase{
public:
    static UART_Bus& GetInstance() { // 单例模式，提供创建实例方法
        static UART_Bus<busID> instance;
        return instance;
    }

    // 单例模式，删除默认的拷贝构造函数
    UART_Bus(const UART_Bus&) = delete;
    UART_Bus& operator=(const UART_Bus&) = delete;

    // TODO:串口任务处理
    void Handle() override{

    }

    // TODO:回调函数处理
    void CallbackHandle() {

    }

private:
    UART_Bus() {}
};

template <int busID>
class UART_Agent {
public:
    explicit UART_Agent(UART_Bus<busID>& UartBusInstance) : UartBusRef(UartBusInstance) {}

    // TODO:提供给用户的各种接口
    // 定长接收(ByteVector)
    // 不定长接收
    // 串口发送
    // 小批量发送(ByteVector)
private:
    UART_Bus<busID>& UartBusRef;
};

#endif //FINEMOTE_UART_BASE_H
