/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "RadioMaster_Zorro.h"
#include <cmath>

#ifdef RADIOMASTER_ZORRO_MODULE


RadioMaster_Zorro::~RadioMaster_Zorro() {

}

void RadioMaster_Zorro::Decode(UART_Task_t _data) {

    if (_data.task != UART_Task_e::READ) {
        remoteUart.Read(rxBuff, [this](UART_Task_t a) { this->Decode(a); });
        return;
    }
    info.rR = (((rxBuff[1] | (rxBuff[2] << 8)) & 0x07FF) - 1000) / 810.0f;
    info.rC = ((((rxBuff[2] >> 3) | (rxBuff[3] << 5)) & 0x07FF) - 1000) / 810.0f;
    info.lC = ((((rxBuff[3] >> 6) | (rxBuff[4] << 2) | (rxBuff[5] << 10)) & 0x07FF) - 1000) / 810.0f;
    info.lR = ((((rxBuff[5] >> 1) | (rxBuff[6] << 7)) & 0x07FF) - 1000) / 810.0f;
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
    info.sE = ((((rxBuff[6] >> 4) | (rxBuff[7] << 4)) & 0x07FF) == 191) ? static_cast <SWITCH_STATE_E> (1)
                                                                        : static_cast <SWITCH_STATE_E> (2);
    info.sB = ((((rxBuff[7] >> 7) | (rxBuff[8] << 1) | (rxBuff[8] << 9)) & 0x07FF) == 256)
              ? static_cast <SWITCH_STATE_E> (2) :
              ((((rxBuff[7] >> 7) | (rxBuff[8] << 1) | (rxBuff[8] << 9)) & 0x07FF) == 1509)
              ? static_cast <SWITCH_STATE_E> (3) : static_cast <SWITCH_STATE_E> (1);
    info.sF = ((((rxBuff[9] >> 2) | (rxBuff[10] << 6)) & 0x07FF) == 191) ? static_cast <SWITCH_STATE_E> (1)
                                                                         : static_cast <SWITCH_STATE_E> (2);
    last_stamp = HAL_GetTick();
    remoteUart.Read(rxBuff, [this](UART_Task_t a) { this->Decode(a); });
}

void RadioMaster_Zorro::Handle() {
    //remoteUart.Read(rxBuff, [this](UART_Task_t a) { this->Decode(a); });
}

#endif