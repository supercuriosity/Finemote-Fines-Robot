#ifndef FINEMOTE_MOTORBASE_H
#define FINEMOTE_MOTORBASE_H

#include "DeviceBase.h"
#include "Control/ImplementControlBase.hpp"
#include <cstdint>

enum class Motor_Ctrl_Type_e: uint16_t {
    Position = 0,
    Speed,
    Torque,
};

typedef struct {
    float position; //单位为度
    float speed; //单位为DPS
    float torque; //转矩电流的相对值，具体值参考电调手册
    int8_t temperature; //电机温度，单位摄氏度
} Motor_State_t;

using Motor_Param_t = struct Motor_Param_t {
    Motor_Ctrl_Type_e ctrlType; //控制电机的方式
    Motor_Ctrl_Type_e targetType; //控制电机哪个状态
    bool multiTurnSamePosition = false; //多圈电机是否在同一位置
    const float reductionRatio = 1; //减速比
};

class MotorBase : public DeviceBase {
public:
    explicit MotorBase(const Motor_Param_t&& params) : params(params) {

    }


    void ResetController(ImplementControllerBase<1,1>& _controller) {

        controller = &_controller;
        _controller.SetTargets(&target);
        this->SetFeedback();
    }

    void Stop() {
        switch (params.targetType) {
            case Motor_Ctrl_Type_e::Position:
                SetTargetAngle(state.position);
                break;
            case Motor_Ctrl_Type_e::Speed:
                SetTargetSpeed(0);
                break;
            case Motor_Ctrl_Type_e::Torque:
                /** ToDo */
                break;
        }
    }

    void Enable() {

    }

    void Disable() {

    }

    /** Todo: 筛查电机控制类型，不合理调用的Set需要警告 */
    void SetTargetSpeed(float targetSpeed) {
        if(params.targetType != Motor_Ctrl_Type_e::Speed) {
            return;
        }
        target = targetSpeed * params.reductionRatio; //多圈目标，减速后
    }

    void SetTargetAngle(float targetAngle) {
        if(params.targetType != Motor_Ctrl_Type_e::Position) {
            return;
        }
        target = targetAngle * params.reductionRatio; //多圈目标，减速后

        if (params.multiTurnSamePosition) {
            while (target - state.position < -180.f * params.reductionRatio){
                target += 360.f * params.reductionRatio;
            }
            while (target - state.position > 180.f * params.reductionRatio){
                target -= 360.f * params.reductionRatio;
            }
        }
    }

    Motor_State_t& GetState(){
        return state;
    }

    const float GetMultiTurnPosition() {
        return state.position / params.reductionRatio;
    }

protected:
    virtual void SetFeedback() = 0;

    float target = 0; //多圈目标，减速后
    Motor_State_t state = {0, 0, 0, 0}; //单圈状态，不考虑减速
    Motor_Param_t params;
    ImplementControllerBase<1,1>* controller = nullptr;
};

#endif
