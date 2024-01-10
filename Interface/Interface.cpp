/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "PeripheralDefination.h"

#include "MultiMedia/BeepMusic.h"

void Task1();
void Task2();

#ifdef __cplusplus
extern "C" {
#endif

void Setup() {
    HAL_TIM_Base_Start_IT(&TIM_Control);
    HAL_TIM_PWM_Start(&TIM_Buzzer,TIM_Buzzer_Channel);
}

void Loop() {
    uint8_t ss[7] = "Hello\n";
    HAL_UART_Transmit_IT(&Serial_Host, ss, 7);
    HAL_Delay(1000);
}

#ifdef __cplusplus
};
#endif

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim == &TIM_Control) {

        DeviceBase::DevicesHandle();

        Task1();
        Task2();


        if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)) {
            static int index = 1;
            BeepMusic::MusicChannels[0].Play(index++);
            index %= 3;
        }
    }
}

void Task1() {
    BeepMusic::MusicChannels[0].BeepService();
}

void Task2() {
    static int cnt = 0;
    cnt++;
    if(cnt > 1000) {
        cnt = 0;
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    }
}
