/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "PeripheralDefination.h"

#ifdef __cplusplus
extern "C" {
#endif

void Setup() {

}

void Loop() {
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    uint8_t ss[7] = "Hello\n";
    HAL_UART_Transmit_IT(&Serial_Host, ss, 7);

    HAL_Delay(500);
}

#ifdef __cplusplus
};
#endif
