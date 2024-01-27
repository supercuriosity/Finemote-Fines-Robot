/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_PROJECTCONFIG_H
#define FINEMOTE_PROJECTCONFIG_H

/******************************************************************************************************
1.根据cmakelist中选择的构建目标，导入对应BSP包
******************************************************************************************************/
#ifdef __MC_BOARD
#include "MC_Board.h"
#endif

#ifdef __ROBOMASTER_C
#include "Robomaster_C.h"
#endif

/******************************************************************************************************
2.处理各模块对BSP包的依赖关系，若满足全部依赖则启用模块，此处需要以注释形式给出应由BSP包实现的依赖条件的具体内容
 以@def 标记需要的宏定义，@variable 标记需要定义的变量，实现依赖条件后可由BSP包定义依赖条件对应宏定义
 各模块的部分配置选项也可以宏定义的方式提供于此处
 ******************************************************************************************************/

/**
 * I2C_PERIPHERAL 模块所需的I2C外设依赖条件
 * @def USER_I2C 配置的I2C外设
 */
#if defined(I2C_PERIPHERAL) //TODO 当不同BSP支持不同数目的I2C总线时，应该将每一条总线都作为依赖条件
#define I2C_BASE_MODULE
#define AGNET_TASK_MAX_NUM 200
#endif


/**
 * PWM_PERIPHERAL 模块所需的PWM外设依赖条件
 * @variable pwmList 配置好的PWM通道列表，参照下面的结构体和声明，并且需要在源文件中定义 TODO 使用动态数组实现，可以简化这一过程
typedef struct {
    TIM_HandleTypeDef* timerPtr; // 定时器HAL对象指针
    uint32_t channel;              // 定时器通道
    uint32_t activeFlag=0;
}PWM_UNIT_t;
extern PWM_UNIT_t pwmList[7];
 */
#if defined(PWM_PERIPHERAL)
#define PWM_MODULE
#endif

/******************************************************************************************************
3.处理各组件对模块的依赖，以及是否被用户启用
 测试组件应该默认保持关闭状态
 ******************************************************************************************************/

//#define I2CTEST_ENABLED
#if defined(I2CTEST_ENABLED)&& defined(I2C_BASE_MODULE)
#define I2CTEST_COMPONENTS
#endif


#endif //FINEMOTE_PROJECTCONFIG_H
