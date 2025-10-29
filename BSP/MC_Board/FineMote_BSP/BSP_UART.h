/*******************************************************************************
* Copyright (c) 2025.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_BSP_UART_H
#define FINEMOTE_BSP_UART_H

#include "Board.h"

class BSP_UARTs {
public:
    static BSP_UARTs& GetInstance() {
        static BSP_UARTs instance;
        return instance;
    }

private:
    BSP_UARTs() {
        PeripheralsInit::GetInstance();
        BSP_UARTs_Setup();
    }
    void BSP_UARTs_Setup();
};

template <uint8_t ID>
class BSP_UART {
public:
    static BSP_UART& GetInstance() {
        static BSP_UART instance;
        return instance;
    }

    void Transmit(uint8_t* data, uint16_t size);

    void Receive(uint8_t* data, uint16_t size);

private:
    BSP_UART() {
        static_assert(ID > 0 && ID < sizeof(BSP_UARTList) / sizeof(BSP_UARTList[0]) && BSP_UARTList[ID] != nullptr, "Invalid UART ID");
        BSP_UARTs::GetInstance();
        BSP_UART_Setup();
    }
    void BSP_UART_Setup();
};

/**
 * BSP Specific Definitions
 */

inline void BSP_UARTs::BSP_UARTs_Setup() {

}

template<uint8_t ID>
void BSP_UART<ID>::BSP_UART_Setup() {

}

template<uint8_t ID>
void BSP_UART<ID>::Transmit(uint8_t *data, uint16_t size) {
    HAL_UART_Transmit_IT(BSP_UARTList[ID], data, size);
}

template<uint8_t ID>
void BSP_UART<ID>::Receive(uint8_t *data, uint16_t size) {
    HAL_UARTEx_ReceiveToIdle_IT(BSP_UARTList[ID], data, size);
}

#endif
