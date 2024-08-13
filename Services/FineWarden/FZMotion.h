/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_FZMOTION_H
#define FINEMOTE_FZMOTION_H

#include "UARTBaseLite.h"

typedef struct{
    uint32_t rig_id;
    uint32_t seq_pack;
    float cam_time;
    float pos_X;
    float pos_Y;
    float pos_Z;
    float vel_X;
    float vel_Y;
    float vel_Z;
    float acc_X;
    float acc_Y;
    float acc_Z;
    float eul_X;
    float eul_Y;
    float eul_Z;
    float qua_X;
    float qua_Y;
    float qua_Z;
    float qua_W;
} __packed MQTT_rigid_msg_t;

class FZMotion {
public:
    void Decode(uint8_t* data, uint16_t size){
        if (size >= sizeof(rxBuffer)) memcpy(&rxBuffer, data, sizeof(MQTT_rigid_msg_t));
    };

private:
    MQTT_rigid_msg_t rxBuffer;
};

#endif //FINEMOTE_FZMOTION_H
