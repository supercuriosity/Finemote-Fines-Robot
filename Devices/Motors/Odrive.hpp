/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_ODRIVE_H
#define FINEMOTE_ODRIVE_H

#include "Motors/MotorBase.hpp"
#include "Bus/CAN_Base.hpp"

template <int busID>
class Odrive : public MotorBase {
public:
    template <typename T>
    Odrive(const Motor_Param_t&& params, T& _controller, uint32_t addr)
            : MotorBase(std::forward<const Motor_Param_t>(params)), canAgent(addr) {
        ResetController(_controller);
        this->SetDivisionFactor(20);
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
                float txTorque = Clamp(1 * output.dataPtr[0], -2000.f, 2000.f);
                volatile uint32_t txTorqueFloat = *reinterpret_cast<uint32_t*>(&txTorque);

                for (int i = 0; i < 4; ++i) {
                    canAgent[i] = (txTorqueFloat >> (i * 8)) & 0xFF;
                }
                canAgent[4] = 0x00;
                canAgent[5] = 0x00;
                canAgent[6] = 0x00;
                canAgent[7] = 0x00;
                canAgent.Transmit(canAgent.addr << 5 | 0x00e, CAN_ID_STD | CAN_RTR_DATA);
                break;
            }
            case Motor_Ctrl_Type_e::Position: {
                ControllerOutputData output = controller->GetOutputs();
                float pos = output.dataPtr[0] / 360.0f;
                uint32_t pos_binary = *reinterpret_cast<uint32_t*>(&pos);

                for (int i = 0; i < 4; ++i) {
                    canAgent[i] = (pos_binary >> (i * 8)) & 0xFF;
                }
                canAgent[4] = 0x00;
                canAgent[5] = 0x00;
                canAgent[6] = 0x00;
                canAgent[7] = 0x00;
                canAgent.Transmit(canAgent.addr << 5 | 0x00c, CAN_ID_STD | CAN_RTR_DATA);
                break;
            }
            case Motor_Ctrl_Type_e::Speed: {
                ControllerOutputData output = controller->GetOutputs();
                float txSpeed = output.dataPtr[0];
                uint32_t txSpeedFloat = *reinterpret_cast<uint32_t*>(&txSpeed);

                for (int i = 0; i < 4; ++i) {
                    canAgent[i] = (txSpeedFloat >> (i * 8)) & 0xFF;
                }
                canAgent[4] = 0x00;
                canAgent[5] = 0x00;
                canAgent[6] = 0x00;
                canAgent[7] = 0x00;
                canAgent.Transmit(canAgent.addr << 5 | 0x00d, CAN_ID_STD | CAN_RTR_DATA);
                break;
            }
        }
        // canAgent.Send(canAgent.addr, CAN_ID_STD | CAN_RTR_REMOTE); //获取反馈数据
    }

    void Update() {
        uint32_t position_data = (canAgent.rxbuf[0] | (canAgent.rxbuf[1] << 8u) | (canAgent.rxbuf[2] << 16u) | (canAgent.rxbuf[3] << 24u));
        float position_float = *reinterpret_cast<float*>(&position_data);
        state.position = position_float;

        uint32_t speed_data = (canAgent.rxbuf[4] | (canAgent.rxbuf[5] << 8u) | (canAgent.rxbuf[6] << 16u) | (canAgent.rxbuf[7] << 24u));
        float speed_float = *reinterpret_cast<float*>(&speed_data);
        state.speed = speed_float;
    }
};

#endif