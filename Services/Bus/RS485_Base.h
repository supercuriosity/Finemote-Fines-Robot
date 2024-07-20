//
// Created by 25396 on 2024/5/6.
//

#ifndef FINEMOTE_RS485_BASE_H
#define FINEMOTE_RS485_BASE_H

#include "ProjectConfig.h"

#ifdef RS485_BASE_MODULE

#include "UART_Base.h"

template <uint8_t busID>
class RS485_Agent :public UART_Agent<busID>{
public:
    explicit RS485_Agent(uint32_t _addr) : addr(_addr){
        UART_Agent<busID>::uartBusRef.SetDivisionFactor(2);
    }

    // 不定长接收
    void rs485Read(uint8_t* _bufPtr, CallbackFuncPtr callPtr = nullptr)  {
#ifndef RS485_NO_XXX
        HAL_GPIO_WritePin(rs485TxPortList[busID], rs485TxPinList[busID], GPIO_PIN_RESET);
#endif
        UART_Agent<busID>::Read(_bufPtr, std::move(callPtr));
    }

    // 定长接收
    void rs485Read(uint8_t* _bufPtr, uint8_t _size, CallbackFuncPtr callPtr = nullptr) {
#ifndef RS485_NO_XXX
        HAL_GPIO_WritePin(rs485TxPortList[busID], rs485TxPinList[busID], GPIO_PIN_RESET);
#endif
        UART_Agent<busID>::Read(_bufPtr, _size, std::move(callPtr));
    }

    void rs485Write(uint8_t* _bufPtr, uint8_t _size, CallbackFuncPtr callPtr = nullptr) {
       // callPtr = [this](const UART_Task_t& uartTask) {};
#ifndef RS485_NO_XXX
        HAL_GPIO_WritePin(rs485TxPortList[busID], rs485TxPinList[busID], GPIO_PIN_SET);
#endif
        UART_Agent<busID>::Write(_bufPtr, _size, callPtr);
    }

    uint8_t rxbuf[30]{0};
    uint8_t txbuf[11]{0};
    uint32_t addr;
};

#endif //RS485_BASE_MODULE

#endif //FINEMOTE_RS485_BASE_H
