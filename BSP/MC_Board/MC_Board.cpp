
/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "MC_Board.h"

UART_HandleTypeDef *uartHandleList[2] = {&huart3, &huart5};
UART_HandleTypeDef *rs485HandleList[2] = {&huart1, &huart2};
GPIO_TypeDef *rs485TxPortList[2] = {GPIOC, GPIOB};
uint16_t rs485TxPinList[2] = {GPIO_PIN_15, GPIO_PIN_3};

extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi2_rx;
extern DMA_HandleTypeDef hdma_spi2_tx;

SPI_WITH_DMA_t spiWithDMA{&hspi2,&hdma_spi2_rx,&hdma_spi2_tx,
                          &htim3,TIM_CHANNEL_2};

