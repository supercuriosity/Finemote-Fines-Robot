/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_SERVO_H
#define FINEMOTE_SERVO_H

#include "Bus/PWM_Base.hpp"

template<size_t deviceID>
class Servo {
private:
    static constexpr float period         = 20.0f;   // ms
    static constexpr float startDuration  = 0.5f;    // ms
    static constexpr float endDuration    = 2.5f;    // ms
    static constexpr float fullAngle      = 180.0f;  // °

    PWM_Base<deviceID>& pwm = PWM_Base<deviceID>::GetInstance();

public:
    void SetAngle(float angle) {
        float duration = angle / fullAngle * (endDuration - startDuration) + startDuration;
        float duty     = duration / period;
        pwm.SetDutyCycle(duty);
    }

    void Start() { /* 空实现或留接口 */ }
    void Stop()  { /* 空实现或留接口 */ }
};

#endif