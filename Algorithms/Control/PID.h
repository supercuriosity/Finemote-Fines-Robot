/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_PID_H
#define FINEMOTE_PID_H

#include <vector>

#include "ControlBase.h"

#define INRANGE(NUM, MIN, MAX) \
    NUM = ((NUM > (MIN) ? NUM : (MIN)) \
          < (MAX) ? NUM : (MAX))

typedef struct PID_Param_t {
    float kp;
    float ki;
    float kd;
    float iMax;
    float outputMax;
} PID_Param_t;

class PID : public ControllerBase {
public:
    constexpr explicit PID(const PID_Param_t& params) : params(params) {}

    const float& Calc() override{
        float error = *targetPtr - *feedbackPtr;
        totalError += error;
        INRANGE(totalError, -1 * params.iMax, params.iMax);
        output = params.kp * error + params.ki * totalError + params.kd * (error - lastError);
        lastError = error;
        return INRANGE(output, -1 * params.outputMax, params.outputMax);
    }

private:
    const PID_Param_t params;
    float totalError = 0, lastError = 0;
};

template<size_t K>
class CascadePID : public PID {
public:
    template<typename... Params>
    constexpr explicit CascadePID(const PID_Param_t& first, Params... params) : PID(first), nodes{PID(params)...} {
        static_assert(K > 1, "CascadePID should have at least 2 layers");

        Cascade(nodes[0]);
        for (int i = 0; i < K - 2; ++i) {
            nodes[i].Cascade(nodes[i + 1]);
        }
    }

    const float& Calc() final{
        PID::Calc();
        for(auto& node : nodes) {
            node.Calc();
        }

        output = nodes.rbegin()->GetOutput();
        return output;
    }

    void SetFeedback(const std::vector<float*>& feedbackPtrs) final {
        //static_assert(K == feedbackPtrs.size(), "Feedback number should be equal to K");

        auto iter = feedbackPtrs.begin();
        PID::SetFeedback({*iter});
        iter++;
        for(auto& node : nodes) {
            node.SetFeedback({*iter});
            iter++;
        }
    }

private:
    std::array<PID, K - 1> nodes;
};

#endif //FINEMOTE_PID_H
