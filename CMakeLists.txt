set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.21)

#*************************************************************
# 1. 设定项目名称
# 需要给定 PROJECT_NAME 的值，必须与项目名称一致，否则 MDK 生成 *.sct 时路径不对
set(PROJECT_NAME FineMote)
# 2. 设定芯片类型
# 设定芯片类型，必须正确以 -mcpu 的参数规则命名，后边的编译参数会调用这个值
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)

############################################################
# 1. 指定编译器和链接器，避免使用默认的 gcc
############################################################
# 设置编译器 C 的编译器
set(CMAKE_C_COMPILER armclang.exe)
set(CMAKE_C_COMPILER_WORKS TRUE)

# 即便用不到C++ 的编译器，还是要显示说明的,否则报错
set(CMAKE_CXX_COMPILER armclang.exe)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

#设置 ASM 的编译器（不设置配合 -masm=auto 使用）
set(CMAKE_ASM_COMPILER armclang.exe)
set(CMAKE_ASM_COMPILER_WORKS TRUE)

#设置链接器
set(CMAKE_C_LINK_EXECUTABLE armlink.exe)
set(CMAKE_ASM_LINK_EXECUTABLE armlink.exe)
set(CMAKE_CXX_LINK_EXECUTABLE armlink.exe)


#*******************************************************************************************#
# 2. 此部分移动到子目录中(获取芯片地址描述信息)
# 获取当前 MCU 的 section 描述，及存储空间和起始地址的描述（注意：需要先用 keil 打开编译一下当前项目才能生成）
# 设置Sections脚本路径,CubeMX生成 MDK 后先编译一下，就会在MDK-ARM 目录下生成工程名命名的文件夹
# 在那个目录里会生成以项目名命名的sct文件。 由于在本地调用，不需要加 global 全局变量标记，更简洁
# set(SECTIONS_SCRIPT_PATH ${CMAKE_HOME_DIRECTORY}/BSP/${BOARD_NAME}/MDK-ARM/${BOARD_NAME}/${BOARD_NAME}.sct)


#********************************************************************************************#
# 3. 设置与芯片对应的 --target 编译选项
# 设置编译时默认参数片段，这些参数参见 MDK 中的配置，注意：Target 分页中要选 6.12 的 ARM 编译器,并抄各分页的 --target 配置
set(C_TARGET_FLAG --target=arm-arm-none-eabi)               # MDK 的 link 分页中的配置
set(CXX_TARGET_FLAG --target=arm-arm-none-eabi)             # MDK 的 link 分页中的配置
set(ASM_TARGET_FLAG --target=arm-arm-none-eabi)             # MDK 的 link 分页中的配置(不支持显示指定 ASM 编译器的方式)
set(LINKER_TARGET_FLAG --cpu=${CMAKE_SYSTEM_PROCESSOR})     # MDK 的 link 分页中的配置


#********************************************************************************************#
# 4. 设置编译规则，这些规则变量要被外部的 CMakelistes.txt 调用，所以需要用 CACHE STRING "global" 声明为全局变量
# 编译参数，这些编译参数是从 MDK 中来的，具体的含义看 《 armclang Reference Guide 》 第一章节有详细介绍
# 需要注意的是不是 MDK 中的每个参数都要哪进来的，有些参数已经在 include 和 source 中被配置了
set(COMPILE_RULE_FLAG "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")

# 设置 C 编译器选项(这里就把MDK中的 C/C++ 分页里最下边一栏的属性贴进来)
# 参数 -w 表示忽略所有警告，不然要配具体忽略哪些警告，尽管贴过来也行，但是太乱
# 优化选项 -O 有 1~3   -Os 是平衡  -Oz 是最小体积
set(CMAKE_C_FLAGS "${C_TARGET_FLAG} ${COMPILE_RULE_FLAG} \
    \
    -fno-rtti -funsigned-char -fshort-enums -fshort-wchar \
    -gdwarf-4 -O0 -ffunction-sections \
    -Wno-packed -Wno-missing-variable-declarations -Wno-missing-prototypes -Wno-missing-noreturn -Wno-sign-conversion -Wno-nonportable-include-path -Wno-reserved-id-macro -Wno-unused-macros -Wno-documentation-unknown-command -Wno-documentation -Wno-license-management -Wno-parentheses-equality -Wno-reserved-identifier \
    ")

# 设置 C++ 编译器选项
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})


set(CMAKE_ASM_FLAGS_INIT "${ASM_TARGET_FLAG} ${COMPILE_RULE_FLAG} \
        -masm=auto -c -gdwarf-4")



# project settings
project(${PROJECT_NAME} C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# 设置可执行文件和库文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib) # 对于静态库

#Uncomment for hardware floating point
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
#add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

#Uncomment for software floating point
#add_compile_options(-mfloat-abi=soft)

function(INCLUDE_SUB_DIR_INC root_dir)
        if(IS_DIRECTORY ${root_dir})
                #找含有.h文件或含有.hpp文件，但该目录不一定是Inc或include目录,则将当前目录添加到头文件搜索路径
                file(GLOB_RECURSE head_files ${root_dir}/*.h ${root_dir}/*.hpp)
                if(head_files)
                include_directories(${root_dir})
                message(STATUS "Add head file search path: ${root_dir}")
                endif()
                #递归寻找子目录
                file(GLOB sub_directories RELATIVE ${root_dir} ${root_dir}/*)
                foreach(sub_dir ${sub_directories})
                        INCLUDE_SUB_DIR_INC(${root_dir}/${sub_dir})
                endforeach()
        endif()
endfunction()

INCLUDE_SUB_DIR_INC(${PROJECT_SOURCE_DIR}/Interface)
INCLUDE_SUB_DIR_INC(${PROJECT_SOURCE_DIR}/Devices)
INCLUDE_SUB_DIR_INC(${PROJECT_SOURCE_DIR}/Algorithms)
INCLUDE_SUB_DIR_INC(${PROJECT_SOURCE_DIR}/Services)
INCLUDE_SUB_DIR_INC(${PROJECT_SOURCE_DIR}/Components)
INCLUDE_SUB_DIR_INC(${PROJECT_SOURCE_DIR}/Tests)

#include_directories(
#        Interface
#        Devices
#        Algorithms
#        Services
#        Components
#        Tests
#)

# 对于混合兼容的环境，需要屏蔽各种编译环境引起的文件“干扰”,通过 list(REMOVE_ITEM) 命令移除不同编译环境下的干扰文件
# 获取不同芯片的公共代码，于子模块中调用此变量
file(GLOB_RECURSE FINEMOTE_SOURCE_FILES
        "Interface/*.*"
        "Devices/*.*"
        "Algorithms/*.*"
        "Services/*.*"
        "Components/*.*"
        "Tests/*.*"
        )

include_directories(ThirdParty/CMSIS-DSP/Include)
include_directories(ThirdParty/CMSIS-DSP/Include/dsp)
include_directories(ThirdParty/CMSIS-DSP/PrivateInclude)

file(GLOB_RECURSE CMSIS_DSP_SOURCE_FILES
        "ThirdParty/CMSIS-DSP/Source/BasicMathFunctions/BasicMathFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/BayesFunctions/BayesFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/CommonTables/CommonTables.c"
        "ThirdParty/CMSIS-DSP/Source/ComplexMathFunctions/ComplexMathFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/ControllerFunctions/ControllerFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/DistanceFunctions/DistanceFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/FastMathFunctions/FastMathFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/FilteringFunctions/FilteringFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/InterpolationFunctions/InterpolationFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/MatrixFunctions/MatrixFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/QuaternionMathFunctions/QuaternionMathFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/StatisticsFunctions/StatisticsFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/SupportFunctions/SupportFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/SVMFunctions/SVMFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/TransformFunctions/TransformFunctions.c"
        "ThirdParty/CMSIS-DSP/Source/WindowFunctions/WindowFunctions.c"
)

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
file(GLOB_RECURSE CMSIS_DSP_SOURCE_FILES ${CMSIS_DSP_SOURCE_FILES}
        "ThirdParty/CMSIS-DSP/Source/BasicMathFunctions/BasicMathFunctionsF16.c"
        "ThirdParty/CMSIS-DSP/Source/BayesFunctions/BayesFunctionsF16.c"
        "ThirdParty/CMSIS-DSP/Source/CommonTables/CommonTablesF16.c"
        "ThirdParty/CMSIS-DSP/Source/ComplexMathFunctions/ComplexMathFunctionsF16.c"
        "ThirdParty/CMSIS-DSP/Source/DistanceFunctions/DistanceFunctionsF16.c"
        "ThirdParty/CMSIS-DSP/Source/FastMathFunctions/FastMathFunctionsF16.c"
        "ThirdParty/CMSIS-DSP/Source/FilteringFunctions/FilteringFunctionsF16.c"
        "ThirdParty/CMSIS-DSP/Source/InterpolationFunctions/InterpolationFunctionsF16.c"
        "ThirdParty/CMSIS-DSP/Source/MatrixFunctions/MatrixFunctionsF16.c"
        "ThirdParty/CMSIS-DSP/Source/StatisticsFunctions/StatisticsFunctionsF16.c"
        "ThirdParty/CMSIS-DSP/Source/SupportFunctions/SupportFunctionsF16.c"
        "ThirdParty/CMSIS-DSP/Source/SVMFunctions/SVMFunctionsF16.c"
        "ThirdParty/CMSIS-DSP/Source/TransformFunctions/TransformFunctionsF16.c"
)
endif()

file(GLOB_RECURSE COMMON_SOURCE_FILES ${FINEMOTE_SOURCE_FILES} ${CMSIS_DSP_SOURCE_FILES})

add_subdirectory(BSP/MC_Board)
add_subdirectory(BSP/Robomaster_C)
