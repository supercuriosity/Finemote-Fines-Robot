/*******************************************************************************
 * Copyright (c) 2025.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_CONTROLBASE_HPP
#define FINEMOTE_CONTROLBASE_HPP

#include <array>
#include <cstdint>
#include <functional>

using ControllerOutputData = struct ControllerOutputData{
    float* dataPtr;
    uint8_t size;
};

class ControllerBase {
public:
    ControllerBase() = default;
    virtual ~ControllerBase() = default;

    ControllerOutputData Calc() {
        PerformCalc();
        if (nextCalc) {
            return nextCalc();
        }
        return GetOutputs();
    }

protected:

    virtual void PerformCalc() = 0;
    virtual ControllerOutputData GetOutputs() = 0;

    std::function<ControllerOutputData()> nextCalc = nullptr;
};


template <size_t InputSize, size_t OutputSize>
class ControllerBaseImpl  : public ControllerBase {
public:
    ControllerBaseImpl() = default;
    virtual ~ControllerBaseImpl() = default;

    template <size_t NextInputSize, size_t NextOutputSize>
    void Cascade( ControllerBaseImpl<NextInputSize, NextOutputSize>& nextController ) {
        static_assert(OutputSize == NextInputSize, "Output size of current controller must match input size of the next one.");
        nextController.SetTargets(this->outputs);
        nextCalc = std::bind(&ControllerBaseImpl<NextInputSize, NextOutputSize>::Calc, &nextController);
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
        return { outputs.data(), OutputSize };
    }

protected:

    std::array<float*, InputSize> targetPtrs{};
    std::array<float*, InputSize> feedbackPtrs{};
    std::array<float, OutputSize> outputs{};
};


#endif
