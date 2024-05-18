/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

//
// Created by 25396 on 2024/2/19.
//

#ifndef FINEMOTE_MOTOR4010_H
#define FINEMOTE_MOTOR4010_H

#include "ProjectConfig.h"


#ifdef MOTOR_COMPONENTS

#include "DeviceBase.h"
#include "Bus/CAN_Base.h"
#include "Motors/MotorBase.h"


template<int busID>
class Motor4010 : public MotorBase {
public:
    //TODO 这里不觉得很奇怪吗，两个不同电机的初始化结构体是相同的，显然是可以归并到基类的行为，具体通讯方式可以由派生类管理
    //右值引用的重载，虽然不涉及资源的接管，但是为方便构建对象
    explicit Motor4010(MOTOR_INIT_t &&motorInit) : canAgent(motorInit.addr) {

        if (motorInit.speedPID) speedPID.PIDInfo = *motorInit.speedPID;
        if (motorInit.anglePID) anglePID.PIDInfo = *motorInit.anglePID;
        reductionRatio = motorInit.reductionRatio;
        ctrlType = motorInit.ctrlType;
    }

    explicit Motor4010(MOTOR_INIT_t &motorInit) : canAgent(motorInit.addr) {

        if (motorInit.speedPID) speedPID.PIDInfo = *motorInit.speedPID;
        if (motorInit.anglePID) anglePID.PIDInfo = *motorInit.anglePID;
        reductionRatio = motorInit.reductionRatio;
        ctrlType = motorInit.ctrlType;
    }

    void Handle() override {

        MotorStateUpdate();
        IntensityCalc();
        CANMessageGet();

    };

    CAN_Agent<busID> canAgent;
private:
    void CANMessageGet() {
        // txSpeed = (uint16_t)targetSpeed;
        txSpeed = 0x300;
        txAngle = (uint32_t) (targetAngle * 100.0f);

        switch (ctrlType) {
            case External_Speed :
            case External_Position: {
                canAgent.DLC = 8;
                canAgent.txbuf[0] = 0xA1;
                canAgent.txbuf[1] = 0x00;
                canAgent.txbuf[2] = 0x00;
                canAgent.txbuf[3] = 0x00;
                canAgent.txbuf[4] = intensity;
                canAgent.txbuf[5] = intensity >> 8;
                canAgent.txbuf[6] = 0x00;
                canAgent.txbuf[7] = 0x00;
                break;
            }
            case Internal: {
                canAgent.DLC = 8;
                canAgent.txbuf[0] = 0xA4;
                canAgent.txbuf[1] = 0x00;
                canAgent.txbuf[2] = txSpeed;
                canAgent.txbuf[3] = txSpeed >> 8;
                canAgent.txbuf[4] = txAngle;
                canAgent.txbuf[5] = txAngle >> 8;
                canAgent.txbuf[6] = txAngle >> 16;
                canAgent.txbuf[7] = txAngle >> 24;
                break;
            }
        }
        canAgent.Send();
    }

    void MotorStateUpdate() {
        state.angle = (canAgent.rxbuf[6] | (canAgent.rxbuf[7] << 8u)) * 360.0f / 16384.0f;
        state.speed = (canAgent.rxbuf[4] | (canAgent.rxbuf[5] << 8u)) / reductionRatio;
        state.moment = canAgent.rxbuf[2] | (canAgent.rxbuf[3] << 8u);
        state.temperature = canAgent.rxbuf[1];

        switch (ctrlType) {
            case External_Speed: {
                break;
            }
            case External_Position: {

                if (state.angle <= lastAngle) {
                    if (lastAngle - state.angle > 8000)
                        realAngle += (state.angle + 16384.0f - lastAngle) * 360.0f / 16384.0f / reductionRatio;
                    else
                        realAngle -= (lastAngle - state.angle) * 360.0f / 16384.0f / reductionRatio;
                } else {
                    if (state.angle - lastAngle > 8000)
                        realAngle -= (lastAngle + 16384.0f - state.angle) * 360.0f / 16384.0f / reductionRatio;
                    else
                        realAngle += (state.angle - lastAngle) * 360.0f / 16384.0f / reductionRatio;
                }
                state.angle = realAngle;
                lastAngle = state.angle;
                break;
            }
            case Internal:
                break;
        }
    }

    void IntensityCalc() {
        switch (ctrlType) {
            case Internal:
                intensity = (int16_t) targetAngle;
                break;

            case External_Speed:
                intensity = (int16_t) speedPID.PIDCalc(targetSpeed, state.speed);
                break;

            case External_Position:
                float _targetSpeed = anglePID.PIDCalc(targetAngle, state.angle);
                intensity = (int16_t) speedPID.PIDCalc(_targetSpeed, state.speed);
                break;
        }
    }


};

#endif
#endif //FINEMOTE_MOTOR4010_H
