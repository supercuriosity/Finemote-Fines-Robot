/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_I2C_TEST_H
#define FINEMOTE_I2C_TEST_H

#include "DeviceBase.h"
#include "Bus/I2C_Base.h"

template <uint8_t busID>
class I2C_test :public DeviceBase ,public I2C_Agent<busID>{
    uint8_t rxBuffer[50];
    int32_t data[8];
    uint8_t i2cRxBuffer[8];
public:
    HAL_StatusTypeDef rxState = HAL_TIMEOUT;

    explicit I2C_test(uint8_t addr) : I2C_Agent<busID>(addr,I2C_Bus<busID>::GetInstance()) {}

    void Handle() override {
        if (rxState != HAL_OK) {
            rxState = HAL_UARTEx_ReceiveToIdle_IT(&Serial_Host, rxBuffer, 50);
        }
        //std::vector<uint8_t> vec(data,data+8);
        //this->Write(vec);
        I2C_Bus<busID>::RTHandle();
    };
    void Trigger(){
        data_extract(rxBuffer, data, 8);
        std::vector<uint8_t> vec(data,data+8);
        this->Write(vec);
        this->Read(i2cRxBuffer,4);
        HAL_UARTEx_ReceiveToIdle_IT(&Serial_Host, rxBuffer, 50);
    }
    static void data_extract(uint8_t *rx, int32_t *data, int32_t num){
        // 示例：假设 PWM 命令格式是 "PWM:1000,2000,1500,1800,1600,1400\n"
        if (strncmp((char*)rx, "PWM:", 4) == 0) {

            char *data_str = (char*)rx + 4;
            char *token = strtok(data_str, ",");
            int i = 0;
            while (token != NULL && i < num) {
                data[i] = atoi(token);
                token = strtok(NULL, ",");
                i++;
            }
        }
    }
};


#endif //FINEMOTE_I2C_TEST_H
