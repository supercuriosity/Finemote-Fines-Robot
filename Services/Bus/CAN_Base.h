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


constexpr CAN_HandleTypeDef* qwq[] = {&hcan1, &hcan2};

typedef struct {
    uint32_t addr;
    uint8_t DLC;
    uint8_t message[8];
} CAN_Package_t;

template <int busID>
class CAN_Bus {
public:
    static CAN_Bus &GetInstance() {
        static CAN_Bus<busID> instance;
        return instance;
    }
    static void Init() {
        CAN_Bus<busID>();
    }

    CAN_Bus(const CAN_Bus &) = delete;
    CAN_Bus& operator=(const CAN_Bus&) = delete;

    static void RxHandle() {
        static_assert((busID > 0) && (busID <= CAN_BUS_MAXIMUM_COUNT));

        uint8_t tempBuf[8];
        CAN_RxHeaderTypeDef Header;
        HAL_CAN_GetRxMessage(qwq[busID - 1], CAN_RX_FIFO0, &Header, tempBuf);
        memcpy(map[Header.StdId], tempBuf, sizeof(tempBuf));//未被执行

    }

    static void TxLoader() {
        static_assert((busID > 0) && (busID <= CAN_BUS_MAXIMUM_COUNT));
     
        if (!dataQueue.empty()) {
            CAN_TxHeaderTypeDef Header;
            uint32_t TxMailbox;
            Header.StdId = dataQueue.front().addr;
            Header.DLC = dataQueue.front().DLC;
            Header.IDE = CAN_ID_STD;
            Header.RTR = CAN_RTR_DATA;
            Header.TransmitGlobalTime = DISABLE;

            HAL_CAN_AddTxMessage(qwq[busID - 1], &Header, dataQueue.front().message, &TxMailbox);

            dataQueue.pop();
        }
    }


    static std::map<uint32_t, uint8_t *> map;
    static std::queue<CAN_Package_t> dataQueue;
    
private:
    CAN_Bus() {
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

template<int busID>
std::map<uint32_t, uint8_t *> CAN_Bus<busID>::map = {};
template<int busID>
std::queue<CAN_Package_t> CAN_Bus<busID>::dataQueue = {};

template <int busID>
class CAN_Agent {
public:
    CAN_Agent(uint32_t addr, CAN_Bus<busID> &CANBusInstance) : addr(addr), CANBusRef(CANBusInstance) {
        static_assert((busID > 0) && (busID <= CAN_BUS_MAXIMUM_COUNT));
        CANBusRef.GetInstance().map[addr] = rxbuf;
    }

    void Send() {
        CAN_Package_t temp;
        temp.addr = addr;
        temp.DLC = DLC;
        memcpy(temp.message, txbuf, DLC);
        CANBusRef.GetInstance().dataQueue.push(temp);
    }

    uint8_t rxbuf[8] = {0};
    uint8_t txbuf[8] = {0};
    uint8_t DLC{};
    uint32_t addr;

private:
    CAN_Bus<busID> &CANBusRef;

};
#endif
#endif //FINEMOTE_CAN_BASE_H
