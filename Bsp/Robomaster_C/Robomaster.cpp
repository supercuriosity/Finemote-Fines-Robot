/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "Robomaster_C.h"

PWM_UNIT_t pwmList[7] = {
{&htim1,TIM_CHANNEL_1},
{&htim1,TIM_CHANNEL_2},
{&htim1,TIM_CHANNEL_3},
{&htim1,TIM_CHANNEL_4},
{&htim8,TIM_CHANNEL_1},
{&htim8,TIM_CHANNEL_2},
{&htim8,TIM_CHANNEL_3}
};
UART_HandleTypeDef uartHandleList[3]={huart3,
                                      huart1,
                                      huart6};

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;

SPI_WITH_DMA_t spiWithDMA{&hspi1,&hdma_spi1_rx,&hdma_spi1_tx,
                          &htim10,TIM_CHANNEL_1};
