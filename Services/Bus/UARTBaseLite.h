/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_UARTBASELITE_H
#define FINEMOTE_UARTBASELITE_H

#include "ProjectConfig.h"

#ifdef UART_BASE_MODULE

#include "DeviceBase.h"
#include <queue>

template <uint8_t ID>
class UARTBaseLite : public DeviceBase {
public:
    static UARTBaseLite& GetInstance() {
        static UARTBaseLite instance;
        return instance;
    }

    void Bind(std::function<void(uint8_t*, uint16_t)>& _decodeFunc) {
        decodeFunc = _decodeFunc;
    }

    void Transmit(uint8_t* data, uint16_t size) {
        txQueue.push(std::make_pair(data, size));
    }

    void TxLoader() {
        if (!txQueue.empty()) {
            HAL_UART_Transmit_IT(uartHandleList[ID], txQueue.front().first, txQueue.front().second);
            txQueue.pop();
        }
    }

    void Handle() final {
        TxLoader();
    }

    void RxHandle(uint16_t size) {
        if(decodeFunc) {
            decodeFunc(rxBuffer[0], size);
        }
        HAL_UARTEx_ReceiveToIdle_IT(uartHandleList[ID], rxBuffer[0], 50);
    }

private:
    UARTBaseLite() {
        HALInit::GetInstance();
        HAL_UARTEx_ReceiveToIdle_IT(uartHandleList[ID], rxBuffer[0], 50);
    }

    std::queue<std::pair<uint8_t*, uint16_t>> txQueue;
    uint8_t rxBuffer[1][200] = {0};
    std::function<void(uint8_t*, uint16_t)> decodeFunc = nullptr;
};

#endif UART_BASE_MODULE
#endif FINEMOTE_UARTBASELITE_H
