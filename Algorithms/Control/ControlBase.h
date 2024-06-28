/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_CONTROLBASE_H
#define FINEMOTE_CONTROLBASE_H

#include <utility>
#include <array>
#include <vector>

/**
 * 控制器的输出应当不是一个立即数，而是根据指定地址存放数据，从而保证实际控制发生前控制量的生命周期
 */

class ControllerBase {
public:
    ControllerBase() = default;
    explicit ControllerBase(float* _targetPtr) : targetPtr(_targetPtr) {}

    virtual const float& Calc() = 0;

    void Cascade(ControllerBase& next) {
        next.targetPtr = &output;
    }

    const float& GetOutput() const {
        return output;
    }

    void SetTarget(float* _targetPtr) {
        this->targetPtr = _targetPtr;
    }

    virtual void SetFeedback(const std::vector<float*>& feedbackPtrs) {
        this->feedbackPtr = feedbackPtrs[0];
    }

protected:
    float* targetPtr = nullptr;
    float* feedbackPtr = nullptr;
    float output = 0;
};

template<typename T, size_t M, typename... Args, size_t... I>
std::array<T, M> CreateControllersImpl(std::index_sequence<I...>, Args&&... args) {
    return { (static_cast<void>(I), T{std::forward<Args>(args)...})... };
}

template<typename T, size_t M, typename... Args>
auto CreateControllers(Args&&... args) {
    static_assert(!std::is_same<T, ControllerBase>::value, "ControllerBase is not allowed");
    static_assert(sizeof...(Args) <= 1, "Only one parameter is allowed");
    return CreateControllersImpl<T, M>(std::make_index_sequence<M>{}, std::forward<Args>(args)...);
}

template<template<size_t> typename T, size_t M, typename... Args>
auto CreateControllers(Args&&... args) {
    constexpr size_t N = sizeof...(Args);
    return CreateControllersImpl<T<N>, M>(std::make_index_sequence<M>{}, std::forward<Args>(args)...);
}

template <int K = 1>
class Amplifier : public ControllerBase {
    const float& Calc() final {
        output = K * (*targetPtr);
        return output;
    }
};

#endif //FINEMOTE_CONTROLBASE_H
