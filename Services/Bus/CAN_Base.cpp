/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "CAN_Base.h"
#ifdef CAN_BASE_MODULE

template <int busID>
void CAN_Bus<busID>::RxHandle() {
    uint8_t tempBuf[8];
    CAN_RxHeaderTypeDef Header;
    HAL_CAN_GetRxMessage(CAN_Buses[busID - 1], CAN_RX_FIFO0, &Header, tempBuf);

    if(Header.IDE == CAN_ID_STD) {
        memcpy(Getmap()[Header.StdId], tempBuf, Header.DLC);
    }
    else if(Header.IDE == CAN_ID_EXT) {
        memcpy(Getmap()[Header.ExtId], tempBuf, Header.DLC);
    }
}

template <int busID>
void CAN_Bus<busID>::TxLoader() {
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

        HAL_CAN_AddTxMessage(CAN_Buses[busID - 1], &Header, dataQueue.front().message, &TxMailbox);
        dataQueue.pop();
    }
}

template<int busID>
std::queue<CAN_Package_t> CAN_Bus<busID>::dataQueue = {};

template <int busID>
void FineMoteAux_CAN::Reloader(CAN_HandleTypeDef* hcan) {
    if (hcan == CAN_Buses[busID - 1]) {
        CAN_Bus<busID>::TxLoader();
    } else {
        Reloader<busID - 1>(hcan);
    }
}

template <>
void FineMoteAux_CAN::Reloader<0>(CAN_HandleTypeDef* hcan) { }

template <int busID>
void FineMoteAux_CAN::Distributer(CAN_HandleTypeDef* hcan) {
    if (hcan == CAN_Buses[busID - 1]) {
        CAN_Bus<busID>::RxHandle();
    } else {
        Distributer<busID - 1>(hcan);
    }
}

template <>
void FineMoteAux_CAN::Distributer<0>(CAN_HandleTypeDef* hcan) { }

void FineMoteAux_CAN::Reload(CAN_HandleTypeDef* hcan) {
    Reloader<CAN_BUS_MAXIMUM_COUNT>(hcan);
}

void FineMoteAux_CAN::Distribute(CAN_HandleTypeDef* hcan) {
    Distributer<CAN_BUS_MAXIMUM_COUNT>(hcan);
}

/**
 * C接口
 */
#ifdef __cplusplus
extern "C" {
#endif

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    FineMoteAux_CAN::Distribute(hcan);
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
    FineMoteAux_CAN::Reload(hcan);
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
    FineMoteAux_CAN::Reload(hcan);
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
    FineMoteAux_CAN::Reload(hcan);
}

#ifdef __cplusplus
}
#endif
#endif