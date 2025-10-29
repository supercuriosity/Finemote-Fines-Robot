/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_MOTOR3507_H
#define FINEMOTE_MOTOR3507_H

#include "Motors/MotorBase.hpp"
#include "Bus/CAN_Base.hpp"

/**
 * Todo: Reduction ratio
 */
template <int busID>
class HO3507 : public MotorBase {
public:
    template <typename T>
    HO3507(const Motor_Param_t&& params, T& _controller, uint32_t addr) :
            MotorBase(std::forward<const Motor_Param_t>(params)), canAgent(addr) {
        ResetController(_controller);
        initTick = HAL_GetTick();
        this->SetDivisionFactor(20);
    }

    void Handle() final {
        Update();
        controller->Calc();
        if (HAL_GetTick() - initTick < 5000) {
            ChooseCtrlType();
            Start();
        } else {
            MessageGenerate();
        }
    }

    CAN_Agent<busID> canAgent;

private:
    uint32_t initTick;

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

    /**
     * Todo: 放到构造函数里面
     */
    void Start() {
        canAgent[0] = 0xFF;
        canAgent[1] = 0xFF;
        canAgent[2] = 0xFF;
        canAgent[3] = 0xFF;
        canAgent[4] = 0xFF;
        canAgent[5] = 0XFF;
        canAgent[6] = 0xFF;
        canAgent[7] = 0xFC;
        canAgent.Transmit(canAgent.addr);
    }

    /**
     * Todo: 放到构造函数里面
     */
    void ChooseCtrlType() {
        switch (params.ctrlType) {
            //力矩环下，力矩（电流）代表电机在该力矩下运行
            case Motor_Ctrl_Type_e::Torque: {
                canAgent[0] = 0xFF;
                canAgent[1] = 0xFF;
                canAgent[2] = 0xFF;
                canAgent[3] = 0xFF;
                canAgent[4] = 0xFF;
                canAgent[5] = 0XFF;
                canAgent[6] = 0xFF;
                canAgent[7] = 0xF9;
                break;
            }
                //位置速度力矩三闭环模式下,速度命令代表电机在位置控制下可达到的最大速度,力矩命令代表电机可达到的最大力矩
            case Motor_Ctrl_Type_e::Position: {
                canAgent[0] = 0xFF;
                canAgent[1] = 0xFF;
                canAgent[2] = 0xFF;
                canAgent[3] = 0xFF;
                canAgent[4] = 0xFF;
                canAgent[5] = 0XFF;
                canAgent[6] = 0xFF;
                canAgent[7] = 0xFB;
                break;
            }
                //速度力矩环下，速度命令代表电机运行速度，力矩（电流）代表电机在该速度下运行，能提供的最大电流
            case Motor_Ctrl_Type_e::Speed: {
                canAgent[0] = 0xFF;
                canAgent[1] = 0xFF;
                canAgent[2] = 0xFF;
                canAgent[3] = 0xFF;
                canAgent[4] = 0xFF;
                canAgent[5] = 0XFF;
                canAgent[6] = 0xFF;
                canAgent[7] = 0xFA;
                break;
            }
        }
        canAgent.Transmit(canAgent.addr);
    }

    void MessageGenerate() {
        switch (params.ctrlType) {
            case Motor_Ctrl_Type_e::Speed: {
                ControllerOutputData output = controller->GetOutputs();
                float txSpeed = -output.dataPtr[0]; //方向取CCW
                int32_t txSpeedCode = txSpeed / 58.639f * 0x800 + 0x800;
                canAgent[0] = 0x00;
                canAgent[1] = 0x00;
                canAgent[2] = (txSpeedCode >> 4) & 0x0FF;
                canAgent[3] = ((txSpeedCode << 8) & 0xF00) + 1; //kp
                canAgent[4] = 0x00;
                canAgent[5] = 0x02; //kd
                canAgent[6] = 0x00;
                canAgent[7] = 0x00;
                break;
            }
            case Motor_Ctrl_Type_e::Position: {
                ControllerOutputData output = controller->GetOutputs();
                float txPosition = -output.dataPtr[0]; //方向取CCW
                uint16_t txPositionCode = txPosition / 360.0f * 0x8000 + 0x8000;
                uint16_t velocity = (100 / 200) * 0x800 + 0x800; //100为设置速度，200为最大速度
                uint16_t torque = (1 / 4) * 0x800 + 0x800; //100为设置速度，200为最大速度
                canAgent[0] = (txPositionCode >> 8) & 0xFF;
                canAgent[1] = txPositionCode & 0xFF;
                canAgent[2] = 0xB8;
                canAgent[3] = 0x00; //kp
                canAgent[4] = 0x50;
                canAgent[5] = 0x60; //kd
                canAgent[6] = 0x0A;
                canAgent[7] = 0x00;
                break;
            }
        }
        canAgent.Transmit(canAgent.addr);
    }

    void Update() { //正方向取CCW
        int16_t position_code = (canAgent.rxbuf[1] << 8) | canAgent.rxbuf[2];
        state.position = -static_cast<float>(position_code - 0x8000) / 32768.0f * 360.0f;
        int16_t speed_code = (canAgent.rxbuf[3] << 4) | ((canAgent.rxbuf[4] >> 4) & 0x0F);
        state.speed = -static_cast<float>(speed_code - 0x800) / 2048.0f * 58.639f;
        int16_t torque_code = (((canAgent.rxbuf[4]) & 0xF0) << 4) | canAgent.rxbuf[5];
        state.torque = -static_cast<float>(torque_code - 0x800) / 2048.0f * 4.0f;
    }
};

#endif