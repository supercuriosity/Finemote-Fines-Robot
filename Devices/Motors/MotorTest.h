/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

//
// Created by 25396 on 2024/2/19.
//

#ifndef FINEMOTE_MOTORTEST_H
#define FINEMOTE_MOTORTEST_H

#include "DeviceBase.h"
#include "Bus/CAN_Base.h"
#include "MotorBase.h"


template<int busID>
class MotorTest : public MotorBase {
public:
    explicit MotorTest(uint32_t addr): canAgent(addr, CAN_Bus<busID>::GetInstance()) {}

    void Handle() override {
        feedback.angle = canAgent.rxbuf[6] | (canAgent.rxbuf[7] << 8u);
        feedback.speed = canAgent.rxbuf[4] | (canAgent.rxbuf[5] << 8u);
        feedback.moment = canAgent.rxbuf[2] | (canAgent.rxbuf[3] << 8u);
        feedback.temp = canAgent.rxbuf[1];
        // txSpeed = (uint16_t)targetSpeed;
        txSpeed = 0x300;
        txAngle = (uint32_t)(targetAngle * 100.0f);

        canAgent.DLC = 8;
        canAgent.txbuf[0] = 0xA4;
        canAgent.txbuf[1] = 0x00;
        canAgent.txbuf[2] = txSpeed;
        canAgent.txbuf[3] = txSpeed >> 8;
        canAgent.txbuf[4] = txAngle;
        canAgent.txbuf[5] = txAngle >> 8;
        canAgent.txbuf[6] = txAngle >> 16;
        canAgent.txbuf[7] = txAngle >> 24;
        canAgent.Send();

    };

    uint16_t txSpeed = 0;
    uint32_t txAngle = 0;
    CAN_Agent<busID> canAgent;
};

#endif //FINEMOTE_MOTORTEST_H
