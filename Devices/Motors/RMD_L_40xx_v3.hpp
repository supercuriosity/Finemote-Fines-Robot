/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_RMD_L_40xx_V3_H
#define FINEMOTE_RMD_L_40xx_V3_H

#include "Motors/MotorBase.hpp"
#include "Bus/CAN_Base.hpp"
#include "Control/Clamp.hpp"

/**
 * Todo: Reduction ratio
 */
template <int busID>
class RMD_L_40xx_v3 : public MotorBase {
public:
    template <typename T>
    RMD_L_40xx_v3(const Motor_Param_t&& params, T& _controller, uint32_t addr)
            : MotorBase(std::forward<const Motor_Param_t>(params)), canAgent(addr) {
        SetDivisionFactor(3);
        ResetController(_controller);
    }

    void Handle() final {
        Update();
        controller->Calc();
        MessageGenerate();
    }

    CAN_Agent<busID> canAgent;

private:
    void SetFeedback() final {
        switch (params.targetType) {
            case Motor_Ctrl_Type_e::Position:
                controller->SetFeedbacks(&state.position);
                break;
            case Motor_Ctrl_Type_e::Speed:
                controller->SetFeedbacks(&state.speed);
                break;
        }
    }

    void MessageGenerate() {
        switch (params.ctrlType) {
            case Motor_Ctrl_Type_e::Torque: {
                ControllerOutputData output = controller->GetOutputs();
                int16_t txTorque = Clamp(1 * output.dataPtr[0], -1000.f, 1000.f);

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
                ControllerOutputData output = controller->GetOutputs();
                int32_t txAngle = 100 * output.dataPtr[0];

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
        canAgent.Transmit(canAgent.addr - 0x100);
    }

    void Update() {
        state.position = static_cast<int16_t>(canAgent.rxbuf[6] | (canAgent.rxbuf[7] << 8u)) / 65536.0f * 360.0f;
        state.speed = static_cast<int16_t>(canAgent.rxbuf[4] | (canAgent.rxbuf[5] << 8u));
        state.torque = static_cast<int16_t>(canAgent.rxbuf[2] | (canAgent.rxbuf[3] << 8u));
        state.temperature = static_cast<int8_t>(canAgent.rxbuf[1]);
    }
};

#endif