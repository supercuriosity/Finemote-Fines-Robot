
/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "IMUBase.h"

IMUBase imu;

IMUBase::IMUBase(){
    HALInit::GetInstance();
    if (BMI088_init() != BMI088_NO_ERROR) {
        uint32_t tmp = HAL_GetTick();
        Error_Handler();
    }
#ifdef IMU_USE_MAG
    if(ist8310_init() != IST8310_NO_ERROR) Error_Handler();
#endif
    BMI088_read(rawData.gyro,rawData.accel,&rawData.temp);
    PID_Regulator_t _tempPID = {
            .kp = 340,
            .ki = 0.04,
            .kd = 0.0,
            .componentKpMax = 2000,
            .componentKiMax = 900,
            .outputMax = 999
    };
    tempPid.PIDInfo = _tempPID;
    quat[0] = 1.0f,quat[1] = 0,quat[2] = 0,quat[3] = 0;

    if(HAL_SPI_Init(&hspi1) != HAL_OK) Error_Handler();
    SPI_DMA_init(spiWithDMA, (uint32_t) buf.gyro_dma_tx_buf, (uint32_t) buf.gyro_dma_rx_buf,
                 SPI_DMA_GYRO_LENGHT);

    state.imu_start_dma_flag = 1;

}
void IMUBase::Handle() {


    if(state.gyro_update_flag & (1u << IMU_NOTIFY_SHFITS))
    {
        state.gyro_update_flag &= ~(1u << IMU_NOTIFY_SHFITS);
        BMI088_gyro_read_over(buf.gyro_dma_rx_buf + BMI088_GYRO_RX_BUF_DATA_OFFSET, rawData.gyro);
    }

    if(state.accel_update_flag & (1u << IMU_UPDATE_SHFITS))
    {
        state.accel_update_flag &= ~(1u << IMU_UPDATE_SHFITS);
        BMI088_accel_read_over(buf.accel_dma_rx_buf + BMI088_ACCEL_RX_BUF_DATA_OFFSET, rawData.accel, &rawData.time);
    }

    if(state.accel_temp_update_flag & (1u << IMU_UPDATE_SHFITS))
    {
        state.accel_temp_update_flag &= ~(1u << IMU_UPDATE_SHFITS);
        BMI088_temperature_read_over(buf.accel_temp_dma_rx_buf + BMI088_ACCEL_RX_BUF_DATA_OFFSET, &rawData.temp);
        imu_temp_control(rawData.temp);
    }

    AHRS_update(quat, 0.001f, rawData.gyro, rawData.accel, rawData.mag);
    get_angle(quat,&attitude.yaw, &attitude.pitch, &attitude.rol);
    attitude.yaw_v = rawData.gyro[2];
    attitude.pitch_v = rawData.gyro[0];
    attitude.rol_v = rawData.gyro[1];

    attitude.neg_yaw_v = -attitude.yaw_v;
    attitude.neg_pitch_v = -attitude.pitch_v;
    attitude.neg_rol_v = -attitude.rol_v;

}
void IMUBase::ITHandle(uint16_t GPIO_Pin) {
    if(GPIO_Pin == INT1_ACCEL_Pin)
    {
        state.accel_update_flag |= 1u << IMU_DR_SHFITS;
        state.accel_temp_update_flag |= 1u << IMU_DR_SHFITS;
        if(state.imu_start_dma_flag)
        {
            imu_cmd_spi_dma();
        }
    }
    else if(GPIO_Pin == INT1_GYRO_Pin)
    {
        state.gyro_update_flag |= 1u << IMU_DR_SHFITS;
        if(state.imu_start_dma_flag)
        {
            imu_cmd_spi_dma();
        }
    }
    /*else if(GPIO_Pin == IST8310_DRDY_Pin)
    {
        state.mag_update_flag |= 1u << IMU_DR_SHFITS;

        if(state.mag_update_flag &= 1u << IMU_DR_SHFITS)
        {
            state.mag_update_flag &= ~(1u<< IMU_DR_SHFITS);
            state.mag_update_flag |= (1u << IMU_SPI_SHFITS);

            ist8310_read_mag(rawData.mag);
        }
    }*/
}

/**
  * @brief          open the SPI DMA accord to the value of imu_update_flag
  * @param[in]      none
  * @retval         none
  */
/**
  * @brief          根据imu_update_flag的值开启SPI DMA
  * @param[in]      temp:bmi088的温度
  * @retval         none
  */
void IMUBase::imu_cmd_spi_dma(void)
{

    //开启陀螺仪的DMA传输
    if( (state.gyro_update_flag & (1u << IMU_DR_SHFITS) ) && !(hspi1.hdmatx->Instance->CR & DMA_SxCR_EN) && !(hspi1.hdmarx->Instance->CR & DMA_SxCR_EN)
        && !(state.accel_update_flag & (1u << IMU_SPI_SHFITS)) && !(state.accel_temp_update_flag & (1u << IMU_SPI_SHFITS)))
    {
        state.gyro_update_flag &= ~(1u << IMU_DR_SHFITS);
        state.gyro_update_flag |= (1u << IMU_SPI_SHFITS);

        HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_RESET);
        SPI_DMA_enable(spiWithDMA, (uint32_t) buf.gyro_dma_tx_buf, (uint32_t) buf.gyro_dma_rx_buf,
                       SPI_DMA_GYRO_LENGHT);
        return;
    }
    //开启加速度计的DMA传输
    if((state.accel_update_flag & (1u << IMU_DR_SHFITS)) && !(hspi1.hdmatx->Instance->CR & DMA_SxCR_EN) && !(hspi1.hdmarx->Instance->CR & DMA_SxCR_EN)
       && !(state.gyro_update_flag & (1u << IMU_SPI_SHFITS)) && !(state.accel_temp_update_flag & (1u << IMU_SPI_SHFITS)))
    {
        state.accel_update_flag &= ~(1u << IMU_DR_SHFITS);
        state.accel_update_flag |= (1u << IMU_SPI_SHFITS);

        HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_RESET);
        SPI_DMA_enable(spiWithDMA, (uint32_t) buf.accel_dma_tx_buf, (uint32_t) buf.accel_dma_rx_buf,
                       SPI_DMA_ACCEL_LENGHT);
        return;
    }


    if((state.accel_temp_update_flag & (1u << IMU_DR_SHFITS)) && !(hspi1.hdmatx->Instance->CR & DMA_SxCR_EN) && !(hspi1.hdmarx->Instance->CR & DMA_SxCR_EN)
       && !(state.gyro_update_flag & (1u << IMU_SPI_SHFITS)) && !(state.accel_update_flag & (1u<< IMU_SPI_SHFITS)))
    {
        state.accel_temp_update_flag &= ~(1u << IMU_DR_SHFITS);
        state.accel_temp_update_flag |= (1u << IMU_SPI_SHFITS);

        HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_RESET);
        SPI_DMA_enable(spiWithDMA, (uint32_t) buf.accel_temp_dma_tx_buf, (uint32_t) buf.accel_temp_dma_rx_buf,
                       SPI_DMA_ACCEL_TEMP_LENGHT);
        return;
    }

}

//TODO 想办法处理下
#ifdef __ROBOMASTER_C
void DMA2_Stream0_IRQHandler(void) {
    imu.ITHandle();
}
#elif __MC_BOARD
void DMA1_Stream3_IRQHandler(void) {
    imu.ITHandle();
}
#endif
void IMUBase::ITHandle() {
    auto hspi = spiWithDMA.spiHandle;
    if(__HAL_DMA_GET_FLAG(hspi->hdmarx, __HAL_DMA_GET_TC_FLAG_INDEX(hspi->hdmarx)) != RESET)
    {
        __HAL_DMA_CLEAR_FLAG(hspi->hdmarx, __HAL_DMA_GET_TC_FLAG_INDEX(hspi->hdmarx));

        //gyro read over
        //陀螺仪读取完毕
        if(state.gyro_update_flag & (1u << IMU_SPI_SHFITS))
        {
            //SystemEstimator::systemEstimator.ITEstimatorTrigger(MPU_GYRO_IT);
            state.gyro_update_flag &= ~(1u << IMU_SPI_SHFITS);
            state.gyro_update_flag |= (1u << IMU_UPDATE_SHFITS);

            HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_SET);

        }

        //accel read over
        //加速度计读取完毕
        if(state.accel_update_flag & (1u << IMU_SPI_SHFITS))
        {
            //SystemEstimator::systemEstimator.ITEstimatorTrigger(MPU_ACCEL_IT);
            state.accel_update_flag &= ~(1u << IMU_SPI_SHFITS);
            state.accel_update_flag |= (1u << IMU_UPDATE_SHFITS);

            HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_SET);
        }
        //temperature read over
        //温度读取完毕
        if(state.accel_temp_update_flag & (1u << IMU_SPI_SHFITS))
        {
            //SystemEstimator::systemEstimator.ITEstimatorTrigger(MPU_TEMP_IT);
            state.accel_temp_update_flag &= ~(1u << IMU_SPI_SHFITS);
            state.accel_temp_update_flag |= (1u << IMU_UPDATE_SHFITS);

            HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_SET);
        }

        imu_cmd_spi_dma();

        if(state.gyro_update_flag & (1u << IMU_UPDATE_SHFITS))
        {
            state.gyro_update_flag &= ~(1u << IMU_UPDATE_SHFITS);
            state.gyro_update_flag |= (1u << IMU_NOTIFY_SHFITS);
        }
    }
}

void IMUBase::ErrorHandle() {

}

void IMUBase::AHRS_update(float quat[4], float time, float gyro[3], float accel[3], float mag[3])
{
#ifdef IMU_USE_MAG
    MahonyAHRSupdate(quat, gyro[0], gyro[1], gyro[2],
            accel[0], accel[1], accel[2],
            mag[0],mag[1], mag[2]);
#else
    MahonyAHRSupdate(quat, gyro[0], gyro[1], gyro[2],
                     accel[0], accel[1], accel[2],
                     0,0,0);
#endif
}

void IMUBase::get_angle(float q[4], float *yaw, float *pitch, float *roll)
{
    *yaw = atan2f(2.0f*(q[0]*q[3]+q[1]*q[2]), 2.0f*(q[0]*q[0]+q[1]*q[1])-1.0f);
    *pitch = asinf(-2.0f*(q[1]*q[3]-q[0]*q[2]));
    *roll = atan2f(2.0f*(q[0]*q[1]+q[2]*q[3]),2.0f*(q[0]*q[0]+q[3]*q[3])-1.0f);
}


/**
  * @brief          控制bmi088的温度
  * @param[in]      temp:bmi088的温度
  * @retval         none
  */
void IMUBase::imu_temp_control(float temp)
{
    uint16_t tempPWM;
    static uint8_t temp_constant_time = 0;
    if (first_temperate)
    {
        tempPid.PIDCalc(45.0f,temp);
        if (tempPid.PIDInfo.output < 0.0f)
        {
            tempPid.PIDInfo.output = 0.0f;
        }
        tempPWM = (uint16_t)tempPid.PIDInfo.output;

        IMU_temp_PWM(tempPWM);

    }
    else
    {
        //在没有达到设置的温度，一直最大功率加热
        //in beginning, max power
        if (temp > 45.0f)
        {
            temp_constant_time++;
            if (temp_constant_time > 200)
            {
                //达到设置温度，将积分项设置为一半最大功率，加速收敛
                //
                first_temperate = 1;
                tempPid.PIDInfo.componentKiMax = MPU6500_TEMP_PWM_MAX / 2.0f;
            }
        }

        IMU_temp_PWM(MPU6500_TEMP_PWM_MAX - 1);
    }
}

void IMUBase::IMU_temp_PWM(float pwm) {
    __HAL_TIM_SetCompare(spiWithDMA.timHandleForHeat, spiWithDMA.timChannelForHeat, pwm);
}

