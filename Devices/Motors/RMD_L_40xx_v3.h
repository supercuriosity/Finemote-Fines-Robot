/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_RMD_L_40xx_V3_H
#define FINEMOTE_RMD_L_40xx_V3_H

#include "ProjectConfig.h"

#ifdef MOTOR_COMPONENTS

#include "Bus/CAN_Base.h"
#include "Motors/MotorBase.h"

/**
 * Todo: Reduction ratio
 */
template<int busID>
class RMD_L_40xx_v3 : public MotorBase {
public:

    template<typename T>
    RMD_L_40xx_v3(const Motor_Param_t&& params, T& _controller, uint32_t addr) : MotorBase(std::forward<const Motor_Param_t>(params)), canAgent(addr) {
        SetDivisionFactor(2);
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
                int16_t txTorque = Clamp(-controller->GetOutput(), -500.f, 500.f);

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
                float targetAngle = -controller->GetOutput();

                int32_t txAngle = 100 * targetAngle * -1;//统一正方向
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
        canAgent.Send(canAgent.addr - 0x100);
    }

    void Update() {
        state.position = -(int16_t)(canAgent.rxbuf[6] | (canAgent.rxbuf[7] << 8u)) / 65536.0f * 360.0f;
        state.speed = -(int16_t)(canAgent.rxbuf[4] | (canAgent.rxbuf[5] << 8u));
        state.torque = -(int16_t)(canAgent.rxbuf[2] | (canAgent.rxbuf[3] << 8u));
        state.temperature = (int8_t)(canAgent.rxbuf[1]);
    }
};

#endif //MOTOR_COMPONENTS
#endif //FINEMOTE_RMD_L_40xx_V3_H
