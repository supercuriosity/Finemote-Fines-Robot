/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "CAN_Base.h"

template <int num>
std::map<uint32_t, uint8_t *> CAN_Agent<num>::map = {};

#ifdef __cplusplus
extern "C" {
#endif

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if(hcan == &hcan1) {
        CAN_Agent<1>::RxHandle();
    }
    if(hcan == &hcan1) {
        CAN_Agent<2>::RxHandle();
    }
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {

}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {

}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {

}

#ifdef __cplusplus
}
#endif
