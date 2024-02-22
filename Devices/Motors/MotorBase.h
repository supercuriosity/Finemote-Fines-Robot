#ifndef FINEMOTE_MOTORBASE_H
#define FINEMOTE_MOTORBASE_H


#include "DeviceBase.h"
#include "Bus/CAN_Base.h"
typedef enum {
    DIRECT = 0,
    SPEED_Single,
    POSITION_Double
} MOTOR_CTRL_TYPE_e;

typedef struct {
    uint16_t angle;
    int16_t speed;
    int16_t moment;
    int8_t temp;
} MOTOR_FEEDBACK_t;

typedef struct {
    float speed;//最终电机输出轴的转速，单位为RPM
    float angle;//输出轴的角度，单位为度
    float moment;//转矩电流的相对值，具体值参考电调手册
    float temperature;//电机温度，单位摄氏度
} MOTOR_STATE_t;

class MotorBase: public DeviceBase{
public:
    MotorBase()= default;

    void Stop(){
        stopflag = true;
    };
    void SetTargetSpeed(float _targetSpeed){
        targetSpeed = _targetSpeed;
    };
    void SetTargetAngle(float _targetAngle){
        targetAngle = _targetAngle;
    };


    float reductionRatio{};
    //PID speedPID,anglePID;
    bool stopflag{};
    float targetSpeed{};
    float targetAngle{};
    MOTOR_CTRL_TYPE_e ctrlType{};
    MOTOR_FEEDBACK_t feedback{};
    MOTOR_STATE_t state{};
};



#endif //FINEMOTE_MOTORBASE_H
