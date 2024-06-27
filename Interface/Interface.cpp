/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "ProjectConfig.h"
#include "BeepMusic.h"
#include "DeviceBase.h"
#include "LED.h"
#include "Motor4010.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 用户初始化
 */
void Setup() {

}

/**
 * @brief 主循环，优先级低于定时器中断，不确定执行频率
 */
void Loop() {

}

#ifdef __cplusplus
};
#endif

/*****  示例1 *****/
/**
 * @brief LED闪烁
 */
void Task1() {
    static int cnt = 0;
    cnt++;
    if(cnt > 1000) {
        cnt = 0;
        LED::Toggle();
    }
}

/*****  示例2 *****/
/**
 * @brief 音乐播放与切换
 */
void Task2() {
    if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)) {
        static int index = 1;
        BeepMusic::MusicChannels[0].Play(index++);
        index %= 3;
    }
    BeepMusic::MusicChannels[0].BeepService();
}

/*****  示例3 *****/
#include "Chassis.h"
#include "RadioMaster_Zorro.h"

extern Chassis chassis;
extern RadioMaster_Zorro remote;

/**
 * @brief 底盘根据遥控器数据运动
 */
void Task3() {
    chassis.ChassisSetVelocity(remote.GetInfo().rightCol, remote.GetInfo().rightRol, remote.GetInfo().leftRol * 2 * PI * 2);
}

/*****  不要修改以下代码 *****/

#ifdef __cplusplus
extern "C" {
#endif

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim == &TIM_Control) {
        HAL_IWDG_Refresh(&hiwdg);
        DeviceBase::DevicesHandle();
        Task1();
        Task2();
        Task3();

        CAN_Bus<1>::TxLoader();
        CAN_Bus<2>::TxLoader();
    }
}

#ifdef __cplusplus
};
#endif
