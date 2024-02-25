/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "BMI088.h"

#ifdef BMI088_MODULE

BMI088 imu;

BMI088::BMI088() {
    tempPID = TempPID_t{10, 0.1, 5, 0, 0, 0, 0};
}

BMI088::~BMI088() {

}

void BMI088::BMI088_ACC_Write_Single_Reg(std::uint8_t reg, std::uint8_t data) {
    BMI088_ACC_NS_L();
    BMI088_Write_Single_Reg(reg, data);
    BMI088_ACC_NS_H();
}

void BMI088::BMI088_ACC_Read_Single_Reg(std::uint8_t reg, std::uint8_t *return_data) {
    BMI088_ACC_NS_L();
    BMI088_Read_Single_Reg(reg, return_data);
    BMI088_ACC_NS_H();
}

void BMI088::BMI088_ACC_Read_Multi_Reg(std::uint8_t reg, std::uint8_t *buf, std::uint8_t len) {
    BMI088_ACC_NS_L();
    BMI088_Read_Multi_Reg(reg, buf, len);
    BMI088_ACC_NS_H();
}

void BMI088::BMI088_GYRO_Write_Single_Reg(std::uint8_t reg, std::uint8_t data) {
    BMI088_GYRO_NS_L();
    BMI088_Write_Single_Reg(reg, data);
    BMI088_GYRO_NS_H();
}

void BMI088::BMI088_GYRO_Read_Single_Reg(std::uint8_t reg, std::uint8_t *return_data) {
    BMI088_GYRO_NS_L();
    BMI088_Read_Single_Reg(reg, return_data);
    BMI088_GYRO_NS_H();
}

void BMI088::BMI088_GYRO_Read_Multi_Reg(std::uint8_t reg, std::uint8_t *buf, std::uint8_t len) {
    BMI088_GYRO_NS_L();
    BMI088_Read_Multi_Reg(reg, buf, len);
    BMI088_GYRO_NS_H();
}

uint8_t BMI088::BMI088_Read_Write_Byte(std::uint8_t txdata) {
    uint8_t return_data = 0;
    HAL_SPI_TransmitReceive(&hspi2, &txdata, &return_data, 1, 100);
    return return_data;
}

void BMI088::BMI088_Delay_us(std::uint32_t us) {
    uint32_t ticks = 0;
    uint32_t told = 0;
    uint32_t tnow = 0;
    uint32_t tcnt = 0;
    uint32_t reload = 0;
    reload = SysTick->LOAD;
    ticks = us * 168;
    told = SysTick->VAL;
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

void BMI088::BMI088_Delay_ms(std::uint32_t ms) {
    uint32_t i = 0;
    for (i = 0; i < ms; i++)
    {
        BMI088_Delay_us(1000);
    }
}

void BMI088::BMI088_ACC_NS_L(void) {
    HAL_GPIO_WritePin(SPI2_Accel_CS_GPIO_Port , SPI2_Accel_CS_Pin, GPIO_PIN_RESET);
}

void BMI088::BMI088_ACC_NS_H(void) {
    HAL_GPIO_WritePin(SPI2_Accel_CS_GPIO_Port , SPI2_Accel_CS_Pin, GPIO_PIN_SET);
}

void BMI088::BMI088_GYRO_NS_L(void) {
    HAL_GPIO_WritePin(SPI2_Gyro_CS_GPIO_Port , SPI2_Gyro_CS_Pin, GPIO_PIN_RESET);
}

void BMI088::BMI088_GYRO_NS_H(void) {
    HAL_GPIO_WritePin(SPI2_Gyro_CS_GPIO_Port , SPI2_Gyro_CS_Pin, GPIO_PIN_SET);
}

void BMI088::IMU_Init() {
    Acc_Init();
    Gyro_Init();
}

void BMI088::BMI088_Write_Single_Reg(std::uint8_t reg, std::uint8_t data) {
    BMI088_Read_Write_Byte(reg);
    BMI088_Read_Write_Byte(data);
}

void BMI088::BMI088_Read_Single_Reg(std::uint8_t reg, std::uint8_t *return_data) {
    BMI088_Read_Write_Byte(reg | 0x80);
    *return_data = BMI088_Read_Write_Byte(0x55);
}

void BMI088::BMI088_Read_Multi_Reg(std::uint8_t reg, std::uint8_t *buf, std::uint8_t len) {
    uint8_t i = 0;
    BMI088_Read_Write_Byte(reg | 0x80);
    for (i = 0; i < len; i++)
    {
        buf[i] = BMI088_Read_Write_Byte(0x55);
    }
}

void BMI088::Acc_Init() {
    uint8_t res = 0;
    uint8_t write_reg_num = 0;

    //check communication
    BMI088_ACC_Read_Single_Reg(BMI088_ACC_CHIP_ID, &res);
    BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    BMI088_ACC_Read_Single_Reg(BMI088_ACC_CHIP_ID, &res);
    BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    //accel software reset
    BMI088_ACC_Write_Single_Reg(BMI088_ACC_SOFTRESET, BMI088_ACC_SOFTRESET_VALUE);
    BMI088_Delay_ms(BMI088_LONG_DELAY_TIME);

    //check communication is normal after reset
    BMI088_ACC_Read_Single_Reg(BMI088_ACC_CHIP_ID, &res);
    BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    BMI088_ACC_Read_Single_Reg(BMI088_ACC_CHIP_ID, &res);
    BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    // check the "who am I"
    if (res != BMI088_ACC_CHIP_ID_VALUE)
    {
        errorStatus[0] = static_cast<BMI088_ERROR_E>(BMI088_NO_SENSOR);
    }

    //set accel sensor config and check
    for (write_reg_num = 0; write_reg_num < BMI088_WRITE_ACCEL_REG_NUM; write_reg_num++)
    {

        BMI088_ACC_Write_Single_Reg(Write_BMI088_ACC_Reg_Data_Error[write_reg_num][0], Write_BMI088_ACC_Reg_Data_Error[write_reg_num][1]);
        BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        BMI088_ACC_Read_Single_Reg(Write_BMI088_ACC_Reg_Data_Error[write_reg_num][0], &res);
        BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        if (res != Write_BMI088_ACC_Reg_Data_Error[write_reg_num][1])
        {
            errorStatus[0] = static_cast<BMI088_ERROR_E>(Write_BMI088_ACC_Reg_Data_Error[write_reg_num][2]);
        }
    }
    errorStatus[0] = BMI088_NO_ERROR;
}

void BMI088::Gyro_Init() {
    uint8_t write_reg_num = 0;
    uint8_t res = 0;

    //check communication
    BMI088_GYRO_Read_Single_Reg(BMI088_GYRO_CHIP_ID, &res);
    BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    BMI088_GYRO_Read_Single_Reg(BMI088_GYRO_CHIP_ID, &res);
    BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    //reset the gyro sensor
    BMI088_GYRO_Write_Single_Reg(BMI088_GYRO_SOFTRESET, BMI088_GYRO_SOFTRESET_VALUE);
    BMI088_Delay_ms(BMI088_LONG_DELAY_TIME);
    //check communication is normal after reset
    BMI088_GYRO_Read_Single_Reg(BMI088_GYRO_CHIP_ID, &res);
    BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);
    BMI088_GYRO_Read_Single_Reg(BMI088_GYRO_CHIP_ID, &res);
    BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    // check the "who am I"
    if (res != BMI088_GYRO_CHIP_ID_VALUE)
    {
        errorStatus[1] = BMI088_NO_SENSOR;
    }

    //set gyro sensor config and check
    for (write_reg_num = 0; write_reg_num < BMI088_WRITE_GYRO_REG_NUM; write_reg_num++)
    {

        BMI088_GYRO_Write_Single_Reg(Write_BMI088_Gyro_Reg_Data_Error[write_reg_num][0], Write_BMI088_Gyro_Reg_Data_Error[write_reg_num][1]);
        BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        BMI088_GYRO_Read_Single_Reg(Write_BMI088_Gyro_Reg_Data_Error[write_reg_num][0], &res);
        BMI088_Delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        if (res != Write_BMI088_Gyro_Reg_Data_Error[write_reg_num][1])
        {
            errorStatus[1] = static_cast<BMI088_ERROR_E>(Write_BMI088_Gyro_Reg_Data_Error[write_reg_num][2]);
        }
    }

    errorStatus[1] = BMI088_NO_ERROR;
}

void BMI088::ProcessRaw() {

}

void BMI088::IMU_Read_Data(BMI088RawData_t *rawData) {
    uint8_t buf[8] = {0, 0, 0, 0, 0, 0};
    int16_t bmi088_raw_temp;

    BMI088_ACC_Read_Multi_Reg(BMI088_ACCEL_XOUT_L, buf, 6); // 0x12~0x17 为X轴低8位到Z轴高8位

    bmi088_raw_temp = (int16_t)((buf[1]) << 8) | buf[0];
    rawData->accel[0] = static_cast<float>(bmi088_raw_temp * BMI088_ACCEL_3G_SEN);
    bmi088_raw_temp = (int16_t)((buf[3]) << 8) | buf[2];
    rawData->accel[1] = static_cast<float>(bmi088_raw_temp * BMI088_ACCEL_3G_SEN);
    bmi088_raw_temp = (int16_t)((buf[5]) << 8) | buf[4];
    rawData->accel[2] = static_cast<float>(bmi088_raw_temp * BMI088_ACCEL_3G_SEN);

    BMI088_GYRO_Read_Multi_Reg(BMI088_GYRO_CHIP_ID, buf, 8); // 0x00~0x07 为CHIP_ID, Reserved, X轴低8位到Z轴高8位
    if(buf[0] == BMI088_GYRO_CHIP_ID_VALUE)
    {
        bmi088_raw_temp = (int16_t)((buf[3]) << 8) | buf[2];
        rawData->gyro[0] = static_cast<float>(bmi088_raw_temp * BMI088_GYRO_2000_SEN);
        bmi088_raw_temp = (int16_t)((buf[5]) << 8) | buf[4];
        rawData->gyro[1] = static_cast<float>(bmi088_raw_temp * BMI088_GYRO_2000_SEN);
        bmi088_raw_temp = (int16_t)((buf[7]) << 8) | buf[6];
        rawData->gyro[2] = static_cast<float>(bmi088_raw_temp * BMI088_GYRO_2000_SEN);
    }
    BMI088_ACC_Read_Multi_Reg(BMI088_TEMP_M, buf, 2); // 0x22为temperature[10:3], 0x23为temperature[2:0]

    bmi088_raw_temp = (int16_t)((buf[0] << 3) | (buf[1] >> 5));

    if (bmi088_raw_temp > 1023)
    {
        bmi088_raw_temp -= 2048;
    }

    rawData->temp = static_cast<float>(bmi088_raw_temp * BMI088_TEMP_FACTOR + BMI088_TEMP_OFFSET);
}

void BMI088::TempPid_calc(BMI088::TempPID_t *pid, float target, float measure) {
//    if (TempPidTimer >= 200){ //pid计算周期为100ms
    pid->err = target - measure; // 本次误差
    pid->err_sum += pid->err; // 误差积分
    pid->output = pid->kp * pid->err + pid->ki * pid->err_sum + pid->kd * (pid->err - pid->err_last);
    pid->err_last = pid->err; // 保存误差
//    }
}

void BMI088::TempPid_output(BMI088::TempPID_t *pid) {
    if (pid->output > 100) pid->output = 100;
    if (pid->output < 0) pid->output = 0;
    TIM3->CCR2 = static_cast<uint32_t>(499*(1-pid->output/100)); // 0%占空比对应499，100%占空比对应0
//    TempPidTimer = 0;
}

void BMI088::Handle() {
    IMU_Read_Data(&rawData);
    ProcessRaw();
    TempPid_calc(&tempPID, ExpectTemp, rawData.temp);
    TempPid_output(&tempPID);
}

#endif



