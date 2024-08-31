#ifndef FINEMOTE_RS485_BASE_H
#define FINEMOTE_RS485_BASE_H

#include "ProjectConfig.h"

#ifdef RS485_BASE_MODULE

#include "UARTBaseLite.h"

template <uint8_t busID>
class RS485_Base :public DeviceBase{
public:
    uint8_t rxBuffer[30]{0};
    std::queue<std::pair<uint8_t*, uint16_t>> txQueue;

    explicit RS485_Base(){
        HALInit::GetInstance();
        this->SetDivisionFactor(4);
        HAL_UARTEx_ReceiveToIdle_IT(uartHandleList[busID], UARTBaseLite<busID>::GetInstance().rxBuffer[0], 200);
    }

    static RS485_Base& GetInstance() {
        static RS485_Base instance;
        return instance;
    }

    void Transmit(uint8_t* data, uint16_t size) {
        txQueue.push(std::make_pair(data, size));
    }

    void TxLoader() {
        if (!txQueue.empty()) {
#ifndef RS485_NO_XXX
            HAL_GPIO_WritePin(rs485TxPortList[busID], rs485TxPinList[busID], GPIO_PIN_SET);
#endif
            HAL_UART_Transmit_IT(uartHandleList[busID], txQueue.front().first, txQueue.front().second);
            txQueue.pop();
        }
    }

    void RxHandle() {
#ifndef RS485_NO_XXX
        HAL_GPIO_WritePin(rs485TxPortList[busID], rs485TxPinList[busID], GPIO_PIN_RESET);
#endif
    }

    void Handle() final {
        std::copy(UARTBaseLite<busID>::GetInstance().rxBuffer[0],UARTBaseLite<busID>::GetInstance().rxBuffer[0]+30,rxBuffer);
        TxLoader();
    }
};

template <uint8_t busID>
class RS485_Agent : public DeviceBase{
public:

    RS485_Agent(uint32_t _addr) : addr(_addr){}

    void SendMsg(uint16_t size){
        RS485_Base<busID>::GetInstance().Transmit(txbuf,size);
    }

/**** 验证地址（貌似放这里不太合适） ****/
    void Handle() override{
        if(RS485_Base<busID>::GetInstance().rxBuffer[2] == addr){
            std::copy(RS485_Base<busID>::GetInstance().rxBuffer,RS485_Base<busID>::GetInstance().rxBuffer+30,rxbuf);
        }
    }

    uint32_t addr;
    uint8_t rxbuf[30]{0};
    uint8_t txbuf[30]{0};
};

#endif //RS485_BASE_MODULE

#endif //FINEMOTE_RS485_BASE_H
