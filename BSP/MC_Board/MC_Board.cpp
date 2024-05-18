
/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "MC_Board.h"

UART_HandleTypeDef *uartHandleList[5] = {nullptr, &huart1, &huart2, &huart3, &huart5};
GPIO_TypeDef *rs485TxPortList[3] = {nullptr, GPIOC, GPIOB};
uint16_t rs485TxPinList[3] = {NULL, GPIO_PIN_15, GPIO_PIN_3};


extern DMA_HandleTypeDef hdma_spi2_rx;
extern DMA_HandleTypeDef hdma_spi2_tx;

SPI_WITH_DMA_t spiWithDMA{&hspi2,&hdma_spi2_rx,&hdma_spi2_tx,
                          &htim3,TIM_CHANNEL_2};

