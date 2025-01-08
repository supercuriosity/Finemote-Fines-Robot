/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_VERIFY_H
#define FINEMOTE_VERIFY_H

#include <cstdint>

uint8_t CRC8Calc(uint8_t *data, uint16_t length);

uint16_t CRC16Calc(uint8_t *data, uint16_t length);

uint32_t CRC32Calc(uint8_t *data,uint16_t length);


#endif //FINEMOTE_VERIFY_H
