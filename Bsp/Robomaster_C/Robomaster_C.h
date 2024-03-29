/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_ROBOMASTER_C_H
#define FINEMOTE_ROBOMASTER_C_H

#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
extern void SystemClock_Config();
#ifdef __cplusplus
};
#endif

class HALInit{
/**
 *  @attention 此函数需要与cube生成的保持一致
 */
    HALInit(){
        HAL_Init();
        SystemClock_Config();

        /* USER CODE BEGIN SysInit */

        /* USER CODE END SysInit */

        /* Initialize all configured peripherals */
        MX_GPIO_Init();
        MX_DMA_Init();
        MX_ADC1_Init();
        MX_ADC3_Init();
        MX_CAN1_Init();
        MX_CAN2_Init();
        MX_SPI1_Init();
        MX_TIM4_Init();
        MX_TIM5_Init();
        MX_USART3_UART_Init();
        MX_TIM8_Init();
        MX_I2C3_Init();
        MX_TIM1_Init();
        MX_TIM3_Init();
        MX_TIM10_Init();
        MX_USART1_UART_Init();
        MX_USART6_UART_Init();
        MX_TIM6_Init();
       // MX_IWDG_Init();
        MX_TIM7_Init();
        MX_I2C2_Init();
    };

public:
    HALInit(const HALInit &) = delete;
    HALInit& operator=(const HALInit&) = delete;
    static HALInit& GetInstance(){
        static HALInit instance;
        return instance;
    }
};
#define HAL_INIT_HANDLE

#define Serial_Host huart6
#define Serial_RS4851 huart1

#define TIM_Buzzer htim4
#define TIM_Buzzer_Channel TIM_CHANNEL_3
#define BUZZER_PERIPHERAL

#define LED_GPIO_Port   LED_B_GPIO_Port
#define LED_Pin         LED_B_Pin
#define LED_PERIPHERAL

#define TIM_Control htim7
#define SPI_BMI088 hspi1 /** todo */

#define CAN_BUS_MAXIMUM_COUNT 2

extern UART_HandleTypeDef uartHandleList[3];
#define UART_PERIPHERAL

#define USER_I2C hi2c2
#define I2C_PERIPHERAL
#define CAN_PERIPHERAL


typedef struct {
    TIM_HandleTypeDef* timerPtr; // 定时器HAL对象指针
    uint32_t channel;              // 定时器通道
    uint32_t activeFlag;
}PWM_UNIT_t;
extern PWM_UNIT_t pwmList[7];
#define PWM_PERIPHERAL


typedef struct {
    SPI_HandleTypeDef* spiHandle;
    DMA_HandleTypeDef* rxDMAHandle;
    DMA_HandleTypeDef* txDMAHandle;
    TIM_HandleTypeDef* timHandleForHeat;
    uint32_t timChannelForHeat;
}SPI_WITH_DMA_t;
extern SPI_WITH_DMA_t spiWithDMA;

#define IMU_PERIPHERAL


#endif //FINEMOTE_ROBOMASTER_C_H
