/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "I2C_Base.h"

#ifdef I2C_BASE_MODULE

#ifdef __cplusplus
extern "C" {
#endif

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    //TODO 当存在多个I2C总线时，应该通过遍历模板类示例的方法执行，目前需要手动添加
    I2C_Bus<2>::GetInstance().CallbackHandle( I2C_Bus<2>::Callback_e::MASTER_TX);
}
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    I2C_Bus<2>::GetInstance().CallbackHandle( I2C_Bus<2>::Callback_e::MASTER_RX);
}
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c){
    I2C_Bus<2>::GetInstance().CallbackHandle( I2C_Bus<2>::Callback_e::MEM_READ);
}
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c){//TODO 想办法存储任务发送失败这个信息
    I2C_Bus<2>::GetInstance().CallbackHandle( I2C_Bus<2>::Callback_e::ERROR_CALL);
}


#ifdef __cplusplus
}
#endif

#endif //I2C_Base_Module