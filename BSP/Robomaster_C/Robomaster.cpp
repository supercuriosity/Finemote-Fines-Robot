/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "Robomaster_C.h"

#include "BeepMusic.h"

PWM_UNIT_t pwmList[7] = {
{&htim1,TIM_CHANNEL_1},
{&htim1,TIM_CHANNEL_2},
{&htim1,TIM_CHANNEL_3},
{&htim1,TIM_CHANNEL_4},
{&htim8,TIM_CHANNEL_1},
{&htim8,TIM_CHANNEL_2},
{&htim8,TIM_CHANNEL_3}
};

UART_HandleTypeDef *uartHandleList[3] = {&huart3, &huart1, &huart6};
GPIO_TypeDef *rs485TxPortList[3] = {nullptr, nullptr, nullptr};
uint16_t rs485TxPinList[3] = {NULL, NULL, NULL};

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;

SPI_WITH_DMA_t spiWithDMA{&hspi1,&hdma_spi1_rx,&hdma_spi1_tx,
                          &htim10,TIM_CHANNEL_1};

#ifdef __cplusplus
extern "C" {
#endif

void BSP_Setup() {
    HAL_TIM_Base_Start_IT(&TIM_Control);
    HAL_TIM_PWM_Start(&TIM_Buzzer,TIM_Buzzer_Channel);
    BeepMusic::MusicChannels[0].Play(3);
}

#ifdef __cplusplus
}
#endif
