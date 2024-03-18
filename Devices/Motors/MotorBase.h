#ifndef FINEMOTE_MOTORBASE_H
#define FINEMOTE_MOTORBASE_H


#include "DeviceBase.h"
#include "Bus/CAN_Base.h"
#include "Control/PID.h"

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

typedef struct {
    uint32_t addr;
    PID_Regulator_t *speedPID;//速度环pid参数结构体指针
    PID_Regulator_t *anglePID;//角度环pid参数结构体指针
    MOTOR_CTRL_TYPE_e ctrlType;//控制电机的方式
    float reductionRatio;//减速比
} MOTOR_INIT_t;

class MotorBase : public DeviceBase {
public:
    MotorBase() = default;

    void Stop() {
        stopflag = true;
    };

    void SetTargetSpeed(float _targetSpeed) {
        targetSpeed = _targetSpeed;
    };

    void SetTargetAngle(float _targetAngle) {
        targetAngle = _targetAngle;
    };

    static uint16_t CRC16Calc(uint8_t *data, uint16_t length){
        uint16_t crc = 0xffff;        // Initial value
        while (length--) {
            crc ^= *data++;            // crc ^= *data; data++;
            for (uint8_t i = 0; i < 8; ++i) {
                if (crc & 1)
                    crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005
                else
                    crc = (crc >> 1);
            }
        }
        return crc;
    }

    uint32_t addr;
    float reductionRatio{};
    PID speedPID, anglePID;
    bool stopflag{};
    float targetSpeed{};
    float targetAngle{};
    MOTOR_CTRL_TYPE_e ctrlType{};
    MOTOR_FEEDBACK_t feedback{};
    MOTOR_STATE_t state{};
};


#endif //FINEMOTE_MOTORBASE_H
