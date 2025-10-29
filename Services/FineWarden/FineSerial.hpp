/*******************************************************************************
* Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINESERIAL_H
#define FINESERIAL_H

#include "ProjectConfig.h"

#include "Verification/CRC.h"

class FineSerial {
public:
    void Decode(uint8_t* data, uint16_t size) {
        constexpr uint8_t FRAME_HEADER = 0xAA;
        constexpr uint8_t FRAME_TRAILER = 0xBB;

        uint8_t crc = CRC8Calc(data + 3, size - 5);
        if (data[0] != FRAME_HEADER || data[size - 1] != FRAME_TRAILER || data[size - 2] != crc || size != 5 + data[2]) {
            return;
        }

        std::memcpy(&velCmd[0], data + 3, sizeof(float));
        std::memcpy(&velCmd[1], data + 7, sizeof(float));
        std::memcpy(&velCmd[2], data + 11, sizeof(float));
        std::memcpy(&rtCmd[0], data + 15, sizeof(float));
        std::memcpy(&rtCmd[1], data + 19, sizeof(float));
        std::memcpy(&rtCmd[2], data + 23, sizeof(float));
    }

    std::array<float, 3> GetVelCmd() const {
        std::array<float, 3> retval = {velCmd[0], velCmd[1], rtCmd[2]};
        return std::move(retval);
    }

private:
    std::array<float, 3> velCmd;
    std::array<float, 3> rtCmd;
};

#endif
