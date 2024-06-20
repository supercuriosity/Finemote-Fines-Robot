/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_MOTOR4010_H
#define FINEMOTE_MOTOR4010_H

#include "ProjectConfig.h"

#ifdef MOTOR_COMPONENTS

#include "DeviceBase.h"
#include "Bus/CAN_Base.h"
#include "Motors/MotorBase.h"

/**
 * Todo: Reduction ratio
 */
template<int busID>
class Motor4010 : public MotorBase {
public:
    template<typename T>
    Motor4010(const Motor_Param_t&& params, T& _controller, uint32_t addr) : MotorBase(std::forward<const Motor_Param_t>(params)), canAgent(addr) {
        ResetController(_controller);
    }

    void Handle() final{
        Update();
        controller->Calc();
        MessageGenerate();
    };

    CAN_Agent<busID> canAgent;

private:
    void SetFeedback() final{
        switch (params.targetType) {
            case Motor_Ctrl_Type_e::Position:
                controller->SetFeedback({&state.position, &state.speed});
                break;
            case Motor_Ctrl_Type_e::Speed:
                controller->SetFeedback({&state.speed});
                break;
        }
    }

    void MessageGenerate() {
        switch (params.targetType) {
            case Motor_Ctrl_Type_e::Torque: {
                uint16_t txTorque = controller->GetOutput();

                canAgent.DLC = 8;
                canAgent.txbuf[0] = 0xA1;
                canAgent.txbuf[1] = 0x00;
                canAgent.txbuf[2] = 0x00;
                canAgent.txbuf[3] = 0x00;
                canAgent.txbuf[4] = txTorque;
                canAgent.txbuf[5] = txTorque >> 8;
                canAgent.txbuf[6] = 0x00;
                canAgent.txbuf[7] = 0x00;
                break;
            }
            case Motor_Ctrl_Type_e::Position: {
                constexpr uint16_t txSpeed = 0x300;
                uint32_t txAngle = 100 * controller->GetOutput();

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

    void Update() {
        state.position = (canAgent.rxbuf[6] | (canAgent.rxbuf[7] << 8u)) * 360.0f / 16384.0f;
        state.speed = canAgent.rxbuf[4] | (canAgent.rxbuf[5] << 8u);
        state.torque = canAgent.rxbuf[2] | (canAgent.rxbuf[3] << 8u);
        state.temperature = canAgent.rxbuf[1];
    }
};

#endif //MOTOR_COMPONENTS
#endif //FINEMOTE_MOTOR4010_H
