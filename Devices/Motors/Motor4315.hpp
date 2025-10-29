/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_MOTOR4315_H
#define FINEMOTE_MOTOR4315_H

#include "Motors/MotorBase.hpp"
#include "Bus/RS485_Base.hpp"
#include "Verification/CRC.h"

#define MOTOR_MAP_LENGTH 10

template <uint8_t ID>
class HTMotorProxy_RS485 {
public:
    HTMotorProxy_RS485(MotorBase* motor, uint8_t addr) {
        getMotorMap().insert(etl::make_pair(addr, motor));
    }

    static void Transmit(uint8_t* data, size_t size) {
        static RS485_Agent<ID> rs485Agent(0x3C, [](uint8_t* data, size_t size) {
            Decode(data, size);
        });
        rs485Agent.Transmit(data, size);
    }

private:
    static etl::map<uint8_t, MotorBase*, MOTOR_MAP_LENGTH>& getMotorMap() {
        static etl::map<uint8_t, MotorBase*, MOTOR_MAP_LENGTH> instance;
        return instance;
    }

    static void Decode(uint8_t* data, size_t size) {
        auto& motorMap = getMotorMap();
        if (CRC16Calc(data, 13) == (data[13] | data[14] << 8u) && motorMap.contains(data[2])) {
            MotorBase* motor = motorMap[data[2]];
            if (data[3] == 0x55) {
                motor->GetState().position = -1 * ((data[7] | (data[8] << 8u) | (data[9] << 16u) | (data[10] << 24u)) * 360.0f / 16384.0f);
                motor->GetState().speed = -1 * static_cast<int16_t>(data[11] | (data[12] << 8u));
                motor->GetState().torque = 0; // 电机应答不返回电流值
                motor->GetState().temperature = 0; // 电机应答不返回温度参数
            }
        }
    }
};

template <uint8_t BusID>
class Motor4315 : public MotorBase {
public:
    template <typename T>
    Motor4315(const Motor_Param_t&& params, T& _controller, uint8_t addr)
            : MotorBase(std::forward<const Motor_Param_t>(params)), id(addr), commuAgent(this, addr) { // Todo: ID和地址分离逻辑
        ResetController(_controller);
        this->SetDivisionFactor(20);
    }

    void Handle() override {
        controller->Calc();
        MessageGenerate();
    }

private:
    uint8_t txbuf[11] = {};

    void SetFeedback() override {
        switch (params.ctrlType) {
            case Motor_Ctrl_Type_e::Position:
                controller->SetFeedbacks(&state.position);
                break;
        }
    }

    void MessageGenerate() {
        switch (params.ctrlType) {
            case Motor_Ctrl_Type_e::Position: {
                ControllerOutputData output = controller->GetOutputs();
                float targetAngle = -1 * output.dataPtr[0];
                int32_t txAngle = targetAngle * 16384.0f / 360.0f;

                txbuf[0] = 0x3E; // 协议头
                txbuf[1] = 0x00; // 包序号
                txbuf[2] = id; // ID
                txbuf[3] = 0x55; // 绝对位置闭环控制命令码
                txbuf[4] = 0x04; // 数据包长度
                txbuf[5] = txAngle;
                txbuf[6] = txAngle >> 8u;
                txbuf[7] = txAngle >> 16u;
                txbuf[8] = txAngle >> 24u;
                uint16_t crc16 = CRC16Calc(txbuf, 9);
                txbuf[9] = crc16;
                txbuf[10] = crc16 >> 8u;
                commuAgent.Transmit(txbuf, 11);
                break;
            }
        }
    }

    const uint8_t id;
    HTMotorProxy_RS485<BusID> commuAgent;
};

#endif