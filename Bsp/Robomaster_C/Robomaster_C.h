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

#define Serial_Host huart6
#define Serial_RS4851 huart1

#define TIM_Buzzer htim4
#define TIM_Buzzer_Channel TIM_CHANNEL_3

#define TIM_Control htim7

#define SPI_BMI088 hspi1 /** todo */

#define CAN_BUS_MAXIMUM_COUNT 2

#define USER_I2C hi2c2

 typedef struct {
    TIM_HandleTypeDef* timerPtr; // 定时器HAL对象指针
    uint32_t channel;              // 定时器通道
    uint32_t activeFlag=0;
}PWM_UNIT_t;

extern PWM_UNIT_t pwmList[7];

#endif //FINEMOTE_ROBOMASTER_C_H
