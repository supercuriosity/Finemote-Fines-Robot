/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "RadioMaster_Zorro.h"
#include <cmath>

#ifdef RADIOMASTER_ZORRO_MODULE

void RadioMaster_Zorro::Decode(uint8_t* data, uint16_t length) {

    info.rR = (((data[1] | (data[2] << 8)) & 0x07FF) - 1000) / 810.0f;
    info.rC = ((((data[2] >> 3) | (data[3] << 5)) & 0x07FF) - 1000) / 810.0f;
    info.lC = ((((data[3] >> 6) | (data[4] << 2) | (data[5] << 10)) & 0x07FF) - 1000) / 810.0f;
    info.lR = ((((data[5] >> 1) | (data[6] << 7)) & 0x07FF) - 1000) / 810.0f;
    //设置死区
    if(abs(info.rR) > 0.1)
        info.rightRol = info.rR;
    else
        info.rightRol = 0;
    if(abs(info.rC) > 0.1)
        info.rightCol = info.rC;
    else
        info.rightCol = 0;
    if(abs(info.lC) > 0.1)
        info.leftCol = info.lC;
    else
        info.leftCol = 0;
    if(abs(info.lR) > 0.1)
        info.leftRol = info.lR;
    else
        info.leftRol = 0;

    info.sA = ((data[6] >> 4 | data[7] << 4) & 0x07FF) == 191 ? UP_POS : DOWN_POS;
    info.sB = ((data[7] >> 7 | data[8] << 1 | data[9] << 9) & 0x07FF) == 191
                  ? UP_POS
                  : ((data[7] >> 7 | data[8] << 1 | data[9] << 9) & 0x07FF) == 997
                  ? MID_POS
                  : DOWN_POS;
    info.sC = ((data[9] >> 2 | data[10] << 6) & 0x07FF) == 191
                  ? UP_POS
                  : ((data[9] >> 2 | data[10] << 6) & 0x07FF) == 997
                  ? MID_POS
                  : DOWN_POS;
    info.sD = ((data[10] >> 5 | data[11] << 3) & 0x07FF) == 191 ? UP_POS : DOWN_POS;
}

#endif
