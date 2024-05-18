#ifndef FINEMOTE_MOTORBASE_H
#define FINEMOTE_MOTORBASE_H


#include "DeviceBase.h"
#include "Bus/CAN_Base.h"
#include "Control/PID.h"
#include "Communication/Verify.h"


typedef enum {
    Internal = 0,
    External_Speed,
    External_Position,
} MOTOR_CTRL_TYPE_e;

typedef struct {
    float speed;//最终电机输出轴的转速，单位为RPM
    float angle;//输出轴的角度，单位为度
    float moment;//转矩电流的相对值，具体值参考电调手册
    float temperature;//电机温度，单位摄氏度
} MOTOR_STATE_t;

typedef struct {
    /** 内容根据下方批注调整 */
    uint32_t addr;
    PID_Regulator_t *speedPID;//速度环pid参数结构体指针
    PID_Regulator_t *anglePID;//角度环pid参数结构体指针
    MOTOR_CTRL_TYPE_e ctrlType;//控制电机的方式
    float reductionRatio;//减速比
} MOTOR_INIT_t;

//TODO 电机基类的抽象程度不够，派生类冗余内容太多
class MotorBase : public DeviceBase {
public:
    MotorBase(){
        this ->SetDivisionFactor(4);
    }

    /** 基类当中这个东西的用途是什么，stop和disable的区别是什么 */
    void Stop() {
        stopflag = true;
    };

    /** Todo: 筛查电机控制类型，不合理调用的Set需要警告 */
    void SetTargetSpeed(float _targetSpeed) {
        targetSpeed = _targetSpeed;
    };

    void SetTargetAngle(float _targetAngle) {
        targetAngle = _targetAngle;
    };

    float reductionRatio{1};

    /** 算法用通用接口，函数指针找控制算法 */
    PID speedPID, anglePID;
    /** 用来干啥，查清楚 */
    bool stopflag{};

    float targetSpeed;
    float targetAngle;

    int16_t intensity;
    uint16_t txSpeed = 0;
    int32_t txAngle = 0;

    float realAngle;
    float lastAngle;
    float zeroAngle;

    MOTOR_CTRL_TYPE_e ctrlType;
    MOTOR_STATE_t state;

};


#endif //FINEMOTE_MOTORBASE_H
