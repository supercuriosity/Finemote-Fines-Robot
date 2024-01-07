/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_I2C_BASE_H
#define FINEMOTE_I2C_BASE_H

#include <queue>

#include "PeripheralDefination.h"

typedef enum : uint8_t {
    READ = 0,
    WRITE,
    WRITE_READ,
} I2C_Task_e;

typedef struct {
    uint8_t addr;
    I2C_Task_e task;
    uint8_t reg;
    uint8_t length;
    uint8_t* buf;
} I2C_Task_t __packed;

template <uint8_t busID>
class I2C_Bus {
public:
    static void Init() {
        volatile static I2C_Bus instance;
    }
    I2C_Bus(const I2C_Bus &) = delete;
    I2C_Bus& operator=(const I2C_Bus&) = delete;

    static void RTHandle() {
        if(taskQueue.empty()) {
            isFree = true;
            return;
        }

        switch (taskQueue.front().task) {
            case READ:
                HAL_I2C_Master_Receive_IT(&USER_I2C, taskQueue.front().addr << 1, taskQueue.front().buf, taskQueue.front().length);
                break;//TODO 维护多个可用的I2C总线
            case WRITE:
                HAL_I2C_Master_Transmit_IT(&USER_I2C, taskQueue.front().addr << 1 | 0x01, taskQueue.front().buf, taskQueue.front().length);
            case WRITE_READ:
                HAL_I2C_Mem_Read_IT(&USER_I2C, taskQueue.front().addr, taskQueue.front().reg, I2C_MEMADD_SIZE_8BIT, taskQueue.front().buf, taskQueue.front().length);
                break;
        }

        taskQueue.pop();
        isFree = false;
    }

    static std::queue<I2C_Task_t> taskQueue;

private:
    I2C_Bus() {

    }

    static bool isFree;
};

template <uint8_t busID, uint8_t bufferSize>
class I2C_Agent {
public:
    explicit I2C_Agent(uint8_t _addr) : addr(_addr) {
        I2C_Bus<busID>::Init();
        //HAL_I2C_IsDeviceReady(&hi2c2, addr << 1, 10, 1000);
    }

/*    I2C_Agent(uint8_t reg, uint8_t* buf) {

    }*/

    void Read(uint8_t reg) {
        I2C_Bus<busID>::taskQueue.push({addr, READ, reg, bufferSize, buf});
    }

    void Write(uint8_t reg) {
        buf[0] = reg;
        I2C_Bus<busID>::taskQueue.push({addr, WRITE, reg, bufferSize, buf});
    }

private:
    uint8_t addr;
    uint8_t buf[bufferSize] = {};
};

#endif //FINEMOTE_I2C_BASE_H
