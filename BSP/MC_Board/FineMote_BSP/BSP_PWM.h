/*******************************************************************************
 * Copyright (c) 2025.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_BSP_PWM_H
#define FINEMOTE_BSP_PWM_H

#include "Board.h"

/**
 * BSP Specific Definitions Begin
 */

namespace BSP_PWM_IMPL {

inline void BSP_PWM_SetDutyCycle(uint8_t ID, float _dutyCycle) {
    uint32_t counterPeriod = __HAL_TIM_GET_AUTORELOAD(BSP_PWMList[ID].TIM_Handle);
    __HAL_TIM_SET_COMPARE(BSP_PWMList[ID].TIM_Handle, BSP_PWMList[ID].TIM_CHANNEL, _dutyCycle * counterPeriod);
}

inline void BSP_PWM_SetFrequency(uint8_t ID, uint32_t frequency) {
    uint32_t timerClock = BSP_PWMList[ID].TIM_Frequency * 1000000; // Convert MHz to Hz
    uint32_t prescaler = BSP_PWMList[ID].TIM_Handle->Instance->PSC;
    __HAL_TIM_SET_AUTORELOAD(BSP_PWMList[ID].TIM_Handle, (timerClock / (prescaler + 1)) / frequency);
}

inline void BSP_PWM_Setup(uint8_t ID) {
    // default frequency 50Hz, default compare 0
    BSP_PWM_SetDutyCycle(ID, 0);
    HAL_TIM_PWM_Start(BSP_PWMList[ID].TIM_Handle, BSP_PWMList[ID].TIM_CHANNEL);
}

}
/**
 * BSP Specific Definitions End
 */

class BSP_PWMs {
public:
    static BSP_PWMs &GetInstance() {
        static BSP_PWMs instance;
        return instance;
    }

private:
    BSP_PWMs() { PeripheralsInit::GetInstance(); }
};

template <uint8_t ID> class BSP_PWM {
public:
    static BSP_PWM &GetInstance() {
        static BSP_PWM instance;
        return instance;
    }

  void SetDutyCycle(float dutyCycle) {
      BSP_PWM_IMPL::BSP_PWM_SetDutyCycle(ID, dutyCycle);
  }

  void SetFrequency(uint32_t frequency) {
      BSP_PWM_IMPL::BSP_PWM_SetFrequency(ID, frequency);
  }

private:
    BSP_PWM() {
        static_assert(ID > 0 && ID < sizeof(BSP_PWMList) / sizeof(BSP_PWMList[0]) &&
                          BSP_PWMList[ID].TIM_Handle != nullptr,
                      "Invalid PWM ID");
        BSP_PWMs::GetInstance();
        BSP_PWM_IMPL::BSP_PWM_Setup(ID);
    }
};

#endif
