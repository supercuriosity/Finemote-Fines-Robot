/*******************************************************************************
* Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_RS485_BASE_HPP
#define FINEMOTE_RS485_BASE_HPP

#include "etl/map.h"
#include "DeviceBase.h"
#include "UART_Base.hpp"
#include "BSP_RS485.h"

#define RS485_RX_BUFFER_LENGTH 30
#define RS485_AGENT_SIZE 16
#define RS485_RX_MAX_MS 5

template<size_t ID>
class RS485_Agent;

template<size_t ID>
class RS485_Base : public DeviceBase {
public:
    static RS485_Base &GetInstance() {
        static RS485_Base instance;
        return instance;
    }

    void Handle() final {
        // FSM
        enum class RxState_e {
            IDLE,
            RX,
        };

        static RxState_e state = RxState_e::IDLE;
        static uint8_t cnt = 0;

        switch (state) {
            case RxState_e::IDLE:
                if (waitForResponse) {
                    state = RxState_e::RX;
                    cnt = RS485_RX_MAX_MS - 1;
                }
                break;
            case RxState_e::RX:
                cnt--;
                if (!waitForResponse || cnt == 0) {
                    state = RxState_e::IDLE;
                    waitForResponse = false;
                    RS485FlowControl<ID>::Switch2Tx();
                    UART_Base<BSP_RS485UARTIndexList[ID]>::GetInstance().TxLoader();
                }
                break;
        }
    }

    void RxHandle(uint8_t *data, size_t size) {
        uint8_t id = data[0];
        if (agentMap[id]) {
            agentMap[id]->Decode(data, size);
        }
        waitForResponse = false;
    }

    void Transmit(uint8_t *data, size_t size) {
        UART_Base<BSP_RS485UARTIndexList[ID]>::GetInstance().Transmit(data, size);
    }

private:
    RS485_Base(): buffer([this](uint8_t *data, size_t size) {
        RxHandle(data, size);
    }) {
        UART_Base<BSP_RS485UARTIndexList[ID]>::GetInstance().BindTxHandle([this] {
            return OnTxComplete();
        });
    }

    bool OnTxComplete() {
        RS485FlowControl<ID>::Switch2Rx();
        waitForResponse = true;
        return false;
    }

    bool waitForResponse = false;

    UARTBuffer<ID, RS485_RX_BUFFER_LENGTH> buffer;
    friend class RS485_Agent<ID>;
    etl::map<uint8_t, RS485_Agent<ID> *, RS485_AGENT_SIZE> agentMap;
};

template<size_t ID>
class RS485_Agent {
public:
    RS485_Agent(uint8_t _addr, std::function<void(uint8_t *data, size_t size)> _decodeFunc): addr(_addr),
        decodeFunc(_decodeFunc) {
        static_assert((ID > 0) && (ID <= RS485_BUS_MAXIMUM_COUNT), "Using illegal RS485 BUS");
        RS485_Base<ID>::GetInstance().agentMap[addr] = this;
    }

    void Decode(uint8_t *data, size_t size) {
        decodeFunc(data, size);
    }

    void Transmit(uint8_t *data, size_t size) {
        RS485_Base<ID>::GetInstance().Transmit(data, size);
    }

private:
    uint32_t addr;
    std::function<void(uint8_t *data, size_t size)> decodeFunc;
};

#endif
