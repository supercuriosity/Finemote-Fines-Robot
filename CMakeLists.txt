#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
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
set(BOARD_NAME MC_Board)

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
#set(CMAKE_ASM_COMPILER armasm.exe)     # 指明 ASM 编译器，配合 第二种 CMAKE_ASM_FLAGS_INIT 方式使用
set(CMAKE_ASM_COMPILER_WORKS TRUE)

#设置链接器
set(CMAKE_C_LINK_EXECUTABLE armlink.exe)
set(CMAKE_ASM_LINK_EXECUTABLE armlink.exe)
set(CMAKE_CXX_LINK_EXECUTABLE armlink.exe)


#*******************************************************************************************#
# 2. 获取芯片地址描述信息
# 获取当前 MCU 的 section 描述，及存储空间和起始地址的描述（注意：需要先用 keil 打开编译一下当前项目才能生成）
# 设置Sections脚本路径,CubeMX生成 MDK 后先编译一下，就会在MDK-ARM 目录下生成工程名命名的文件夹
# 在那个目录里会生成以项目名命名的sct文件。 由于在本地调用，不需要加 global 全局变量标记，更简洁
set(SECTIONS_SCRIPT_PATH ${CMAKE_HOME_DIRECTORY}/Bsp/${BOARD_NAME}/MDK-ARM/${BOARD_NAME}/${BOARD_NAME}.sct)


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
set(CMAKE_C_FLAGS_INIT "${C_TARGET_FLAG} ${COMPILE_RULE_FLAG} \
    \
    -fno-rtti -funsigned-char -fshort-enums -fshort-wchar \
    -gdwarf-4 -Oz -ffunction-sections \
    -w")


# -xc -std=c11 --target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -c
#-fno-rtti -flto -funsigned-char -fshort-enums -fshort-wchar
#-gdwarf-4 -Os -ffunction-sections -Wno-packed -Wno-missing-variable-declarations -Wno-missing-prototypes -Wno-missing-noreturn -Wno-sign-conversion -Wno-nonportable-include-path -Wno-reserved-id-macro -Wno-unused-macros -Wno-documentation-unknown-command -Wno-documentation -Wno-license-management -Wno-parentheses-equality -Wno-reserved-identifier -I ../Inc -I ../Drivers/STM32F4xx_HAL_Driver/Inc -I ../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I ../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I ../Drivers/CMSIS/Include -I ../userCode/devices/Inc -I ../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I ../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I ../userCode/tasks/Inc -I ../userCode/algorithms/Inc -I ../userCode/bsp/Inc -I ../userCode/drivers/Inc -I ../userCode/MiddleWares/Inc
#-I./RTE/_RM_Frame_C
#-IC:/Users/xw/AppData/Local/Arm/Packs/ARM/CMSIS/5.9.0/CMSIS/Core/Include
#-IC:/Users/xw/AppData/Local/Arm/Packs/Keil/STM32F4xx_DFP/2.14.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include
#-D__UVISION_VERSION="538" -D_RTE_ -DSTM32F407xx -D_RTE_ -DUSE_HAL_DRIVER -DSTM32F407xx
#-o ./RM_Frame_C/*.o -MD

# 设置 C++ 编译器选项
set(CMAKE_CXX_FLAGS_INIT ${CMAKE_C_FLAGS_INIT})

# 设置ASM编译器选项
# 注意： -masm=auto 选项是 MDK 的 link 分页里没有的参数，需要加上  -g 是debug 用的加不加都行
set(CMAKE_ASM_FLAGS_INIT "${ASM_TARGET_FLAG} ${COMPILE_RULE_FLAG} \
        -masm=auto -c -gdwarf-4")
# 第二种方式 配套显示执行 armasm.exe 为 ASM 编译器的方法，看上起更清晰一些
#set(CMAKE_ASM_FLAGS_INIT "${ASM_TARGET_FLAG} --cpu=Cortex-M3")


# 设置链接器选项
# 这些参数再 ARMCC 文档里么有，但 MDK 的 link 分页有，
# 使用忽略所有警告的配置时没有 --map 及其之后的内容， 这里根据需要保留了一些信息，在 demo.map 中可以看到
set(CMAKE_EXE_LINKER_FLAGS_INIT " \
    ${LINKER_TARGET_FLAG} \
    --strict \
    --scatter ${SECTIONS_SCRIPT_PATH} \
    --info sizes --info totals --info unused --info veneers \
    --summary_stderr \
    --info summarysizes "
        )


# project settings
project(${PROJECT_NAME} C CXX ASM)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

#Uncomment for hardware floating point
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
#add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

#Uncomment for software floating point
#add_compile_options(-mfloat-abi=soft)


include_directories(
        Bsp/${BOARD_NAME}
        Bsp/${BOARD_NAME}/Drivers/CMSIS/Include
        Bsp/${BOARD_NAME}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
        Bsp/${BOARD_NAME}/Drivers/STM32F4xx_HAL_Driver/Inc
        Bsp/${BOARD_NAME}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
        Bsp/${BOARD_NAME}/Core/Inc

        Interface
        Devices
        Algorithms
        Services
        Components

        Tests
)

string(TOUPPER ${BOARD_NAME} UPPER_BOARD_NAME)
add_definitions(-DUSE_HAL_DRIVER -DSTM32F446xx -D__${UPPER_BOARD_NAME})

file(GLOB_RECURSE SOURCES
        "Bsp/${BOARD_NAME}/Drivers/*.*"
        "Bsp/${BOARD_NAME}/Core/*.*"
        "Bsp/${BOARD_NAME}/MDK-ARM/startup_*.s"
        )
# 对于混合兼容的环境，需要屏蔽各种编译环境引起的文件“干扰”,通过 list(REMOVE_ITEM) 命令移除不同编译环境下的干扰文件
# 在原来 CubeMX 自动生成的 gcc 编译环境目录上,附加 ARMCC 编译需要的文件
file(GLOB_RECURSE SOURCES ${SOURCES}
        "Interface/*.*"
        "Devices/*.*"
        "Algorithms/*.*"
        "Services/*.*"
        "Components/*.*"

        "Tests/*.*"
        )

# 将由 CubeMX 生成的 GCC 编译环境中的会干扰ARMCC环境的文件，放在 EXCLUDE_SRCS 自定义列表中
file(GLOB_RECURSE EXCLUDE_SRCS
        "Bsp/${BOARD_NAME}/Core/Startup/*.*"
        "Bsp/${BOARD_NAME}/Core/Src/syscalls.c"
        "Bsp/${BOARD_NAME}/Core/Src/sysmem.c"
        "Bsp/${BOARD_NAME}/STM32*_FLASH.ld"
        "Bsp/${BOARD_NAME}/STM32*_RAM.ld"
        )
# 从源文件列表(SOURCES)中剔除干扰文件(EXCLUDE_SRCS)
list(REMOVE_ITEM SOURCES ${EXCLUDE_SRCS})


add_executable(${BOARD_NAME} ${SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${BOARD_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${BOARD_NAME}.bin)
set(ELF_FILE ${PROJECT_BINARY_DIR}/${BOARD_NAME}.elf)

# 使用 armclang 自带的 fromelf 工具，实现 elf 转 hex
set(ARMCC_fromelf fromelf.exe)
add_custom_command(TARGET ${BOARD_NAME} POST_BUILD
        # 相当于fromelf.exe" --i32combined --output="xxx/demo.hex" "xxx/demo.elf"
        COMMAND ${ARMCC_fromelf} --i32combined --output="${HEX_FILE}" "${ELF_FILE}"
        COMMENT "Building ${HEX_FILE}"
        )

## 使用了 gcc 的 arm-none-eabi-objcopy 工具实现 elf 转 hex 和 bin， 跟 armclang 不是一脉
#set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${HEX_FILE}
#        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE}
#        COMMENT "Building ${HEX_FILE} Building ${BIN_FILE}")
