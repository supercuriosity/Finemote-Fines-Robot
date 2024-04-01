/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "RadioMaster_Zorro.h"

#ifdef RADIOMASTER_ZORRO_MODULE

RadioMaster_Zorro zorro;

RadioMaster_Zorro::~RadioMaster_Zorro() {

}

void RadioMaster_Zorro::Decode(UART_Task_t _data) {
//    channel[0] = (rxBuff[1] | (rxBuff[2] << 8)) & 0x07FF;
//    channel[1] = ((rxBuff[2] >> 3) | (rxBuff[3] << 5)) & 0x07FF;
//    channel[2] = ((rxBuff[3] >> 6) | (rxBuff[4] << 2) | (rxBuff[5] << 10)) & 0x07FF;
//    channel[3] = ((rxBuff[5] >> 1) | (rxBuff[6] << 7)) & 0x07FF;
//    channel[4] = ((rxBuff[6] >> 4) | (rxBuff[7] << 4)) & 0x07FF;
//    channel[5] = ((rxBuff[7] >> 7) | (rxBuff[8] << 1) | (rxBuff[8] << 9)) & 0x07FF;
//    channel[6] = ((rxBuff[9] >> 2) | (rxBuff[10] << 6)) & 0x07FF;
//    channel[7] = ((rxBuff[10] >> 5) | (rxBuff[11] << 3)) & 0x07FF;
//    channel[8] = (rxBuff[12] | (rxBuff[13] << 8)) & 0x07FF;
//    channel[9] = ((rxBuff[13] >> 3) | (rxBuff[14] << 5)) & 0x07FF;
//    channel[10] = ((rxBuff[14] >> 6) | (rxBuff[15] << 2) | (rxBuff[16] << 10)) & 0x07FF;
//    channel[11] = ((rxBuff[16] >> 1) | (rxBuff[17] << 7)) & 0x07FF;
//    channel[12] = ((rxBuff[17] >> 4) | (rxBuff[18] << 4)) & 0x07FF;
//    channel[13] = ((rxBuff[18] >> 7) | (rxBuff[19] << 1) | (rxBuff[20] << 9)) & 0x07FF;
//    channel[14] = ((rxBuff[20] >> 2) | (rxBuff[21] << 6)) & 0x07FF;
//    channel[15] = ((rxBuff[21] >> 5) | (rxBuff[22] << 3)) & 0x07FF;
    if (_data.task != UART_Task_e::READ) {
        remoteUart.Read(rxBuff, [this](UART_Task_t a) { this->Decode(a); });
        return;
    }
    info.rightRol = (((rxBuff[1] | (rxBuff[2] << 8)) & 0x07FF) - 1000) / 810.0f;
    info.rightCol = ((((rxBuff[2] >> 3) | (rxBuff[3] << 5)) & 0x07FF) - 1000) / 810.0f;
    info.leftCol = ((((rxBuff[3] >> 6) | (rxBuff[4] << 2) | (rxBuff[5] << 10)) & 0x07FF) - 1000) / 810.0f;
    info.leftRol = ((((rxBuff[5] >> 1) | (rxBuff[6] << 7)) & 0x07FF) - 1000) / 810.0f;
    info.sE = ((((rxBuff[6] >> 4) | (rxBuff[7] << 4)) & 0x07FF) == 191) ? static_cast <SWITCH_STATE_E> (1)
                                                                        : static_cast <SWITCH_STATE_E> (2);
    info.sB = ((((rxBuff[7] >> 7) | (rxBuff[8] << 1) | (rxBuff[8] << 9)) & 0x07FF) == 256)
              ? static_cast <SWITCH_STATE_E> (2) :
              ((((rxBuff[7] >> 7) | (rxBuff[8] << 1) | (rxBuff[8] << 9)) & 0x07FF) == 1509)
              ? static_cast <SWITCH_STATE_E> (3) : static_cast <SWITCH_STATE_E> (1);
    info.sF = ((((rxBuff[9] >> 2) | (rxBuff[10] << 6)) & 0x07FF) == 191) ? static_cast <SWITCH_STATE_E> (1)
                                                                         : static_cast <SWITCH_STATE_E> (2);

    remoteUart.Read(rxBuff, [this](UART_Task_t a) { this->Decode(a); });
}

void RadioMaster_Zorro::Handle() {
    //remoteUart.Read(rxBuff, [this](UART_Task_t a) { this->Decode(a); });
}

#endif