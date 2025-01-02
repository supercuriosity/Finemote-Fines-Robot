#ifndef FINEMOTE_MOTORBASE_H
#define FINEMOTE_MOTORBASE_H

#include "DeviceBase.h"
#include "Control/PID.h"

enum class Motor_Ctrl_Type_e: uint16_t {
    Position = 0,
    Speed,
    Torque,
};

typedef struct {
    float position;//单位为度
    float speed;//单位为DPS
    float torque;//转矩电流的相对值，具体值参考电调手册
    float temperature;//电机温度，单位摄氏度
} Motor_State_t;

typedef struct {
    Motor_Ctrl_Type_e ctrlType;//控制电机的方式
    Motor_Ctrl_Type_e targetType;//控制电机哪个状态
    float reductionRatio = 1;//减速比
} Motor_Param_t;

class MotorBase : public DeviceBase {
public:
    explicit MotorBase(const Motor_Param_t&& params) : params(params) {

    }

    template<typename T>
    void ResetController(T& _controller) {
        static_assert(!std::is_same<ControllerBase, T>::value, "ControllerBase itself is not allowed as a controller");

        controller = &_controller;
        _controller.SetTarget(&target);
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
        target = targetSpeed;
    }

    void SetTargetAngle(float targetAngle) {
        if(params.targetType != Motor_Ctrl_Type_e::Position) {
            return;
        }
        target = targetAngle;
    }

    Motor_State_t& GetState(){
        return state;
    }

protected:
    virtual void SetFeedback() = 0;

    float target = 0;
    Motor_State_t state = {0, 0, 0, 0};
    Motor_Param_t params;
    ControllerBase* controller = nullptr;
};

#endif //FINEMOTE_MOTORBASE_H
