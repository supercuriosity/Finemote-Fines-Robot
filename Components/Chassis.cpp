/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "Chassis.h"
#include <cmath>

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
    RFRAngle = atan2(A, D) * 180 / PI;
    RFLAngle = atan2(A, C) * 180 / PI;
    RBLAngle = atan2(B, C) * 180 / PI;
    RBRAngle = atan2(B, D) * 180 / PI;

    //计算四个轮子线速度，单位：度/s
    ClassisSpeed[0] = -sqrt(A * A + D * D)  / (WHEEL_DIAMETER * PI) * 360;//右前轮1
    ClassisSpeed[1] = sqrt(A * A + C * C)/ (WHEEL_DIAMETER * PI) * 360;//左前轮2
    ClassisSpeed[2] = sqrt(B * B + C * C) / (WHEEL_DIAMETER * PI) * 360;//左后轮3
    ClassisSpeed[3] = -sqrt(B * B + D * D) / (WHEEL_DIAMETER * PI) * 360;//右后轮4

/*    float nowAngle[4] = {SFR.state.position - SFR.zeroAngle, SFL.state.position - SFL.zeroAngle,
                         SBL.state.position - SBL.zeroAngle, SBR.state.position - SBR.zeroAngle};*/

    //设置底盘电机角度
    SFR.SetTargetAngle(RFRAngle);
    SFL.SetTargetAngle(RFLAngle);
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
