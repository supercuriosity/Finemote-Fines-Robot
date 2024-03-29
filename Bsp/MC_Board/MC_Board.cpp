
/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "MC_Board.h"
UART_HandleTypeDef uartHandleList[4] ={huart3,huart1,huart2,huart5};

extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi2_rx;
extern DMA_HandleTypeDef hdma_spi2_tx;

SPI_WITH_DMA_t spiWithDMA{&hspi2,&hdma_spi2_rx,&hdma_spi2_tx,
                          &htim3,TIM_CHANNEL_2};

