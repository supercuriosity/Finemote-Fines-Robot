/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_PID_HPP
#define FINEMOTE_PID_HPP

#include "ImplementControlBase.hpp"

typedef struct PID_Param_t {
    float kp;
    float ki;
    float kd;
} PID_Param_t;

class PID : public ImplementControllerBase<1, 1> {
public:
    constexpr explicit PID(const PID_Param_t& params) : params(params) {}

    void PerformCalc() override {
        float error = *(this->targetPtrs[0]) - *(this->feedbackPtrs[0]);
        totalError += error;
        //Clamp(totalError, -params.iMax, params.iMax);
        this->outputs[0] = params.kp * error + params.ki * totalError + params.kd * (error - lastError);
        lastError = error;
    }

    ControllerOutputData GetOutputs() override {
        return { this->outputs.data(), 1 };
    }

private:
    const PID_Param_t params;
    float totalError = 0;
    float lastError = 0;
};

#endif
