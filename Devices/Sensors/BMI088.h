/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_BMI088_H
#define FINEMOTE_BMI088_H

#include "ProjectConfig.h"

#ifdef BMI088_MODULE

#include "BMI088reg.h"

extern uint16_t TempPidTimer;
constexpr uint8_t ExpectTemp = 60;

class BMI088 : public DeviceBase {
public:
    enum BMI088_ERROR_E
    {
        BMI088_NO_ERROR = 0x00,
        BMI088_ACC_PWR_CTRL_ERROR = 0x01,
        BMI088_ACC_PWR_CONF_ERROR = 0x02,
        BMI088_ACC_CONF_ERROR = 0x03,
        BMI088_ACC_SELF_TEST_ERROR = 0x04,
        BMI088_ACC_RANGE_ERROR = 0x05,
        BMI088_INT1_IO_CTRL_ERROR = 0x06,
        BMI088_INT_MAP_DATA_ERROR = 0x07,
        BMI088_GYRO_RANGE_ERROR = 0x08,
        BMI088_GYRO_BANDWIDTH_ERROR = 0x09,
        BMI088_GYRO_LPM1_ERROR = 0x0A,
        BMI088_GYRO_CTRL_ERROR = 0x0B,
        BMI088_GYRO_INT3_INT4_IO_CONF_ERROR = 0x0C,
        BMI088_GYRO_INT3_INT4_IO_MAP_ERROR = 0x0D,
        BMI088_SELF_TEST_ACCEL_ERROR = 0x80,
        BMI088_SELF_TEST_GYRO_ERROR = 0x40,
        BMI088_NO_SENSOR = 0xFF,
    };
    struct __packed BMI088RawData_t{
            float accel[3];
            float temp;
            float gyro[3];
    };
    struct __packed BMI088RealData_t{
            float accel[3];
            float temp;
            float gyro[3];
    };
    struct TempPID_t{
        float kp;
        float ki;
        float kd;
        float T;
        float err;
        float err_last;
        float err_sum;
        float output;
    };
public:
    BMI088();
    ~BMI088();

    void IMU_Init();
    void Acc_Init();
    void Gyro_Init();

    void ProcessRaw(); // TODO: 处理原始数据
    void Handle() override;

    void IMU_Read_Data(BMI088RawData_t *rawData);

    inline const BMI088RawData_t &GetRaw() const { return rawData; }
    inline const BMI088RealData_t &GetReal() const { return realData; }
private:
    BMI088RawData_t rawData;
    BMI088RealData_t realData;
    TempPID_t tempPID;
    BMI088_ERROR_E errorStatus[2];
private:
    void BMI088_Delay_us(uint32_t us);
    void BMI088_Delay_ms(uint32_t ms);

    void BMI088_ACC_NS_L(void);
    void BMI088_ACC_NS_H(void);
    void BMI088_GYRO_NS_L(void);
    void BMI088_GYRO_NS_H(void);

    uint8_t BMI088_Read_Write_Byte(uint8_t txdata);

    void BMI088_Write_Single_Reg(uint8_t reg, uint8_t data);
    void BMI088_Read_Single_Reg(uint8_t reg, uint8_t *return_data);
    void BMI088_Read_Multi_Reg(uint8_t reg, uint8_t *buf, uint8_t len);

    void BMI088_ACC_Write_Single_Reg(uint8_t reg, uint8_t data);
    void BMI088_ACC_Read_Single_Reg(uint8_t reg, uint8_t *return_data);
    void BMI088_ACC_Read_Multi_Reg(uint8_t reg, uint8_t *buf, uint8_t len);

    void BMI088_GYRO_Write_Single_Reg(uint8_t reg, uint8_t data);
    void BMI088_GYRO_Read_Single_Reg(uint8_t reg, uint8_t *return_data);
    void BMI088_GYRO_Read_Multi_Reg(uint8_t reg, uint8_t *buf, uint8_t len);

    void TempPid_calc(TempPID_t *pid, float target, float measure);
    void TempPid_output(TempPID_t *pid);
private:
    uint8_t Write_BMI088_ACC_Reg_Data_Error[BMI088_WRITE_ACCEL_REG_NUM][3] =
            {
                    {BMI088_ACC_PWR_CTRL, BMI088_ACC_ENABLE_ACC_ON, BMI088_ACC_PWR_CTRL_ERROR},
                    {BMI088_ACC_PWR_CONF, BMI088_ACC_PWR_ACTIVE_MODE, BMI088_ACC_PWR_CONF_ERROR},
                    {BMI088_ACC_CONF,  BMI088_ACC_NORMAL| BMI088_ACC_800_HZ | BMI088_ACC_CONF_MUST_Set, BMI088_ACC_CONF_ERROR},
                    {BMI088_ACC_RANGE, BMI088_ACC_RANGE_3G, BMI088_ACC_RANGE_ERROR},
                    {BMI088_INT1_IO_CTRL, BMI088_ACC_INT1_IO_ENABLE | BMI088_ACC_INT1_GPIO_PP | BMI088_ACC_INT1_GPIO_LOW, BMI088_INT1_IO_CTRL_ERROR},
                    {BMI088_INT_MAP_DATA, BMI088_ACC_INT1_DRDY_INTERRUPT, BMI088_INT_MAP_DATA_ERROR}
            };
    uint8_t Write_BMI088_Gyro_Reg_Data_Error[BMI088_WRITE_GYRO_REG_NUM][3] =
            {
                    {BMI088_GYRO_RANGE, BMI088_GYRO_2000, BMI088_GYRO_RANGE_ERROR},
                    {BMI088_GYRO_BANDWIDTH, BMI088_GYRO_1000_116_HZ | BMI088_GYRO_BANDWIDTH_MUST_Set, BMI088_GYRO_BANDWIDTH_ERROR},
                    {BMI088_GYRO_LPM1, BMI088_GYRO_NORMAL_MODE, BMI088_GYRO_LPM1_ERROR},
                    {BMI088_GYRO_CTRL, BMI088_DRDY_ON, BMI088_GYRO_CTRL_ERROR},
                    {BMI088_GYRO_INT3_INT4_IO_CONF, BMI088_GYRO_INT3_GPIO_PP | BMI088_GYRO_INT3_GPIO_LOW, BMI088_GYRO_INT3_INT4_IO_CONF_ERROR},
                    {BMI088_GYRO_INT3_INT4_IO_MAP, BMI088_GYRO_DRDY_IO_INT3, BMI088_GYRO_INT3_INT4_IO_MAP_ERROR}

            };
};

#endif



#endif //FINEMOTE_BMI088_H
