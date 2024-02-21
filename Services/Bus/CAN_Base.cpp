/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "CAN_Base.h"
#ifdef CAN_BASE_MODULE


#ifdef __cplusplus
extern "C" {
#endif

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if(hcan == &hcan1) {
        CAN_Bus<1>::RxHandle();
    }
    if(hcan == &hcan2) {
        CAN_Bus<2>::RxHandle();
    }
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
    if(hcan == &hcan1) {
        CAN_Bus<1>::TxLoader();
    }
    if(hcan == &hcan2) {
        CAN_Bus<2>::TxLoader();
    }
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
    if(hcan == &hcan1) {
        CAN_Bus<1>::TxLoader();
    }
    if(hcan == &hcan2) {
        CAN_Bus<2>::TxLoader();
    }
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
    if(hcan == &hcan1) {
        CAN_Bus<1>::TxLoader();
    }
    if(hcan == &hcan2) {
        CAN_Bus<2>::TxLoader();
    }
}

#ifdef __cplusplus
}
#endif
#endif