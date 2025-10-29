/*******************************************************************************
* Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "BSP_CAN.h"
#include "Bus/CAN_Base.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    FineMoteAux_CAN<>::OnRxComplete(hcan);
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
    FineMoteAux_CAN<>::OnTxComplete(hcan);
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
    FineMoteAux_CAN<>::OnTxComplete(hcan);
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
    FineMoteAux_CAN<>::OnTxComplete(hcan);
}

#ifdef __cplusplus
}
#endif
