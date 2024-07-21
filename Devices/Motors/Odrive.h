/*******************************************************************************
* Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_MOTORODRIVE_H
#define FINEMOTE_MOTORODRIVE_H

#include "ProjectConfig.h"

#ifdef MOTOR_COMPONENTS

#include "Bus/CAN_Base.h"
#include "Motors/MotorBase.h"

template<int busID>
class Odrive : public MotorBase {
public:

    template<typename T>
    Odrive(const Motor_Param_t&& params, T& _controller, uint32_t addr) : MotorBase(std::forward<const Motor_Param_t>(params)), canAgent(addr) {
        ResetController(_controller);
    }

    void Handle() final{
        Update();
        controller->Calc();
        MessageGenerate();
    }

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
                float txTorque = controller->GetOutput();
                INRANGE(txTorque, -2000, 2000);
                volatile uint32_t txTorqueFloat = *reinterpret_cast<uint32_t*>(&txTorque);

                for (int i = 0; i < 4; ++i) {
                    canAgent[i] = (txTorqueFloat >> (i * 8)) & 0xFF;
                }
                canAgent[4] = 0x00;
                canAgent[5] = 0x00;
                canAgent[6] = 0x00;
                canAgent[7] = 0x00;
                canAgent.Send(canAgent.addr & 0xffffffe0 | 0x00e);
                break;
            }
            case Motor_Ctrl_Type_e::Position: {
                float pos = controller->GetOutput();
                uint32_t pos_binary = *reinterpret_cast<uint32_t*>(&pos);

                for (int i = 0; i < 4; ++i) {
                    canAgent[i] = (pos_binary >> (i * 8)) & 0xFF;
                }
                canAgent[4] = 0x00;
                canAgent[5] = 0x00;
                canAgent[6] = 0x00;
                canAgent[7] = 0x00;
                canAgent.Send(canAgent.addr & 0xffffffe0 | 0x00c);
                break;
            }
            case Motor_Ctrl_Type_e::Speed: {
                float txSpeed = controller->GetOutput();
                uint32_t txSpeedFloat = *reinterpret_cast<uint32_t*>(&txSpeed);

                for (int i = 0; i < 4; ++i) {
                    canAgent[i] = (txSpeedFloat >> (i * 8)) & 0xFF;
                }
                canAgent[4] = 0x00;
                canAgent[5] = 0x00;
                canAgent[6] = 0x00;
                canAgent[7] = 0x00;
                canAgent.Send(canAgent.addr & 0xffffffe0 | 0x00d);
                break;
            }
        }
        canAgent.Send(canAgent.addr, CAN_ID_STD | CAN_RTR_REMOTE);
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
#endif //FINEMOTE_MOTORODRIVE_H
