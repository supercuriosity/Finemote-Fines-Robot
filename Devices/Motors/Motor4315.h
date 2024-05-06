//
// Created by 25396 on 2024/3/13.
//

#ifndef FINEMOTE_MOTOR4315_H
#define FINEMOTE_MOTOR4315_H

#include "ProjectConfig.h"


#ifdef MOTOR_COMPONENTS

#include "DeviceBase.h"
#include "Bus/RS485_Base.h"
#include "Motors/MotorBase.h"

template<int busID>
class Motor4315 : public MotorBase {
public:
    //右值引用的重载，虽然不涉及资源的接管，但是为方便构建对象
    explicit Motor4315(MOTOR_INIT_t &&motorInit) {
        if (motorInit.speedPID) speedPID.PIDInfo = *motorInit.speedPID;
        if (motorInit.anglePID) anglePID.PIDInfo = *motorInit.anglePID;
        reductionRatio = motorInit.reductionRatio;
        ctrlType = motorInit.ctrlType;
        addr = motorInit.addr;
        this ->SetDivisionFactor(10);
    }
    explicit Motor4315(MOTOR_INIT_t &motorInit) {
        if (motorInit.speedPID) speedPID.PIDInfo = *motorInit.speedPID;
        if (motorInit.anglePID) anglePID.PIDInfo = *motorInit.anglePID;
        reductionRatio = motorInit.reductionRatio;
        ctrlType = motorInit.ctrlType;
        addr = motorInit.addr;
        this ->SetDivisionFactor(10);
    }

    void Handle() override {

        AngleCalc();
        CANMessageGet();

    };

    uint8_t rxbuf[15]{0};
    uint8_t txbuf[11]{0};
    RS485_Agent<busID> rs485Agent;

    float nowAngle{};
    float zeroAngle{};
private:
    void CANMessageGet() {
        auto receivedCallbackPack = [this](UART_Task_t uartTask) { this->receivedCallback(uartTask); };
        AngleCalc();

        txbuf[0] = 0x3E;//协议头
        txbuf[1] = 0x00;//包序号
        txbuf[2] = addr; //ID
        txbuf[3] = 0x55;//相对位置闭环控制命令码
        txbuf[4] = 0x04;//数据包长度
        txbuf[5] = txAngle;
        txbuf[6] = txAngle >> 8u;
        txbuf[7] = txAngle >> 16u;
        txbuf[8] = txAngle >> 24u;
        uint16_t crc16 = CRC16Calc(txbuf, 9);
        txbuf[9] = crc16;
        txbuf[10] = crc16 >> 8u;

        rs485Agent.Write(txbuf, 11);
    }

    void AngleCalc() {
        if (targetAngle - lastAngle > 180) {
            zeroAngle -= 360;
        }
        if (lastAngle - targetAngle > 180) {
            zeroAngle += 360;
        }
        lastAngle = targetAngle;
        realAngle = targetAngle + zeroAngle;
        if (stopflag) {
            txAngle = (int32_t) (zeroAngle * 16384.0f / 360.0f);
        } else {
            txAngle = (int32_t) (realAngle * 16384.0f / 360.0f);
        }
    }

    void receivedCallback(const UART_Task_t &_data) {
        memcpy(rxbuf, _data.bufPtr, _data.size);
        nowAngle = (float) (rxbuf[7] | (rxbuf[8] << 8u) | (rxbuf[9] << 16u) | (rxbuf[10] << 24u)) / 16384.0f * 360.0f;
    }

    uint16_t txSpeed = 0;
    int32_t txAngle = 0;


    float realAngle{0};
    float thisAngle{};
    float lastAngle{};


};

#endif

#endif //FINEMOTE_MOTOR4315_H
