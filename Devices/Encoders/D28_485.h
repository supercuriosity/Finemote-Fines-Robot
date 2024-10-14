/*******************************************************************************
* Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef D28_485_H
#define D28_485_H

#include "ProjectConfig.h"
#include "DeviceBase.h"
#include "Bus/RS485_Base.h"
#include "Communication/Verify.h"


template<int busID>
class D28_485 : public DeviceBase {
public:
    D28_485(uint32_t addr) :  rs485Agent(addr) {
        this->SetDivisionFactor(1000);
    }
    void Handle() override {
		GetAngle();
		Update();
    };

    RS485_Agent<busID> rs485Agent;
    float angle{0};

private:
    void GetAngle() {
        rs485Agent.txbuf[0] = rs485Agent.addr;
        rs485Agent.txbuf[1] = 0x00;
        rs485Agent.txbuf[2] = 0x00;
        rs485Agent.txbuf[3] = 0x00;
        rs485Agent.txbuf[4] = 0x00;
        rs485Agent.txbuf[5] = 0x00;
        rs485Agent.txbuf[6] = 0x00;
        rs485Agent.txbuf[7] = 0x00;
        rs485Agent.txbuf[8] = 0x00;
        rs485Agent.txbuf[9] = 0x00;
        rs485Agent.txbuf[10] = 0x00;
        rs485Agent.txbuf[11] = 0x00;
        rs485Agent.txbuf[12] = 0x00;
        rs485Agent.txbuf[13] = 0x00;
        rs485Agent.txbuf[14] = 0x00;
        rs485Agent.txbuf[15] = 0x00;
	    rs485Agent.txbuf[16] = 0x00;
        uint32_t crc32 = CRC32Calc(rs485Agent.txbuf, 17);
        rs485Agent.txbuf[17] = crc32;
        rs485Agent.txbuf[18] = crc32 >> 8;
        rs485Agent.txbuf[19] = crc32 >> 16;
        rs485Agent.txbuf[20] = crc32 >> 24;
        rs485Agent.SendMsg(21);
    }

    void Update() {
        int Position;
        if((rs485Agent.rxbuf[0]>>7) == 0)
        {
            Position = (((int64_t)0x000000)<<40) |\
            (((int64_t)rs485Agent.rxbuf[1])<<32) | \
            (((int64_t)rs485Agent.rxbuf[2])<<24) | \
            (((int64_t)rs485Agent.rxbuf[3])<<16) | \
            (((int64_t)rs485Agent.rxbuf[4])<<8) | \
            (((int64_t)rs485Agent.rxbuf[5])<<0);
        } else{
            Position = (((int64_t)0xFFFFFF)<<40) |\
            (((int64_t)rs485Agent.rxbuf[1])<<32) | \
            (((int64_t)rs485Agent.rxbuf[2])<<24) | \
            (((int64_t)rs485Agent.rxbuf[3])<<16) | \
            (((int64_t)rs485Agent.rxbuf[4])<<8) | \
            (((int64_t)rs485Agent.rxbuf[5])<<0);
        }
        angle = (float)Position/10485761*360;
    }
};



#endif //D28_485_H
