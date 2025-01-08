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

extern int main();

#ifdef __cplusplus
}
#endif

class HALInit {
/**
 *  @attention 此函数需要与cube生成的保持一致
 */
    HALInit() {
        main();
    };

public:
    HALInit(const HALInit &) = delete;

    HALInit &operator=(const HALInit &) = delete;

    static HALInit &GetInstance() {
        static HALInit instance;
        return instance;
    }
};

#define HAL_INIT_HANDLE

extern UART_HandleTypeDef *uartHandleList[];
extern GPIO_TypeDef *rs485TxPortList[3];
extern uint16_t rs485TxPinList[3];


#define UART_PERIPHERAL


#define TIM_Buzzer htim2
#define TIM_Buzzer_Channel TIM_CHANNEL_4
#define BUZZER_PERIPHERAL

#define LED_GPIO_Port   GPIOC
#define LED_Pin         GPIO_PIN_0
#define LED_PERIPHERAL

#define TIM_Heater htim3
#define TIM_Heater_Channel TIM_CHANNEL_2

#define TIM_Control htim7

#define CAN_PERIPHERAL
constexpr CAN_HandleTypeDef* CAN_Buses[] = {&hcan1, &hcan2};
constexpr uint8_t CAN_BUS_MAXIMUM_COUNT = sizeof(CAN_Buses) / sizeof(CAN_HandleTypeDef*);

#define RS485_PERIPHERAL

typedef struct {
    SPI_HandleTypeDef *spiHandle;
    DMA_HandleTypeDef *rxDMAHandle;
    DMA_HandleTypeDef *txDMAHandle;
    TIM_HandleTypeDef *timHandleForHeat;
    uint32_t timChannelForHeat;
} SPI_WITH_DMA_t;
extern SPI_WITH_DMA_t spiWithDMA;
#define IMU_PERIPHERAL

#endif //FINEMOTE_MC_BOARD_H
