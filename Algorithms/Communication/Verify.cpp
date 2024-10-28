/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "Verify.h"

uint8_t CRC8Calc(uint8_t *data, uint16_t length) {
    const uint8_t polynomial = 0x07; // 标准CRC8多项式
    uint8_t crc = 0x00; //初始值

    while (length--) {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (uint8_t)((crc << 1) ^ polynomial);
            } else {
                crc <<= 1; //
            }
        }
    }
    return crc;
}

uint16_t CRC16Calc(uint8_t *data, uint16_t length){
    uint16_t crc = 0xffff;        // Initial value
    while (length--) {
        crc ^= *data++;            // crc ^= *data; data++;
        for (uint8_t i = 0; i < 8; ++i) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

uint32_t CRC32Calc(uint8_t *data,uint16_t length) {
    const uint32_t polynomial = 0xEDB88320; // 标准CRC32多项式
    uint32_t crc = 0xFFFFFFFF; // 初始值

    while(length--) {
        crc ^= *data++;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
    }

    return ~crc; // 取反得到最终的CRC32值
}
