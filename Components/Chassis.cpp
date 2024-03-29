/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "Chassis.h"
#include <cmath>

PID_Regulator_t pidRegulator3 = {//此为储存pid参数的结构体
        .kp = 0.3f,
        .ki = 0.002f,
        .kd = 0.3f,
        .componentKpMax = 2000,
        .componentKiMax = 0,
        .componentKdMax = 0,
        .outputMax = 2000
};
PID_Regulator_t pidRegulator4 = {//此为储存pid参数的结构体
        .kp = 1.0f,
        .ki = 0.0f,
        .kd = 0.0f,
        .componentKpMax = 2000,
        .componentKiMax = 0,
        .componentKdMax = 0,
        .outputMax = 2000 //4010电机输出电流上限，可以调小，勿调大
};
CHASSIS_INIT_T chassiaInit={
        .CFLMotorInit = {
                .addr = 0x141,
                .speedPID = &pidRegulator3,
                .anglePID = &pidRegulator4,
                .ctrlType = POSITION_Double,
                .reductionRatio = 1
        },
        .CBLMotorInit= {
                .addr = 0x143,
                .speedPID = &pidRegulator3,
                .anglePID = &pidRegulator4,
                .ctrlType = POSITION_Double,
                .reductionRatio = 1
        },
        .CBRMotorInit= {
                .addr = 0x142,
                .speedPID = &pidRegulator3,
                .anglePID = &pidRegulator4,
                .ctrlType = POSITION_Double,
                .reductionRatio = 1
        },
        .CFRMotorInit= {
                .addr = 0x144,
                .speedPID = &pidRegulator3,
                .anglePID = &pidRegulator4,
                .ctrlType = POSITION_Double,
                .reductionRatio = 1
        },
        .SBLMotorInit= {
                .addr = 0x01,
                .speedPID = nullptr,
                .anglePID = nullptr,
                .ctrlType = DIRECT,
                .reductionRatio = 1
        },
        .SBRMotorInit= {
                .addr = 0x02,
                .speedPID = nullptr,
                .anglePID = nullptr,
                .ctrlType = DIRECT,
                .reductionRatio = 1
        },
        .SFLMotorInit= {
                .addr = 0x03,
                .speedPID = nullptr,
                .anglePID = nullptr,
                .ctrlType = DIRECT,
                .reductionRatio = 1
        },
        .SFRMotorInit= {
                .addr = 0x04,
                .speedPID = nullptr,
                .anglePID = nullptr,
                .ctrlType = DIRECT,
                .reductionRatio = 1
        },
};

Chassis chassis(chassiaInit);
void Chassis::ChassisSetVelocity(float _fbV, float _lrV, float _rtV) {
    ChassisStopFlag = false;
    FBVelocity = _fbV;
    LRVelocity = _lrV;
    RTVelocity = _rtV;
}

void Chassis::ChassisStop() {
    ChassisStopFlag = true;

    //Classis_Motor.Stop();
    SFL.Stop();
    SFR.Stop();
    SBL.Stop();
    SBR.Stop();
    CFR.Stop();
    CFL.Stop();
    CBL.Stop();
    CBR.Stop();
}

void Chassis::WheelsSpeedCalc(float fbVelocity, float lrVelocity, float rtVelocity) {
    float ClassisSpeed[4];
    float RFLAngle, RFRAngle, RBLAngle, RBRAngle;
    float vx, vy, w;
    vx = lrVelocity;
    vy = fbVelocity;
    w = rtVelocity;
    float A, B, C, D;
    A = vx + w * LENGTH / 2;
    B = vx - w * LENGTH / 2;
    C = vy + w * WIDTH / 2;
    D = vy - w * WIDTH / 2;

    //计算四个轮子角度，单位：度
    RFRAngle = atan2(B, C) * 180 / PI;
    RFLAngle = atan2(B, D) * 180 / PI;
    RBLAngle = atan2(A, D) * 180 / PI;
    RBRAngle = atan2(A, C) * 180 / PI;

    //计算四个轮子线速度，单位：度/s

    ClassisSpeed[0] = -sqrt(B * B + C * C) / (WHEEL_DIAMETER * PI) * 360;//右前轮
    ClassisSpeed[1] = sqrt(B * B + D * D) / (WHEEL_DIAMETER * PI) * 360;//左前轮
    ClassisSpeed[2] = sqrt(A * A + D * D) / (WHEEL_DIAMETER * PI) * 360;//左后轮
    ClassisSpeed[3] = -sqrt(A * A + C * C) / (WHEEL_DIAMETER * PI) * 360;//右后轮

    float nowAngle[4] = {SFR.nowAngle - SFR.zeroAngle, SFL.nowAngle - SFL.zeroAngle,
                         SBL.nowAngle - SBL.zeroAngle, SBR.nowAngle - SBR.zeroAngle};

    /*修正角度*/
    for (int k = -3; k <= 3; k++) {
        if (abs(RFRAngle + k * 180 - nowAngle[0]) <= 90) {
            RFRAngle += (k * 180);
            if (abs(k) % 2) {
                ClassisSpeed[0] = -ClassisSpeed[0];
            }
            break;
        }
    }
    for (int k = -3; k <= 3; k++) {
        if (abs(RFLAngle + k * 180 - nowAngle[1]) <= 90) {
            RFLAngle += (k * 180);
            if (abs(k) % 2) {
                ClassisSpeed[1] = -ClassisSpeed[1];
            }
            break;
        }
    }
    for (int k = -3; k <= 3; k++) {
        if (abs(RBLAngle + k * 180 - nowAngle[2]) <= 90) {
            RBLAngle += (k * 180);
            if (abs(k) % 2) {
                ClassisSpeed[2] = -ClassisSpeed[2];
            }
            break;
        }
    }
    for (int k = -3; k <= 3; k++) {
        if (abs(RBRAngle + k * 180 - nowAngle[3]) <= 90) {
            RBRAngle += (k * 180);
            if (abs(k) % 2) {
                ClassisSpeed[3] = -ClassisSpeed[3];
            }
            break;
        }
    }

    //设置底盘电机角度
    SFL.SetTargetAngle(RFLAngle);
    SFR.SetTargetAngle(RFRAngle);
    SBL.SetTargetAngle(RBLAngle);
    SBR.SetTargetAngle(RBRAngle);

    //设置底盘电机转速
    CFR.SetTargetSpeed(ClassisSpeed[0]);
    CFL.SetTargetSpeed(ClassisSpeed[1]);
    CBL.SetTargetSpeed(ClassisSpeed[2]);
    CBR.SetTargetSpeed(ClassisSpeed[3]);

}

void Chassis::Handle() {
    WheelsSpeedCalc(FBVelocity, LRVelocity, RTVelocity);
}
