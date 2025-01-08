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

#define RIGID_NUMBER 1

typedef struct{
    float pos_X;
    float pos_Y;
    float pos_Z;
    float eul_X;
    float eul_Y;
    float eul_Z;
} __packed mqtt_brief_t;


typedef struct{
    uint32_t seq_pack;
    // double cam_time;
    bool rig_0_valid:1;
    bool rig_1_valid:1;
    bool rig_2_valid:1;
    bool rig_3_valid:1;
    bool rig_4_valid:1;
    bool rig_5_valid:1;
    bool rig_6_valid:1;
    bool rig_7_valid:1;
    mqtt_brief_t rig0_info;
#if RIGID_NUMBER > 1
    mqtt_brief_t rig1_info;
#endif
#if RIGID_NUMBER > 2
    mqtt_brief_t rig2_info;
#endif
#if RIGID_NUMBER > 3
    mqtt_brief_t rig3_info;
#endif
#if RIGID_NUMBER > 4
    mqtt_brief_t rig4_info;
#endif
#if RIGID_NUMBER > 5
    mqtt_brief_t rig5_info;
#endif
#if RIGID_NUMBER > 6
    mqtt_brief_t rig6_info;
#endif
#if RIGID_NUMBER > 7
    mqtt_brief_t rig7_info;
#endif

} __packed mqtt_brief_composed_t;

class FZMotion {
public:
    void Decode(uint8_t* data, uint16_t size){
        if(size == 29) {
            memcpy(&rxBuffer, data, sizeof(rxBuffer));
        }
    };
    mqtt_brief_composed_t GetData(){
        return rxBuffer;
    }

private:
    mqtt_brief_composed_t rxBuffer;
};

#endif //FINEMOTE_FZMOTION_H
