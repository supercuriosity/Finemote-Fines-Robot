/*******************************************************************************
* Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_MOTOR3507_H
#define FINEMOTE_MOTOR3507_H

#include "ProjectConfig.h"

#ifdef MOTOR_COMPONENTS

#include "Bus/CAN_Base.h"
#include "Motors/MotorBase.h"

/**
 * Todo: Reduction ratio
 */
template<int busID>
class HO3507 : public MotorBase {
public:
    template<typename T>
    HO3507(const Motor_Param_t&& params, T& _controller, uint32_t addr) : MotorBase(std::forward<const Motor_Param_t>(params)), canAgent(addr) {
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

    /**
     * Todo: 放到构造函数里面
     */
    void Start(){
        canAgent[0] = 0xFF;
        canAgent[1] = 0xFF;
        canAgent[2] = 0xFF;
        canAgent[3] = 0xFF;
        canAgent[4] = 0xFF;
        canAgent[5] = 0XFF;
        canAgent[6] = 0xFF;
        canAgent[7] = 0xFC;
        canAgent.Send(0x0001);
    }

    /**
     * Todo: 放到构造函数里面
     */
    void ChooseCtrlType(){
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
        canAgent.Send(0x0001);
    }

    void MessageGenerate() {
        ChooseCtrlType();
        Start();
        switch (params.ctrlType) {
            case Motor_Ctrl_Type_e::Speed: {
                float txSpeed = controller->GetOutput();
                int32_t txSpeedCode = txSpeed/58.639f*0x800+0x800;
                canAgent[0] = 0x00;
                canAgent[1] = 0x00;
                canAgent[2] = (txSpeedCode >> 4) & 0x0FF;
                canAgent[3] = ((txSpeedCode << 8) & 0xF00)+1; //kp
                canAgent[4] = 0x00;
                canAgent[5] = 0x02; //kd
                canAgent[6] = 0x00;
                canAgent[7] = 0x00;
                break;
            }
        }
        canAgent.Send(0x0001);
    }

    void Update() {
        int16_t position_code = (canAgent.rxbuf[1] << 8) | canAgent.rxbuf[2];
        state.position = (position_code - 0x8000) / 32768.0f * 360.0f;
        int16_t speed_code = (canAgent.rxbuf[3] << 4) | ((canAgent.rxbuf[4] >> 4) & 0x0F);
        state.speed = (speed_code - 0x800) / 2048.0f * 58.639f;
        int16_t torque_code = (((canAgent.rxbuf[4]) & 0xF0) << 4) | canAgent.rxbuf[5];
        state.torque = (torque_code - 0x800) / 2048.0f * 4.0f;
    }
};

#endif //MOTOR_COMPONENTS
#endif //FINEMOTE_MOTOR3507_H
