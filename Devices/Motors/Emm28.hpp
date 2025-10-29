/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_EMM28_H
#define FINEMOTE_EMM28_H

#include "Motors/MotorBase.hpp"
#include "Bus/CAN_Base.hpp"

template <int busID>
class Emm28 : public MotorBase {
public:
    template <typename T>
    Emm28(const Motor_Param_t&& params, T& _controller, uint32_t addr) :
            MotorBase(std::forward<const Motor_Param_t>(params)), canAgent(addr) {
        ResetController(_controller);
    }

    void Handle() final {
        Update();
        controller->Calc();
        MessageGenerate();
        GetCurrentPosition();
    }

    CAN_Agent<busID> canAgent;

private:
    void SetFeedback() final {
        switch (params.targetType) {
            case Motor_Ctrl_Type_e::Position:
                controller->SetFeedbacks(&state.position);
                break;
        }
    }

    void MessageGenerate() {
        switch (params.ctrlType) {
            case Motor_Ctrl_Type_e::Position: {
                const uint16_t vel = 0x0100; // 转动速度(RPM)
                ControllerOutputData output = controller->GetOutputs();
                float target = output.dataPtr[0];
                const uint32_t clk = std::abs(std::round(target * 3200.f / 360.f)); // 16 细分下发送 3200 个脉冲电机旋转一圈

                canAgent.SetDLC(8);
                canAgent[0] = 0xFD;
                canAgent[1] = std::signbit(target) ? 0x00 : 0x01; // 0x01表示旋转方向为 CCW,0x00表示CW）
                canAgent[2] = (uint8_t)(vel >> 8);
                canAgent[3] = (uint8_t)(vel >> 0);
                canAgent[4] = 0x00; // 加速度，为0时不使用加速度
                canAgent[5] = (uint8_t)(clk >> 24);
                canAgent[6] = (uint8_t)(clk >> 16);
                canAgent[7] = (uint8_t)(clk >> 8);
                canAgent.Transmit(canAgent.addr, CAN_ID_EXT | CAN_RTR_DATA); // 扩展帧模式

                canAgent.SetDLC(5);
                canAgent[0] = 0xFD;
                canAgent[1] = (uint8_t)(clk >> 0);
                canAgent[2] = 0x01; // 00 表示相对位置模式（01 表示绝对位置模式）
                canAgent[3] = 0x00; // 00 表示不启用多机同步（01表示启用）
                canAgent[4] = 0x6B;
                canAgent.Transmit(canAgent.addr + 1, CAN_ID_EXT | CAN_RTR_DATA); // 第二段指令的地址+1
            }
        }
    }

    void GetCurrentPosition() {
        canAgent.SetDLC(2);
        canAgent[0] = 0x36;
        canAgent[1] = 0x6B;
        canAgent.Transmit(canAgent.addr, CAN_ID_EXT | CAN_RTR_DATA);
    }

    void Update() {
        if (canAgent.rxbuf[2] != 0xEE) {
            float tmp = ((canAgent.rxbuf[2] << 24u) | (canAgent.rxbuf[3] << 16u) | (canAgent.rxbuf[4] << 8u) | (canAgent.rxbuf[5])) * 360.0f / 65536.0f;
            tmp *= canAgent.rxbuf[1] == 0x00 ? -1 : 1;
            state.position = fmod(tmp, 360.);
        }
    }
};

#endif