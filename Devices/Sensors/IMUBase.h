
/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_IMUBASE_H
#define FINEMOTE_IMUBASE_H


#include "DeviceBase.h"
#include "Control//PID.h"
#include "BMI088.h"
#include "MahonyAHRS.h"
#include "Bus/bsp_spi.h"
#include <cmath>

//#define IMU_USE_MAG

#define SPI_DMA_GYRO_LENGHT       8
#define SPI_DMA_ACCEL_LENGHT      9
#define SPI_DMA_ACCEL_TEMP_LENGHT 4

#define IMU_DR_SHFITS        0u
#define IMU_SPI_SHFITS       1u
#define IMU_UPDATE_SHFITS    2u
#define IMU_NOTIFY_SHFITS    3u


#define BMI088_GYRO_RX_BUF_DATA_OFFSET  1u
#define BMI088_ACCEL_RX_BUF_DATA_OFFSET 2u

//ist83100原始数据在缓冲区buf的位置
#define IST8310_RX_BUF_DATA_OFFSET 16

#define MPU6500_TEMP_PWM_MAX 1000 //mpu6500控制温度的设置TIM的重载值，即给PWM最大为 MPU6500_TEMP_PWM_MAX - 1

/*枚举类型定义------------------------------------------------------------*/
/*结构体定义--------------------------------------------------------------*/
typedef struct{
    float accel[3],gyro[3],temp,time,mag[3];
}IMU_Raw_Data_t;

typedef struct{
    float yaw,pitch,rol;
    float yaw_v,pitch_v,rol_v;
    float neg_yaw_v,neg_pitch_v,neg_rol_v;
}IMU_Attitude_t;

typedef struct{
    volatile uint8_t gyro_update_flag = 0;
    volatile uint8_t accel_update_flag = 0;
    volatile uint8_t accel_temp_update_flag = 0;
    volatile uint8_t mag_update_flag = 0;
    volatile uint8_t imu_start_dma_flag = 0;
}IMU_state_t;

typedef struct{
    uint8_t gyro_dma_rx_buf[SPI_DMA_GYRO_LENGHT];
    uint8_t gyro_dma_tx_buf[SPI_DMA_GYRO_LENGHT] = {0x82,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    uint8_t accel_dma_rx_buf[SPI_DMA_ACCEL_LENGHT];
    uint8_t accel_dma_tx_buf[SPI_DMA_ACCEL_LENGHT] = {0x92,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    uint8_t accel_temp_dma_rx_buf[SPI_DMA_ACCEL_TEMP_LENGHT];
    uint8_t accel_temp_dma_tx_buf[SPI_DMA_ACCEL_TEMP_LENGHT] = {0xA2,0xFF,0xFF,0xFF};
}IMU_buffer_t __packed;

/*类型定义----------------------------------------------------------------*/

class IMUBase : private DeviceBase
{


    void ErrorHandle();

    //读取数据


    IMU_buffer_t buf;

    IMU_state_t state;
    void imu_cmd_spi_dma(void);

    //姿态解算
    float quat[4];

    void AHRS_update(float quat[4], float time, float gyro[3], float accel[3], float mag[3]);
    void get_angle(float q[4], float *yaw, float *pitch, float *roll);
    //温度控制
    PID tempPid;
    uint8_t first_temperate;
    void imu_temp_control(float temp);
    void IMU_temp_PWM(float pwm);

public:
    void Handle() override;
    void ITHandle(uint16_t GPIO_Pin);
    void ITHandle(void);
    void Init();
    IMU_Raw_Data_t rawData;
    IMU_Attitude_t attitude;
    IMUBase();
};


/*结构体成员取值定义组------------------------------------------------------*/
/*外部变量声明-------------------------------------------------------------*/
/*外部函数声明-------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
extern void DMA2_Stream0_IRQHandler(void);
#ifdef __cplusplus
}
#endif


#endif //FINEMOTE_IMUBASE_H
