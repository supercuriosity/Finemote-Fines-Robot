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

typedef struct {


}CHASSIS_INIT_T;
class ChassisBuilder;

class Chassis : public DeviceBase {
    friend class ChassisBuilder;
    Chassis(
    Motor4010<1>* CFLPtr,
    Motor4010<1>* CFRPtr,
    Motor4010<1>* CBLPtr,
    Motor4010<1>* CBRPtr,
    Motor4315<1>* SFLPtr,
    Motor4315<1>* SFRPtr,
    Motor4315<1>* SBLPtr,
    Motor4315<1>* SBRPtr):
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

    Motor4010<1> &CFL,&CFR,&CBL,&CBR;
    Motor4315<1> &SFL,&SFR,&SBL,&SBR;

    static ChassisBuilder Build();
};
//TODO 实际上建造者应该只使用电机基类指针，底盘类使用的应该也是电机基类指针
class ChassisBuilder{
private:
    Motor4010<1>* CFLPtr{nullptr};
    Motor4010<1>* CFRPtr{nullptr};
    Motor4010<1>* CBLPtr{nullptr};
    Motor4010<1>* CBRPtr{nullptr};
    Motor4315<1>* SFLPtr{nullptr};
    Motor4315<1>* SFRPtr{nullptr};
    Motor4315<1>* SBLPtr{nullptr};
    Motor4315<1>* SBRPtr{nullptr};
public:
    ChassisBuilder& AddCFLMotor(Motor4010<1> &cflMotor){CFLPtr = &cflMotor;return *this;};
    ChassisBuilder& AddCFRMotor(Motor4010<1> &cfrMotor){CFRPtr = &cfrMotor;return *this;};
    ChassisBuilder& AddCBLMotor(Motor4010<1> &cblMotor){CBLPtr = &cblMotor;return *this;};
    ChassisBuilder& AddCBRMotor(Motor4010<1> &cbrMotor){CBRPtr = &cbrMotor;return *this;};
    ChassisBuilder& AddSFLMotor(Motor4315<1> &sflMotor){SFLPtr = &sflMotor;return *this;};
    ChassisBuilder& AddSFRMotor(Motor4315<1> &sfrMotor){SFRPtr = &sfrMotor;return *this;};
    ChassisBuilder& AddSBLMotor(Motor4315<1> &sblMotor){SBLPtr = &sblMotor;return *this;};
    ChassisBuilder& AddSBRMotor(Motor4315<1> &sbrMotor){SBRPtr = &sbrMotor;return *this;};

    Chassis* Build(){
        if (CFLPtr && CFRPtr && CBLPtr && CBRPtr && SFLPtr && SFRPtr && SBLPtr && SBRPtr){
            return new Chassis{CFLPtr,CFRPtr,CBLPtr,CBRPtr,SFLPtr,SFRPtr,SBLPtr,SBRPtr};
        }else{
            return nullptr;
        }
    }
};



#endif //FINEMOTE_CHASSIS_H
