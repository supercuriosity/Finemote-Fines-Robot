/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_CHASSIS_H
#define FINEMOTE_CHASSIS_H

#include <queue>

#include "ProjectConfig.h"
#include "Matrix/matrix.h"
#include "DeviceBase.h"
#include "MotorBase.h"

#define LENGTH  0.2406f //车身长0.240225f
#define WIDTH  0.24f //车身宽
#define WHEEL_DIAMETER 0.05229//4010直径 m

class ChassisBuilder;
class VelocityProfile;

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
    SBR(*SBRPtr){

    float H1Data[2*3] = {1,0,-LENGTH/2,
                         0,1,WIDTH/2};
    H1 = Matrixf<2,3>(H1Data);
    float H2Data[2*3] = {1,0,-LENGTH/2,
                         0,1,-WIDTH/2};
    H2 = Matrixf<2,3>(H2Data);
    float H3Data[2*3] = {1,0,LENGTH/2,
                         0,1,-WIDTH/2};
    H3 = Matrixf<2,3>(H3Data);
    float H4Data[2*3] = {1,0,LENGTH/2,
                         0,1,WIDTH/2};
    H4 = Matrixf<2,3>(H4Data);
    float AData[3*3] = {4,0,0,
                        0,4,0,
                        0,0,4*(LENGTH*LENGTH/4+WIDTH*WIDTH/4)};
    Matrixf<3,3> A(AData);
    invA = matrixf::inv(A);

    float BData[2*2] = {100,0,
                        0,100};
    B = Matrixf<2,2>(BData);
    float QData[3*3] = {0.1,0,0,
                        0,0.1,0,
                        0,0,0.1};
    Q=Matrixf<3,3>(QData);
    }

public:
    void ChassisSetVelocity(float _fbV, float _lrV, float _rtV);
    void ChassisStop();
    void WheelsSpeedCalc(float fbVelocity, float lrVelocity, float rtVelocity);
    void Handle() override;
    void LSOdometry();
    void ICFOdometry();
    void CheckWheelSlip(Matrixf<2,1> velFR,Matrixf<2,1> velFL,Matrixf<2,1> velBL,Matrixf<2,1> velBR);
    void ResetOdometry(float _x,float _y,float _angle);
    bool ChassisStopFlag = true;
    float FBVelocity{}, LRVelocity{}, RTVelocity{};
    float epi = 0.2;
    float alpha = 0.5;
    float x{0},y{0},yaw{0};
    float FRX,FRY,FLX,FLY,BLX,BLY,BRX,BRY;
    float wheelError[4]{};

    Matrixf<3,3> invA;
    Matrixf<2,3> H1,H2,H3,H4;
    Matrixf<2,2> B;
    Matrixf<3,3> Q;
    Matrixf<3,3> J1=matrixf::eye<3,3>();
    Matrixf<3,3> J2=matrixf::eye<3,3>();
    Matrixf<3,3> J3=matrixf::eye<3,3>();
    Matrixf<3,3> J4=matrixf::eye<3,3>();
    Matrixf<3,1> x1=matrixf::zeros<3,1>();
    Matrixf<3,1> x2=matrixf::zeros<3,1>();
    Matrixf<3,1> x3=matrixf::zeros<3,1>();
    Matrixf<3,1> x4=matrixf::zeros<3,1>();
    Matrixf<3,1> chassisVel = matrixf::zeros<3,1>();
    Matrixf<3,1> chassisPos = matrixf::zeros<3,1>();
    Matrixf<3,1> WCSVelocity = matrixf::zeros<3,1>();

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


class VelocityProfile : public DeviceBase {
public:
    VelocityProfile(float _targetTime, float _initX, float _endX, float _initVel,
                    float _endVel): targetTime(_targetTime), targetPos(_endX) {
        float TData[4 * 4]
        {
            0, 0, 0, 1,
            _targetTime * _targetTime * _targetTime, _targetTime * _targetTime, _targetTime, 1,
            0, 0, 1, 0,
            3 * _targetTime * _targetTime, 2 * _targetTime, 1, 0
        };
        T = Matrixf<4, 4>(TData);

        float resultData[4 * 1]
        {
            _initX, _endX, _initVel, _endVel
        };
        result = Matrixf<4, 1>(resultData);

        constant = matrixf::inv(T) * result;
        initTick = HAL_GetTick();
    }

    void CalcuateVelAndPos() {
        if (currentTime > targetTime) {
            output[0][0] = targetPos;
            output[1][0] = 0.0f;
            return;
        }
        float inputData[2 * 4]
        {
            currentTime * currentTime * currentTime, currentTime * currentTime, currentTime, 1,
            3 * currentTime * currentTime, 2 * currentTime, 1, 0
        };
        input = Matrixf<2, 4>(inputData);
        output = input * constant;
    }

    Matrixf<2, 1>& GetOutput() {
        return output;
    }

    float GetCurrentTime() const {
        return currentTime;
    }

    void Handle() override {
        currentTick = HAL_GetTick();
        currentTime = (float)(currentTick - initTick) * 0.001f;
        CalcuateVelAndPos();
    }

private:
    Matrixf<4, 1> constant;
    Matrixf<4, 4> T;
    Matrixf<4, 1> result;
    Matrixf<2, 1> output = matrixf::zeros<2, 1>(); //分别为位置和速度
    Matrixf<2, 4> input = matrixf::zeros<2, 4>();
    float targetTime{10.0f};
    float currentTime{0.0f};
    float targetPos{0.0f};
    uint32_t initTick, currentTick;
};


class RoutePlanning {
    struct Target {
        float x, xVel, y, yVel, angle, angleVel, time;

        Target(const float _x, const float _xVel, const float _y, const float _yVel, const float _angle,
               const float _angleVel, const float _time): x(_x), y(_y), angle(_angle), time(_time), xVel(_xVel),
                                                          yVel(_yVel), angleVel(_angleVel) {
        }
    };

public:
    float RTVel{0}, FBVel{0}, LRVel{0};
    bool isFinished = false;

    RoutePlanning(const float _Kp): currentX(0), currentY(0), currentAngle(0), Kp(_Kp), target(0, 0, 0, 0, 0, 0, 0) {
    }

    void AddTarget(float x, float xVel, float y, float yVel, float angle, float angleVel, float time) {
        targetList.emplace(x, xVel, y, yVel, angle, angleVel, time);
    }

    void Update(const float x, const float xVel, const float y, const float yVel, const float angle,
                const float angleVel) {
        currentX = x, currentY = y, currentAngle = angle;
        currentXVel = xVel, currentYVel = yVel, currentAngleVel = angleVel;
    }

    void GetPlannedVel(float& _x,float& _y,float& _angle,float& _xVel,float& _yVel,float& _rtVel) {
        if(XVelProfilePtr && YVelProfilePtr && RTVelProfilePtr) {
            _x = XVelProfilePtr->GetOutput()[0][0];
            _y = YVelProfilePtr->GetOutput()[0][0];
            _angle = RTVelProfilePtr->GetOutput()[0][0];
            _xVel = LRVel;
            _yVel = FBVel;
            _rtVel = RTVel;
        }
    }


    void CalcSpeed() {
        if (isArrived) {
            if (targetList.empty()) {
                isFinished = true;
                FBVel = 0, LRVel = 0, RTVel = 0;
                return;
            }
            delete XVelProfilePtr;
            delete YVelProfilePtr;
            delete RTVelProfilePtr;

            target = targetList.front();
            targetList.pop();

            XVelProfilePtr = new VelocityProfile(target.time, currentX, target.x, currentXVel, target.xVel);
            YVelProfilePtr = new VelocityProfile(target.time, currentY, target.y, currentYVel, target.yVel);
            RTVelProfilePtr = new VelocityProfile(target.time, currentAngle, target.angle, currentAngleVel,target.angleVel);

            isArrived = false;
        }
        float wcs_vel_x = XVelProfilePtr->GetOutput()[1][0] + (XVelProfilePtr->GetOutput()[0][0] - currentX) * Kp;
        float wcs_vel_y = YVelProfilePtr->GetOutput()[1][0] + (YVelProfilePtr->GetOutput()[0][0] - currentY) * Kp;
        float wcs_vel_rt = RTVelProfilePtr->GetOutput()[1][0] + (RTVelProfilePtr->GetOutput()[0][0] - currentAngle) *Kp;

        FBVel = cosf(currentAngle) * wcs_vel_y - sinf(currentAngle) * wcs_vel_x;
        LRVel = sinf(currentAngle) * wcs_vel_y + cosf(currentAngle) * wcs_vel_x;
        RTVel = wcs_vel_rt;

        if(targetList.empty()) {
            isArrived = fabsf(target.angle - currentAngle) < 0.003 && fabsf(target.x - currentX) < 0.001 && fabsf(target.y - currentY) < 0.001;
            return;
        }
        isArrived = fabsf(target.angle - currentAngle) < 0.06 && fabsf(target.x - currentX) < 0.03 && fabsf(target.y - currentY) < 0.03;
    }

private:
    std::queue<Target> targetList;
    float currentX, currentY, currentAngle, currentXVel, currentYVel, currentAngleVel;
    float Kp;
    Target target;
    bool isArrived = true;
    VelocityProfile* XVelProfilePtr = nullptr;
    VelocityProfile* YVelProfilePtr = nullptr;
    VelocityProfile* RTVelProfilePtr = nullptr;
};

#endif //FINEMOTE_CHASSIS_H
