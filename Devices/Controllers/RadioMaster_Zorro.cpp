/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "RadioMaster_Zorro.h"

#ifdef RADIOMASTER_ZORRO_MODULE

RadioMaster_Zorro zorro;

RadioMaster_Zorro::RadioMaster_Zorro() {

}

RadioMaster_Zorro::~RadioMaster_Zorro() {

}

void RadioMaster_Zorro::Decode() {
    info.rightRol = (sbus.channel[0] - 1000) / 810.0f;
    info.rightCol = (sbus.channel[1] - 1000) / 810.0f;
    info.leftCol = (sbus.channel[2] - 1000) / 810.0f;
    info.leftRol = (sbus.channel[3] - 1000) / 810.0f;
    info.sE = (sbus.channel[4] == 191) ? static_cast <SWITCH_STATE_E> (1) : static_cast <SWITCH_STATE_E> (2);
    info.sB = (sbus.channel[5] == 256) ? static_cast <SWITCH_STATE_E> (2) :
              (sbus.channel[5] == 1509) ? static_cast <SWITCH_STATE_E> (3) : static_cast <SWITCH_STATE_E> (1);
    info.sF = (sbus.channel[6] == 191) ? static_cast <SWITCH_STATE_E> (1) : static_cast <SWITCH_STATE_E> (2);
}

void RadioMaster_Zorro::Handle() {
    sbus.Receive();
    Decode();
}

#endif