/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_CHASSIS_H
#define FINEMOTE_CHASSIS_H

#include "ProjectConfig.h"

#include "DeviceBase.h"
#include "Motor4010.h"
#include "Motor4315.h"
#define LENGTH  0.24f //车身长
#define WIDTH  0.24f //车身宽
#define WHEEL_DIAMETER 0.0528f//4010直径 m
#define PI 3.1415926f

typedef struct {
    MOTOR_INIT_t CFLMotorInit;
    MOTOR_INIT_t CBLMotorInit;
    MOTOR_INIT_t CFRMotorInit;
    MOTOR_INIT_t CBRMotorInit;
    MOTOR_INIT_t SFLMotorInit;
    MOTOR_INIT_t SBLMotorInit;
    MOTOR_INIT_t SFRMotorInit;
    MOTOR_INIT_t SBRMotorInit;

}CHASSIS_INIT_T;
class Chassis : public DeviceBase {
public:
    explicit Chassis(CHASSIS_INIT_T chassisInitT): CFL(chassisInitT.CFLMotorInit), CBL(chassisInitT.CBLMotorInit ), CBR(chassisInitT.CBRMotorInit),
                                                   CFR(chassisInitT.CFRMotorInit), SBL(chassisInitT.SBLMotorInit), SBR(chassisInitT.SBRMotorInit),
                                                   SFL(chassisInitT.SFLMotorInit), SFR(chassisInitT.SFRMotorInit){}
    void ChassisSetVelocity(float _fbV, float _lrV, float _rtV);
    void ChassisStop();
    void WheelsSpeedCalc(float fbVelocity, float lrVelocity, float rtVelocity);

    void Handle() override;
    bool ChassisStopFlag = true;
    float FBVelocity{}, LRVelocity{}, RTVelocity{};

    Motor4010<1> CFL,CFR,CBL,CBR;
    Motor4315<1> SFL,SFR,SBL,SBR;

};
extern Chassis chassis;


#endif //FINEMOTE_CHASSIS_H
