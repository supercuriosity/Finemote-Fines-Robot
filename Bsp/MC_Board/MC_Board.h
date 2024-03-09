/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_MC_BOARD_H
#define FINEMOTE_MC_BOARD_H

#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "iwdg.h"
#include "stm32f4xx_it.h"



#ifdef __cplusplus
extern "C" {
#endif
extern void SystemClock_Config();
extern void SystemClock_PreConfig();
extern void SystemClock_PostConfig();
#ifdef __cplusplus
};
#endif

class HALInit{
/**
 *  @attention 此函数需要与cube生成的保持一致
 */
    HALInit(){

        HAL_Init();
        SystemClock_PreConfig();
        SystemClock_Config();
        SystemClock_PostConfig();

        //* USER CODE BEGIN SysInit *//

        //* USER CODE END SysInit *//

        //* Initialize all configured peripherals *//
        MX_GPIO_Init();
        MX_DMA_Init();
        MX_CAN1_Init();
        MX_CAN2_Init();
        MX_SPI1_Init();
        MX_UART5_Init();
        MX_USART1_UART_Init();
        MX_ADC1_Init();
        MX_I2C1_Init();
        MX_TIM8_Init();
        MX_I2C3_Init();
        MX_USART2_UART_Init();
        MX_TIM3_Init();
        MX_TIM2_Init();
        MX_TIM7_Init();
//        MX_IWDG_Init();
        MX_USART3_UART_Init();
        MX_SPI2_Init();
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

#define DMA_SBUS hdma_usart3_rx
#define Serial_RC huart3
#define User_UART huart2
#define UART_PERIPHERAL

#define Serial_User huart4
#define Serial_Host huart5

#define TIM_Buzzer htim2
#define TIM_Buzzer_Channel TIM_CHANNEL_4
//#define BUZZER_PERIPHERAL

#define LED_GPIO_Port   GPIOC
#define LED_Pin         GPIO_PIN_0
#define LED_PERIPHERAL

#define TIM_Heater htim3
#define TIM_Heater_Channel TIM_CHANNEL_2

#define TIM_Control htim7

#define CAN_BUS_MAXIMUM_COUNT 2
#define CAN_PERIPHERAL
#endif //FINEMOTE_MC_BOARD_H
