/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "ProjectConfig.h"

#include "MultiMedia/BeepMusic.h"
#include "DeviceBase.h"
#include "MultiMedia/LED.h"


void Task1();
void Task2();

#ifdef __cplusplus
extern "C" {
#endif
//TODO Setup 应该尽可能减少使用而使用构造函数替代，loop应该通过继承Devicebase并重写handle函数来替代


void Setup() {
    HAL_TIM_Base_Start_IT(&TIM_Control);
    HAL_TIM_PWM_Start(&TIM_Buzzer,TIM_Buzzer_Channel);

    uint8_t ss[7] = "Hello\n";
    //HAL_UART_Transmit_IT(&Serial_Host, ss, 7);
    BeepMusic::MusicChannels[0].Play(3);
}

void Loop() {
    uint8_t ss[7] = "Hello\n";
    HAL_UART_Transmit_IT(&Serial_Host, ss, 7);
    HAL_Delay(1000);
}

#ifdef __cplusplus
};
#endif

//I2C_test<2> i2CTest(0x70);


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t datasize)
{
    if(huart == &Serial_Host) {
        //i2CTest.Trigger();
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
    if(huart == &Serial_Host) {
        //i2CTest.rxState = HAL_ERROR;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim == &TIM_Control) {
        HAL_IWDG_Refresh(&hiwdg);
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
        LED::Toggle();
    }
}
