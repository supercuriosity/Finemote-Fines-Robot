/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_PID_H
#define FINEMOTE_PID_H

# include "Interface.h"

typedef struct PID_Regulator {
    float ref;
    float fdb;
    float err[4];
    float errSum;
    float kp;
    float ki;
    float kd;
    float componentKp;
    float componentKi;
    float componentKd;
    float componentKpMax;
    float componentKiMax;
    float componentKdMax;
    float output;
    float outputMax;
} PID_Regulator_t;

class PID{
public:

    PID_Regulator_t PIDInfo{};
    void Reset(PID_Regulator_t * pidRegulator);
    void Reset();
    float PIDCalc(float target,float feedback);
    float PIDCalc(float target,float feedback,float max);
};

#endif //FINEMOTE_PID_H
