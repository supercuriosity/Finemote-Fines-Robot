/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "Chassis.h"
#include "RadioMaster_Zorro.h"
#include "array"


constexpr PID_Param_t speedPID = {0.3f, 0.002f, 0.3f, 2000, 2000};

auto wheelControllers = CreateControllers<PID, 4>(speedPID);
auto swerveControllers = CreateControllers<Amplifier<1>, 4>();

//构建组成底盘的各个电机
#define TORQUE_2_SPEED {Motor_Ctrl_Type_e::Torque, Motor_Ctrl_Type_e::Speed}
Motor4010<1> CBRMotor(TORQUE_2_SPEED, wheelControllers[0], 0x144);
Motor4010<1> CBLMotor(TORQUE_2_SPEED, wheelControllers[1], 0x143);
Motor4010<1> CFLMotor(TORQUE_2_SPEED, wheelControllers[2], 0x142);
Motor4010<1> CFRMotor(TORQUE_2_SPEED, wheelControllers[3], 0x141);

#define DIRECT_POSITION {Motor_Ctrl_Type_e::Position, Motor_Ctrl_Type_e::Position}
Motor4315<1> SBLMotor(DIRECT_POSITION, swerveControllers[0], 0x01);
Motor4315<1> SBRMotor(DIRECT_POSITION, swerveControllers[1], 0x02);
Motor4315<1> SFLMotor(DIRECT_POSITION, swerveControllers[2], 0x03);
Motor4315<1> SFRMotor(DIRECT_POSITION, swerveControllers[3], 0x04);

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

RadioMaster_Zorro remote;
