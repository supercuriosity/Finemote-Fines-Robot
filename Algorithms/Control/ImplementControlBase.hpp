/*******************************************************************************
 * Copyright (c) 2025.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_IMPLEMENTCONTROLBASE_HPP
#define FINEMOTE_IMPLEMENTCONTROLBASE_HPP


#include <array>
#include <cstdint>
#include <functional>
#include <Control/ControlBase.hpp>


template <size_t InputSize, size_t OutputSize>
class ImplementControllerBase  : public ControllerBase {
public:
    ImplementControllerBase() = default;
    virtual ~ImplementControllerBase() = default;

    template <size_t NextInputSize, size_t NextOutputSize>
    void Cascade(ImplementControllerBase<NextInputSize, NextOutputSize>& nextController) {
        static_assert(OutputSize == NextInputSize, "Output size of current controller must match input size of the next one.");
        nextController.SetTargets(this->outputs);
        nextCalc = std::bind(&ImplementControllerBase<NextInputSize, NextOutputSize>::Calc, &nextController);
    }

    void SetTargets(std::array<float, InputSize>& sourceOutputs) {
        for (size_t i = 0; i < InputSize; ++i) {
            targetPtrs[i] = &sourceOutputs[i];
        }
    }

    template <typename... Args>
    void SetTargets(Args... args) {
        static_assert(sizeof...(args) == InputSize, "Number of target pointers must match controller's input size.");
        targetPtrs = {args...};
    }


    template <typename... Args>
    void SetFeedbacks(Args... args) {
        static_assert(sizeof...(args) == InputSize, "Number of feedback pointers must match controller's input size.");
        feedbackPtrs = {args...};
    }


    ControllerOutputData GetOutputs() const {
        return { (float*)outputs.data(), (uint8_t)OutputSize };
    }

protected:

    std::array<float*, InputSize> targetPtrs{};
    std::array<float*, InputSize> feedbackPtrs{};
    std::array<float, OutputSize> outputs{};
};

template <size_t K>
class Amplifier : public ImplementControllerBase<1, 1> {
public:
    Amplifier() = default;
    void PerformCalc() override {

        this->outputs[0] = *(this->targetPtrs[0]) * static_cast<float>(K);
    }
    ControllerOutputData GetOutputs() override {
        return { this->outputs.data(), 1 };
    }
};

#endif
