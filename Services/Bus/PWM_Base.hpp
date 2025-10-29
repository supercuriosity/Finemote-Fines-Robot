/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/
#ifndef FINEMOTE_PWM_BASE_H
#define FINEMOTE_PWM_BASE_H

#include "BSP_PWM.h"

template <size_t ID>
class PWM_Base {
public:
    static PWM_Base &GetInstance() {
        static PWM_Base instance;
        return instance;
    }

    void SetDutyCycle(float dutyCycle) {
        BSP_PWM<ID>::GetInstance().SetDutyCycle(dutyCycle);
    }

    void SetFrequency(uint32_t frequency) {
        BSP_PWM<ID>::GetInstance().SetFrequency(frequency);
    }

private:
    PWM_Base() {
        BSP_PWM<ID>::GetInstance();
    }
    ~PWM_Base() = default;
    PWM_Base(const PWM_Base &) = delete;
    PWM_Base &operator=(const PWM_Base &) = delete;
};

#endif
