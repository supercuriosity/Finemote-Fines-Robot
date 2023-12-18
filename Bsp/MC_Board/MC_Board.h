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
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#define Serial_User huart4
#define Serial_Host huart5

#define TIM_Buzzer htim2
#define TIM_Buzzer_Channel TIM_CHANNEL_4

#define TIM_Control htim7

#endif //FINEMOTE_MC_BOARD_H
