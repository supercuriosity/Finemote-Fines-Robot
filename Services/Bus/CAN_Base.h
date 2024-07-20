/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_CAN_BASE_H
#define FINEMOTE_CAN_BASE_H

#include "ProjectConfig.h"

#ifdef CAN_BASE_MODULE

#include <map>
#include <queue>

/**
 * Todo:
 * 远程帧处理
 */

constexpr CAN_HandleTypeDef* qwq[] = {&hcan1, &hcan2};

typedef struct {
    uint8_t DLC;
    uint8_t IDE;
    uint8_t RTR;
    uint32_t addr;
    uint8_t message[8];
} CAN_Package_t;

template <int busID>
class CAN_Bus {
public:

    static CAN_Bus &GetInstance() {
        static CAN_Bus<busID> instance;
        return instance;
    }

    CAN_Bus(const CAN_Bus &) = delete;
    CAN_Bus& operator=(const CAN_Bus&) = delete;

    static void RxHandle() {
        uint8_t tempBuf[8];
        CAN_RxHeaderTypeDef Header;
        HAL_CAN_GetRxMessage(qwq[busID - 1], CAN_RX_FIFO0, &Header, tempBuf);

        if(Header.IDE == CAN_ID_STD) {
            memcpy(Getmap()[Header.StdId], tempBuf, Header.DLC);
        }
        else if(Header.IDE == CAN_ID_EXT) {
            memcpy(Getmap()[Header.ExtId], tempBuf, Header.DLC);
        }
    }

    static void TxLoader() {
        if (!dataQueue.empty()) {
            CAN_TxHeaderTypeDef Header;
            uint32_t TxMailbox = 0;

            if(dataQueue.front().IDE == CAN_ID_STD) {
                Header.StdId = dataQueue.front().addr;
            } else if(dataQueue.front().IDE == CAN_ID_EXT) {
                Header.ExtId = dataQueue.front().addr;
            }

            Header.DLC = dataQueue.front().DLC;
            Header.IDE = dataQueue.front().IDE;
            Header.RTR = dataQueue.front().RTR;
            Header.TransmitGlobalTime = DISABLE;

            HAL_CAN_AddTxMessage(qwq[busID - 1], &Header, dataQueue.front().message, &TxMailbox);
            dataQueue.pop();
        }
    }

    static std::map<uint32_t, uint8_t *> &Getmap(){
        static std::map<uint32_t, uint8_t *> map;
        return map;
    }
    static std::queue<CAN_Package_t> dataQueue;
    
private:
    CAN_Bus() {
        static_assert((busID > 0) && (busID <= CAN_BUS_MAXIMUM_COUNT), "Using Illegal CAN BUS");
        HALInit::GetInstance();

        HAL_CAN_ActivateNotification(qwq[busID - 1], CAN_IT_RX_FIFO0_MSG_PENDING);
        HAL_CAN_ActivateNotification(qwq[busID - 1], CAN_IT_TX_MAILBOX_EMPTY);

        CAN_FilterTypeDef canFilter;
        canFilter.FilterMode = CAN_FILTERMODE_IDMASK;
        canFilter.FilterScale = CAN_FILTERSCALE_32BIT;
        canFilter.FilterIdHigh = 0x0000;
        canFilter.FilterIdLow = 0x0000;
        canFilter.FilterMaskIdHigh = 0x0000;
        canFilter.FilterMaskIdLow = 0x0000;
        canFilter.FilterFIFOAssignment = CAN_RX_FIFO0;
        canFilter.FilterActivation = ENABLE;
        canFilter.FilterBank = 0;
        canFilter.SlaveStartFilterBank = 14;
        HAL_CAN_ConfigFilter(qwq[busID - 1], &canFilter);

        HAL_CAN_Start(qwq[busID - 1]);
    }
};

template <int busID>
class CAN_Agent {
public:
    explicit CAN_Agent(uint32_t addr) : addr(addr) {
        static_assert((busID > 0) && (busID <= CAN_BUS_MAXIMUM_COUNT), "Using illegal CAN BUS");
        CAN_Bus<busID>::GetInstance().Getmap()[addr] = rxbuf;
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
    void Send(uint32_t _addr, uint8_t config = CAN_ID_STD | CAN_RTR_DATA) {
        txbuf.addr = _addr;
        txbuf.IDE = config & CAN_ID_EXT;
        txbuf.RTR = config & CAN_RTR_REMOTE;

        if (CAN_Bus<busID>::GetInstance().dataQueue.size() < CAN_AGNET_TASK_MAX_NUM) {
            CAN_Bus<busID>::GetInstance().dataQueue.push(txbuf);
        }
    }

    uint8_t& operator[](std::size_t index) {
            return txbuf.message[index];
    }

    uint32_t addr;
    uint8_t rxbuf[8] = {0};

private:
    CAN_Package_t txbuf = {8};
};

#endif //CAN_BASE_MODULE
#endif //FINEMOTE_CAN_BASE_H
