/*******************************************************************************
* Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_UART_BASE_HPP
#define FINEMOTE_UART_BASE_HPP

#include <functional>

#include "etl/queue.h"
#include "DoubleBuffer.hpp"
#include "BSP_UART.h"

#define UART_TX_QUEUE_SIZE 20

template<uint8_t ID>
class UART_Base;

template<size_t ID, size_t N>
class UARTBuffer : public DoubleBuffer<N> {
public:
    explicit UARTBuffer(std::function<void(uint8_t *, size_t)> decodeCallback) : DoubleBuffer<N>(decodeCallback) {
        UART_Base<ID>::GetInstance().Bind(*this);
        this->SwitchBuffer();
    }
};

template<uint8_t ID>
class UART_Base {
public:
    static UART_Base &GetInstance() {
        static UART_Base instance;
        return instance;
    }

    /*** Part 1: Setups ***/

    template<size_t N>
    void Bind(UARTBuffer<ID, N> &buffer) {
        bufferHead = &buffer.GetBuffer();
        rxLength = N;
        commitBufferFunc = [&buffer](size_t size) {
            buffer.CommitBuffer(size);
        };

        BSP_UART<ID>::GetInstance().Receive(*bufferHead, rxLength);
    }

    void BindTxHandle(std::function<bool()> txFunc) {
        OnTxCompleteFunc = txFunc;
    }

    /*** Part 2: Transmit ***/

    void Transmit(uint8_t *data, uint16_t size) {
        if (txQueue.full()) {
            txQueue.pop(); // Todo
        }
        txQueue.push(std::make_pair(data, size)); // Push or Emplace?

        if (isTxComplete) {
            TxLoader();
        }
    }

    void TxLoader() {
        if (!txQueue.empty()) {
            BSP_UART<ID>::GetInstance().Transmit(txQueue.front().first, txQueue.front().second);
            txQueue.pop();
            isTxComplete = false;
        }
    }

    void TxHandle() {
        isTxComplete = true;
        if (OnTxCompleteFunc()) {
            TxLoader();
        }
    }

    /*** Part 3: Receive ***/

    void RxHandle(uint16_t size) {
        BSP_UART<ID>::GetInstance().Receive(*bufferHead, rxLength);
        commitBufferFunc(size);
    }

private:
    UART_Base() {
        static_assert((ID > 0) && (ID <= UART_BUS_MAXIMUM_COUNT), "Using illegal UART BUS");
        BSP_UART<ID>::GetInstance();
    }

    bool isTxComplete = true;

    uint8_t **bufferHead = nullptr;
    size_t rxLength;
    std::function<void(size_t)> commitBufferFunc;

    etl::queue<std::pair<uint8_t *, uint16_t>, UART_TX_QUEUE_SIZE> txQueue;
    std::function<bool()> OnTxCompleteFunc = [] {
        bool autoReload = true;
        return autoReload;
    };
};

template<typename T = decltype(BSP_UARTList[0])>
class FineMoteAux_UART {
public:
    static void OnTxComplete(T &instance) {
        constexpr size_t maxID = sizeof(BSP_UARTList) / sizeof(BSP_UARTList[0]) - 1;
        TxCompleteImpl<maxID>(instance);
    }

    static void OnRxComplete(T &instance, size_t size) {
        constexpr size_t maxID = sizeof(BSP_UARTList) / sizeof(BSP_UARTList[0]) - 1;
        RxCompleteImpl<maxID>(instance, size);
    }

    template<size_t ID>
    static void TxCompleteImpl(T &instance) {
        if constexpr (BSP_UARTList[ID] != nullptr) {
            if (instance == BSP_UARTList[ID]) {
                UART_Base<ID>::GetInstance().TxHandle();
                return;
            }
        }
        if constexpr (ID > 1) {
            TxCompleteImpl<ID - 1>(instance);
        }
    }

    template<size_t ID>
    static void RxCompleteImpl(T &instance, size_t size) {
        if constexpr (BSP_UARTList[ID] != nullptr) {
            if (instance == BSP_UARTList[ID]) {
                UART_Base<ID>::GetInstance().RxHandle(size);
                return;
            }
        }
        if constexpr (ID > 1) {
            RxCompleteImpl<ID - 1>(instance, size);
        }
    }
};

#endif
