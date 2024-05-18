/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "InstanceManager.h"

/** 算法类配合宏定义进行批量赋值 */
PID_Regulator_t adminSpeedPID = {//此为储存pid参数的结构体
        .kp = 0.3f,
        .ki = 0.002f,
        .kd = 0.3f,
        .componentKpMax = 2000,
        .componentKiMax = 0,
        .componentKdMax = 0,
        .outputMax = 2000
};
PID_Regulator_t adminAnglePID = {//此为储存pid参数的结构体
        .kp = 1.0f,
        .ki = 0.0f,
        .kd = 0.0f,
        .componentKpMax = 2000,
        .componentKiMax = 0,
        .componentKdMax = 0,
        .outputMax = 2000 //4010电机输出电流上限，可以调小，勿调大
};
//构建组成底盘的各个电机
Motor4010<1> CBRMotor(MOTOR_INIT_t{0x144, &adminSpeedPID, &adminAnglePID, External_Speed, 1});
Motor4010<1> CBLMotor(MOTOR_INIT_t{0x143, &adminSpeedPID, &adminAnglePID, External_Speed, 1});
Motor4010<1> CFLMotor(MOTOR_INIT_t{0x142, &adminSpeedPID, &adminAnglePID, External_Speed, 1});
Motor4010<1> CFRMotor(MOTOR_INIT_t{0x141, &adminSpeedPID, &adminAnglePID, External_Speed, 1});

Motor4315<1> SBLMotor(MOTOR_INIT_t{0x01, nullptr, nullptr, Internal, 1});
Motor4315<1> SBRMotor(MOTOR_INIT_t{0x02, nullptr, nullptr, Internal, 1});
Motor4315<1> SFLMotor(MOTOR_INIT_t{0x03, nullptr, nullptr, Internal, 1});
Motor4315<1> SFRMotor(MOTOR_INIT_t{0x04, nullptr, nullptr, Internal, 1});


//首先调取底盘类的构建器，然后使用提供的电机添加函数，将上文构建的电机指针传入构建器，最后由构建器返回构建好的底盘类对象
Chassis chassis = Chassis::Build().
        AddCFLMotor(CFLMotor).
        AddCFRMotor(CFRMotor).
        AddCBLMotor(CBLMotor).
        AddCBRMotor(CBRMotor).
        AddSFLMotor(SFLMotor).
        AddSFRMotor(SFRMotor).
        AddSBLMotor(SBLMotor).
        AddSBRMotor(SBRMotor).
        Build();

/** 类名称提供设备信息，对象名称通用 */
RadioMaster_Zorro zorro;
