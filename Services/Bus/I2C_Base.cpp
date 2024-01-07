/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "I2C_Base.h"

template <uint8_t busID>
bool I2C_Bus<busID>::isFree = true;

template <uint8_t busID>
std::queue<I2C_Task_t> I2C_Bus<busID>::taskQueue = {};

#ifdef __cplusplus
extern "C" {
#endif

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    I2C_Bus<2>::RTHandle();
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    I2C_Bus<2>::RTHandle();
}

#ifdef __cplusplus
}
#endif
