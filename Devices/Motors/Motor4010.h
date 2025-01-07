/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_MOTOR4010_H
#define FINEMOTE_MOTOR4010_H

#include "ProjectConfig.h"

#ifdef MOTOR_COMPONENTS

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
        switch (params.ctrlType) {
            case Motor_Ctrl_Type_e::Torque: {
                volatile int16_t txTorque = Clamp(1 * controller->GetOutput(), -2000.f, 2000.f);

                canAgent[0] = 0xA1;
                canAgent[1] = 0x00;
                canAgent[2] = 0x00;
                canAgent[3] = 0x00;
                canAgent[4] = txTorque;
                canAgent[5] = txTorque >> 8;
                canAgent[6] = 0x00;
                canAgent[7] = 0x00;
                break;
            }
            case Motor_Ctrl_Type_e::Position: {
                constexpr uint16_t txSpeed = 0x800;
                int32_t txAngle = 100 * controller->GetOutput() * -1; //统一正方向

                canAgent[0] = 0xA4;
                canAgent[1] = 0x00;
                canAgent[2] = txSpeed;
                canAgent[3] = txSpeed >> 8;
                canAgent[4] = txAngle;
                canAgent[5] = txAngle >> 8;
                canAgent[6] = txAngle >> 16;
                canAgent[7] = txAngle >> 24;
                break;
            }
        }
        canAgent.Send(canAgent.addr);
    }

    void Update() {
        state.position = (canAgent.rxbuf[6] | (canAgent.rxbuf[7] << 8u)) * 360.0f / 16384.0f;
        state.speed = (int16_t)(canAgent.rxbuf[4] | (canAgent.rxbuf[5] << 8u));
        state.torque = (int16_t)(canAgent.rxbuf[2] | (canAgent.rxbuf[3] << 8u));
        state.temperature = canAgent.rxbuf[1];
    }
};

#endif //MOTOR_COMPONENTS
#endif //FINEMOTE_MOTOR4010_H
