/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_CHASSIS_H
#define FINEMOTE_CHASSIS_H

#include "ProjectConfig.h"

#include "DeviceBase.h"
#include "Motors/Motor4010.h"
#include "Motors/Motor4315.h"
#define LENGTH  0.24f //车身长
#define WIDTH  0.24f //车身宽
#define WHEEL_DIAMETER 0.0528f//4010直径 m
#define PI 3.1415926f

class ChassisBuilder;

class Chassis : public DeviceBase {
    friend class ChassisBuilder;
    Chassis(
    MotorBase* CFLPtr,
    MotorBase* CFRPtr,
    MotorBase* CBLPtr,
    MotorBase* CBRPtr,
    MotorBase* SFLPtr,
    MotorBase* SFRPtr,
    MotorBase* SBLPtr,
    MotorBase* SBRPtr):
    CFL(*CFLPtr),
    CFR(*CFRPtr),
    CBL(*CBLPtr),
    CBR(*CBRPtr),
    SFL(*SFLPtr),
    SFR(*SFRPtr),
    SBL(*SBLPtr),
    SBR(*SBRPtr){}

public:
    void ChassisSetVelocity(float _fbV, float _lrV, float _rtV);
    void ChassisStop();
    void WheelsSpeedCalc(float fbVelocity, float lrVelocity, float rtVelocity);
    void Handle() override;
    bool ChassisStopFlag = true;
    float FBVelocity{}, LRVelocity{}, RTVelocity{};

    MotorBase &CFL,&CFR,&CBL,&CBR;
    MotorBase &SFL,&SFR,&SBL,&SBR;

    static ChassisBuilder Build();
};
//TODO 实际上建造者应该只使用电机基类指针，底盘类使用的应该也是电机基类指针
class ChassisBuilder{
private:
    MotorBase* CFLPtr{nullptr};
    MotorBase* CFRPtr{nullptr};
    MotorBase* CBLPtr{nullptr};
    MotorBase* CBRPtr{nullptr};
    MotorBase* SFLPtr{nullptr};
    MotorBase* SFRPtr{nullptr};
    MotorBase* SBLPtr{nullptr};
    MotorBase* SBRPtr{nullptr};
public:
    ChassisBuilder& AddCFLMotor(MotorBase& cflMotor){CFLPtr = &cflMotor;return *this;};
    ChassisBuilder& AddCFRMotor(MotorBase& cfrMotor){CFRPtr = &cfrMotor;return *this;};
    ChassisBuilder& AddCBLMotor(MotorBase& cblMotor){CBLPtr = &cblMotor;return *this;};
    ChassisBuilder& AddCBRMotor(MotorBase& cbrMotor){CBRPtr = &cbrMotor;return *this;};
    ChassisBuilder& AddSFLMotor(MotorBase& sflMotor){SFLPtr = &sflMotor;return *this;};
    ChassisBuilder& AddSFRMotor(MotorBase& sfrMotor){SFRPtr = &sfrMotor;return *this;};
    ChassisBuilder& AddSBLMotor(MotorBase& sblMotor){SBLPtr = &sblMotor;return *this;};
    ChassisBuilder& AddSBRMotor(MotorBase& sbrMotor){SBRPtr = &sbrMotor;return *this;};

    Chassis Build(){
        if (!(CFLPtr && CFRPtr && CBLPtr && CBRPtr && SFLPtr && SFRPtr && SBLPtr && SBRPtr)){
            Error_Handler();
        }
        return Chassis{CFLPtr,CFRPtr,CBLPtr,CBRPtr,SFLPtr,SFRPtr,SBLPtr,SBRPtr};
        
    }
};



#endif //FINEMOTE_CHASSIS_H
