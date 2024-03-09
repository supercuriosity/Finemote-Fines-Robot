/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

//
// Created by 25396 on 2024/2/19.
//

#ifndef FINEMOTE_MOTORTEST_H
#define FINEMOTE_MOTORTEST_H
#include "ProjectConfig.h"


#ifdef MOTOR_COMPONENTS
#include "DeviceBase.h"
#include "Bus/CAN_Base.h"
#include "Motors/MotorBase.h"


template<int busID>
class MotorTest : public MotorBase {
public:
    explicit MotorTest(MOTOR_INIT_t &motorInit) :canAgent(motorInit.addr, CAN_Bus<busID>::GetInstance()) {
        HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
        speedPID.PIDInfo = *motorInit.speedPID;
        anglePID.PIDInfo = *motorInit.anglePID;
        reductionRatio = motorInit.reductionRatio;
        ctrlType = motorInit.ctrlType;
    }

    void Handle() override {

        MotorStateUpdate();
        IntensityCalc();
        CANMessageGet();

    };

    CAN_Agent <busID> canAgent;
private:
    void CANMessageGet(){
        // txSpeed = (uint16_t)targetSpeed;
        txSpeed = 0x300;
        txAngle = (uint32_t) (targetAngle * 100.0f);

        switch (ctrlType) {
            case SPEED_Single :
            case POSITION_Double:{
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
            case DIRECT: {
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
        static int num = 0;
        num++;
        if (num >= 4) {
            canAgent.Send();
            num = 0;
        }
    }
    void MotorStateUpdate() {
        feedback.angle = canAgent.rxbuf[6] | (canAgent.rxbuf[7] << 8u);
        feedback.speed = canAgent.rxbuf[4] | (canAgent.rxbuf[5] << 8u);
        feedback.moment = canAgent.rxbuf[2] | (canAgent.rxbuf[3] << 8u);
        feedback.temp = canAgent.rxbuf[1];

        switch (ctrlType) {
            case SPEED_Single: {
                state.speed = (float) (feedback.speed) / reductionRatio;
                //state.speed /= (360/PI/0.048f);
                break;
            }
            case POSITION_Double: {
                state.speed = (float) (feedback.speed) / reductionRatio;
                state.moment = feedback.moment;
                state.temperature = feedback.temp;
                state.angle = (float) (feedback.angle) * 360.0f / 16384.0f;

                thisAngle = feedback.angle;
                if (thisAngle <= lastRead) {
                    if (lastRead - thisAngle > 8000)
                        realAngle += (thisAngle + 16384.0f - lastRead) * 360.0f / 16384.0f / reductionRatio;
                    else
                        realAngle -= (lastRead - thisAngle) * 360.0f / 16384.0f / reductionRatio;
                } else {
                    if (thisAngle - lastRead > 8000)
                        realAngle -= (lastRead + 16384.0f - thisAngle) * 360.0f / 16384.0f / reductionRatio;
                    else
                        realAngle += (thisAngle - lastRead) * 360.0f / 16384.0f / reductionRatio;
                }
                state.angle = realAngle;
                lastRead = feedback.angle;
                break;
            }
            case DIRECT:
                break;
        }
    }

    void IntensityCalc() {
        switch (ctrlType) {
            case DIRECT:
                intensity = (int16_t) targetAngle;
                break;

            case SPEED_Single:
                intensity = (int16_t) speedPID.PIDCalc(targetSpeed, state.speed);
                break;

            case POSITION_Double:
                float _targetSpeed = anglePID.PIDCalc(targetAngle, state.angle);
                intensity = (int16_t) speedPID.PIDCalc(_targetSpeed, state.speed);
                break;
        }
    }

    uint16_t txSpeed = 0;
    uint32_t txAngle = 0;

    int16_t intensity{0};
    float realAngle{0};
    float thisAngle{};
    float lastRead{};

};

#endif
#endif //FINEMOTE_MOTORTEST_H
