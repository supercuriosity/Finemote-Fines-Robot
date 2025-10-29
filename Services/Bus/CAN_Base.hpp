/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_CAN_BASE_HPP
#define FINEMOTE_CAN_BASE_HPP

#include "etl/map.h"
#include "etl/queue.h"
#include "BSP_CAN.h"

#define CAN_MAP_SIZE 20
#define CAN_TX_QUEUE_SIZE 16

/**
 * Todo:
 * 远程帧处理
 */

typedef struct {
    uint8_t DLC;
    uint8_t IDE;
    uint8_t RTR;
    uint32_t addr;
    uint8_t message[8];
} CAN_Package_t;

template<size_t ID>
class CAN_Base {
public:
    static CAN_Base &GetInstance() {
        static CAN_Base instance;
        return instance;
    }

    CAN_Base(const CAN_Base &) = delete;

    CAN_Base &operator=(const CAN_Base &) = delete;

    void RxHandle() {
        uint8_t tempBuf[8];
        CAN_RxHeaderTypeDef Header;

        BSP_CAN<ID>::GetInstance().Receive(&Header, tempBuf);

        if (Header.IDE == CAN_ID_STD) {
            memcpy(rxBufferMap[Header.StdId], tempBuf, Header.DLC);
        } else if (Header.IDE == CAN_ID_EXT) {
            memcpy(rxBufferMap[Header.ExtId], tempBuf, Header.DLC);
        }
    }

    void TxHandle() {
        if (!dataQueue.empty()) {
            CAN_TxHeaderTypeDef Header;

            if (dataQueue.front().IDE == CAN_ID_STD) {
                Header.StdId = dataQueue.front().addr;
            } else if (dataQueue.front().IDE == CAN_ID_EXT) {
                Header.ExtId = dataQueue.front().addr;
            }

            Header.DLC = dataQueue.front().DLC;
            Header.IDE = dataQueue.front().IDE;
            Header.RTR = dataQueue.front().RTR;
            Header.TransmitGlobalTime = DISABLE;

            BSP_CAN<ID>::GetInstance().Transmit(&Header, dataQueue.front().message);

            dataQueue.pop();
        } else {
            isTxComplete = true;
        }
    }

    bool Transmit(CAN_Package_t &txbuf) {
        if (dataQueue.full()) {
            dataQueue.pop();
        }
        dataQueue.push(txbuf);
        if (isTxComplete == true) {
            TxHandle();
            isTxComplete = false;
        }
        return true;
    }

    void BindRxBuffer(uint8_t *buffer, uint32_t addr) {
        rxBufferMap[addr] = buffer;
    }

private:
    etl::map<uint32_t, uint8_t *, CAN_MAP_SIZE> rxBufferMap;
    etl::queue<CAN_Package_t,CAN_TX_QUEUE_SIZE> dataQueue;
    bool isTxComplete = true;

    CAN_Base() {
        BSP_CAN<ID>::GetInstance();
    }
};

template<size_t ID>
class CAN_Agent {
public:
    explicit CAN_Agent(uint32_t addr) : addr(addr) {
        static_assert(ID > 0 && ID <= CAN_BUS_MAXIMUM_COUNT && BSP_CANList[ID] != nullptr, "Using illegal CAN BUS");
        CAN_Base<ID>::GetInstance().BindRxBuffer(rxbuf, addr);
    }

    void SetDLC(uint8_t _DLC) {
        txbuf.DLC = _DLC;
    }

    /**
     * @brief CAN发送队列装填
     * @param _addr
     * @param config IDE | RTR
     * @param IDE CAN_ID_STD or CAN_ID_EXT
     * @param RTR CAN_RTR_DATA or CAN_RTR_REMOTE
     */
    void Transmit(uint32_t _addr, uint8_t config = CAN_ID_STD | CAN_RTR_DATA) {
        txbuf.addr = _addr;
        txbuf.IDE = config & CAN_ID_EXT;
        txbuf.RTR = config & CAN_RTR_REMOTE;

        CAN_Base<ID>::GetInstance().Transmit(txbuf);
    }

    uint8_t &operator[](std::size_t index) {
        return txbuf.message[index];
    }

    uint8_t operator[](std::size_t index) const {
        return rxbuf[index];
    }

    uint32_t addr;
    uint8_t rxbuf[8] = {0};

private:
    CAN_Package_t txbuf = {8};
};

template<typename T = decltype(BSP_CANList[0])>
class FineMoteAux_CAN {
public:
    static void OnTxComplete(T hcan) {
        constexpr size_t maxID = sizeof(BSP_CANList) / sizeof(BSP_CANList[0]) - 1;
        TxCompleteImpl<maxID>(hcan);
    }

    static void OnRxComplete(T hcan) {
        constexpr size_t maxID = sizeof(BSP_CANList) / sizeof(BSP_CANList[0]) - 1;
        RxCompleteImpl<maxID>(hcan);
    }

private:
    template<size_t ID>
    static void TxCompleteImpl(T hcan) {
        if constexpr (BSP_CANList[ID] != nullptr) {
            if (hcan == BSP_CANList[ID]) {
                CAN_Base<ID>::GetInstance().TxHandle();
                return;
            }
        }
        if constexpr (ID > 1) {
            TxCompleteImpl<ID - 1>(hcan);
        }
    }

    template<size_t ID>
    static void RxCompleteImpl(T hcan) {
        if constexpr (BSP_CANList[ID] != nullptr) {
            if (hcan == BSP_CANList[ID]) {
                CAN_Base<ID>::GetInstance().RxHandle();
                return;
            }
        }
        if constexpr (ID > 1) {
            RxCompleteImpl<ID - 1>(hcan);
        }
    }
};

#endif
