/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_SERVO_H
#define FINEMOTE_SERVO_H

#include "Controllers/PWM.h"

/**
 * 只考虑了180角度舵机
 */
template<uint32_t deviceID>
class Servo {
private:
    float period = 20; //ms
    float startDuration = 0.5;//ms
    float endDuration = 2.5;//ms
    float fullAngle = 180; //角度
    PWM<deviceID> pwm;
public:
    void SetAngle(float angle){
        float compare = angle / fullAngle * (endDuration - startDuration) + startDuration;
        compare = compare/period;
        pwm.SetCompare(compare);
    };
    void Start(){
        pwm.Start();
    };
    void Stop(){
        pwm.Stop();
    };
};


#endif //FINEMOTE_SERVO_H
