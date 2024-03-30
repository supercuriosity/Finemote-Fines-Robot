/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "Chassis.h"
#include <cmath>

PID_Regulator_t adminSpeedPID = {//此为储存pid参数的结构体
        .kp = 0.3f,
        .ki = 0.002f,
        .kd = 0.3f,
        .componentKpMax = 2000,
        .componentKiMax = 0,
        .componentKdMax = 0,
        .outputMax = 2000
};
PID_Regulator_t adminAnglePID = {//此为储存pid参数的结构体
        .kp = 1.0f,
        .ki = 0.0f,
        .kd = 0.0f,
        .componentKpMax = 2000,
        .componentKiMax = 0,
        .componentKdMax = 0,
        .outputMax = 2000 //4010电机输出电流上限，可以调小，勿调大
};
Motor4010<1> CFLMotor(MOTOR_INIT_t{0x141,&adminSpeedPID,&adminAnglePID,POSITION_Double,1});
Motor4010<1> CFRMotor(MOTOR_INIT_t{0x144,&adminSpeedPID,&adminAnglePID,POSITION_Double,1});
Motor4010<1> CBLMotor(MOTOR_INIT_t{0x143,&adminSpeedPID,&adminAnglePID,POSITION_Double,1});
Motor4010<1> CBRMotor(MOTOR_INIT_t{0x142,&adminSpeedPID,&adminAnglePID,POSITION_Double,1});

Motor4315<1> SFLMotor(MOTOR_INIT_t{0x03,nullptr,nullptr,DIRECT,1});
Motor4315<1> SFRMotor(MOTOR_INIT_t{0x04,nullptr,nullptr,DIRECT,1});
Motor4315<1> SBLMotor(MOTOR_INIT_t{0x01,nullptr,nullptr,DIRECT,1});
Motor4315<1> SBRMotor(MOTOR_INIT_t{0x02,nullptr,nullptr,DIRECT,1});


//建造者模式按步骤构造的底盘对象
Chassis& chassis = *Chassis::Build().
        AddCFLMotor(CFLMotor).
        AddCFRMotor(CFRMotor).
        AddCBLMotor(CBLMotor).
        AddCBRMotor(CBRMotor).
        AddSFLMotor(SFLMotor).
        AddSFRMotor(SFRMotor).
        AddSBLMotor(SBLMotor).
        AddSBRMotor(SBRMotor).Build();

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

ChassisBuilder Chassis::Build() {
    return {};
}
