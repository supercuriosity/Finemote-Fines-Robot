/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_UART_TEST_H
#define FINEMOTE_UART_TEST_H

#include "ProjectConfig.h"

#ifdef UART_TEST_COMPONENTS

#include "Bus/UART_Base.h"

/**
 * UARTBase类的示例用法
 */
class UART_Test {
    UART_Agent<0> hostUart;
    uint8_t receivedMessage[40]{};
    ByteVector receivedByteVector;

public:
    void receivedCallback(UART_Task_t _data) {
        //hostUart.Write({'A', 'c', 'k', '\n'});
        uint16_t tmp;
        tmp = receivedMessage[0] + receivedMessage[1];
    }

    UART_Test() {
        // 小批量数据定长发送
        hostUart.Write({'H', 'i', '!', '\n'});

        // 小批量数据接收，读取后调用回调函数（注意数据的生命周期）
        receivedByteVector.resize(3);
        auto receivedCallbackPack = [this](UART_Task_t a){this->receivedCallback(a);};
        hostUart.Read(receivedByteVector, receivedCallbackPack);

        // 三个字符串定长发送
        static uint8_t ss1[] = "Hello\n";
        static uint8_t ss2[] = "Bonjour\n";
        static uint8_t ss3[] = "Ko n ni chi wa\n";

        hostUart.Write(ss1, 6);
        hostUart.Write(ss2, 8);
        hostUart.Write(ss3, 15);

        // 字符串定长数据接收，读取后调用回调函数
        hostUart.Read(receivedMessage, 5, receivedCallbackPack);

        // 读取不定长字符串，读取后调用回调函数
        hostUart.Read(receivedMessage, receivedCallbackPack);

        // 总线停顿,用于等待器件处理等场景
        hostUart.Delay(2);
    }
};

#endif //UART_TEST_COMPONENTS

#endif //FINEMOTE_UART_TEST_H
