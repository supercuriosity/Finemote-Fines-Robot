/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "ProjectConfig.h"

/*****  示例1 *****/
#include "Emm28.h"
#include "LED.h"
#include "BeepMusic.h"
#include "Chassis.h"
#include "RMD_L_40xx_v3.h"
#include "Motor4010.h"
#include "Motor4315.h"
#include "FZMotion.h"
#include "RadioMaster_Zorro.h"
#include "RadioMaster_Pocket.h"

/**
 * @brief LED闪烁
 */
void Task1() {
    static int cnt = 0;
    cnt++;
    if(cnt > 1000) {
        cnt = 0;
        LED::Toggle();
    }
}

/*****  示例2 *****/
/**
 * @brief 音乐播放与切换
 */

void Task2() {
    if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)) {
        static int index = 1;
        BeepMusic::MusicChannels[0].Play(index++);
        index %= 3;
    }
    BeepMusic::MusicChannels[0].BeepService();
}

/*****  示例3 *****/

constexpr PID_Param_t speedPID = {0.23f, 0.008f, 0.3f, 2000, 2000};

auto wheelControllers = CreateControllers<PID, 4>(speedPID);
auto swerveControllers = CreateControllers<Amplifier<1>, 4>();

//构建组成底盘的各个电机
#define TORQUE_2_SPEED {Motor_Ctrl_Type_e::Torque, Motor_Ctrl_Type_e::Speed}
Motor4010<1> CBRMotor(TORQUE_2_SPEED, wheelControllers[0], 0x144);
Motor4010<1> CBLMotor(TORQUE_2_SPEED, wheelControllers[1], 0x143);
Motor4010<1> CFLMotor(TORQUE_2_SPEED, wheelControllers[2], 0x142);
Motor4010<1> CFRMotor(TORQUE_2_SPEED, wheelControllers[3], 0x141);

#define DIRECT_POSITION {Motor_Ctrl_Type_e::Position, Motor_Ctrl_Type_e::Position}
Motor4315<1> SBRMotor(DIRECT_POSITION, swerveControllers[0], 0x04);
Motor4315<1> SBLMotor(DIRECT_POSITION, swerveControllers[1], 0x03);
Motor4315<1> SFLMotor(DIRECT_POSITION, swerveControllers[2], 0x02);
Motor4315<1> SFRMotor(DIRECT_POSITION, swerveControllers[3], 0x01);


// constexpr PID_Param_t speedPID = {0.1f, 0.003f, 0.1f, 2000, 2000};
//
// auto wheelControllers = CreateControllers<PID, 4>(speedPID);
// auto swerveControllers = CreateControllers<Amplifier<1>, 4>();
//
// #define TORQUE_2_SPEED {Motor_Ctrl_Type_e::Torque, Motor_Ctrl_Type_e::Speed}
// #define TORQUE_2_POSITION {Motor_Ctrl_Type_e::Torque, Motor_Ctrl_Type_e::Position}
// #define DIRECT_POSITION {Motor_Ctrl_Type_e::Position, Motor_Ctrl_Type_e::Position}
//
// RMD_L_40xx_v3<1> CFRMotor(TORQUE_2_SPEED, wheelControllers[0], 0x242);
// RMD_L_40xx_v3<1> CFLMotor(TORQUE_2_SPEED, wheelControllers[1], 0x244);
// RMD_L_40xx_v3<1> CBLMotor(TORQUE_2_SPEED, wheelControllers[2], 0x246);
// RMD_L_40xx_v3<1> CBRMotor(TORQUE_2_SPEED, wheelControllers[3], 0x248);
//
// RMD_L_40xx_v3<1> SFRMotor(DIRECT_POSITION, swerveControllers[0], 0x241);
// RMD_L_40xx_v3<1> SFLMotor(DIRECT_POSITION, swerveControllers[1], 0x243);
// RMD_L_40xx_v3<1> SBLMotor(DIRECT_POSITION, swerveControllers[2], 0x245);
// RMD_L_40xx_v3<1> SBRMotor(DIRECT_POSITION, swerveControllers[3], 0x247);

// 首先调取底盘类的构建器，然后使用提供的电机添加函数，将上文构建的电机指针传入构建器，最后由构建器返回构建好的底盘类对象
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




/**
 * @brief 底盘根据遥控器数据运动
 */

FZMotion motion;
RadioMaster_Zorro remote;
RoutePlanning route_planning(0.3);//Kp为误差补偿系数

float plannedVelX{0},plannedVelY{0},plannedVelAngle{0},plannedX,plannedY,plannedAngle;
bool isPlanStart = false;

void Task3() {
    constexpr float speedLimit = 2;

    if(remote.GetInfo().sC == RemoteControl::SWITCH_STATE_E::DOWN_POS)//任务模式
    {
        route_planning.Update(chassis.chassisPos[0][0],chassis.WCSVelocity[0][0],chassis.chassisPos[1][0],chassis.WCSVelocity[1][0],chassis.chassisPos[2][0],chassis.WCSVelocity[2][0]);
        if(!isPlanStart)
        {
            route_planning.AddTarget(0.5,0,0.7,0,-PI/2,0,5);
            route_planning.AddTarget(1,0,1.5,0,0,0,5);
            route_planning.AddTarget(0,0,2,0,PI/2,0,5);
            route_planning.AddTarget(-0.5,0,1,0,PI/4,0,5);
            route_planning.AddTarget(-1,0,0,0,0,0,5);

			      isPlanStart=true;
        }
        route_planning.CalcSpeed();
        chassis.ChassisSetVelocity(route_planning.FBVel,route_planning.LRVel,route_planning.RTVel);
        route_planning.GetPlannedVel(plannedX,plannedY,plannedAngle,plannedVelX,plannedVelY,plannedVelAngle);
    }
    else if(remote.GetInfo().sC == RemoteControl::SWITCH_STATE_E::UP_POS)//遥控模式
    {
        chassis.ChassisSetVelocity(remote.GetInfo().rightCol * speedLimit,
                                   remote.GetInfo().rightRol * speedLimit,
                                   -remote.GetInfo().leftRol * PI);//通道值取负，
    }

    //重置里程计和任务
    if (remote.GetInfo().sA == RemoteControl::SWITCH_STATE_E::DOWN_POS) {
        chassis.ResetOdometry(0,0,0);
        isPlanStart = false;
    }

    if (remote.GetInfo().sD == RemoteControl::SWITCH_STATE_E::DOWN_POS) {
      chassis.ResetOdometry(0,0,motion.GetData().rig0_info.eul_Y/180.0f*PI);
    }
}


/*****  示例4 *****/

void Task4() {
}

/**
 * @brief 用户初始化
 */

#ifdef __cplusplus
extern "C" {
#endif

void Setup() {
    std::function<void(uint8_t *, uint16_t)> remoteDecodeFunc = [](uint8_t* data, uint16_t length){
        remote.Decode(data, length);
    };
    UARTBaseLite<3>::GetInstance().Bind(remoteDecodeFunc);

    std::function<void(uint8_t *, uint16_t)> decodeFunc = [](uint8_t* data, uint16_t length){
        motion.Decode(data, length);
    };
    UARTBaseLite<5>::GetInstance().Bind(decodeFunc);
}

/**
 * @brief 主循环，优先级低于定时器中断，不确定执行频率
 */
void Loop() {

}

#ifdef __cplusplus
}
#endif

/*****  不要修改以下代码 *****/

#ifdef __cplusplus
extern "C" {
#endif

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim == &TIM_Control) {
        HAL_IWDG_Refresh(&hiwdg);
        DeviceBase::DevicesHandle();
        Task1();
        Task2();
        Task3();
        Task4();

        CAN_Bus<1>::TxLoader();
        CAN_Bus<2>::TxLoader();
    }
}

#ifdef __cplusplus
}
#endif
