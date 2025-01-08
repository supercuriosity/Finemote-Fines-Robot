#*******************************************************************************************#
# 该部分内容需要根据实际情况修改 Begin

# 更换BSP需要修改的部分
set(BOARD_NAME Robomaster_C)
set(CHIP_NAME STM32F407xx)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)

# FPU设置
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)

# 该部分内容需要根据实际情况修改 End
#*******************************************************************************************#
# 添加BSP文件

file(GLOB BSP_INCLUDE_DIRS
        "Drivers/CMSIS/Include"
        "Drivers/STM32F*xx_HAL_Driver/Inc"
        "Drivers/STM32F*xx_HAL_Driver/Inc/Legacy"
        "Drivers/CMSIS/Device/ST/STM32F*xx/Include"
        "Core/Inc"
)

include_directories(. ${BSP_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES ${FINEMOTE_COMMON_SOURCE_FILES}
        "Drivers/*.*"
        "Core/*.*"
        "MDK-ARM/startup_*.s"
        "./*.cpp"
)

file(GLOB_RECURSE EXCLUDE_SRCS
        "Core/Startup/*.*"
        "Core/Src/syscalls.c"
        "Core/Src/sysmem.c"
        "STM32*_FLASH.ld"
        "STM32*_RAM.ld"
)

# 从源文件列表(SOURCES)中剔除干扰文件(EXCLUDE_SRCS)
list(REMOVE_ITEM SOURCES ${EXCLUDE_SRCS})

#*******************************************************************************************#

# BSP宏定义
string(TOUPPER ${BOARD_NAME} UPPER_BOARD_NAME)
add_definitions(-DUSE_HAL_DRIVER -D${CHIP_NAME} -D__${UPPER_BOARD_NAME})

set(COMPILE_RULE_FLAG "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
set(LINKER_TARGET_FLAG "--cpu=${CMAKE_SYSTEM_PROCESSOR}")

#*******************************************************************************************#
# 设置编译选项
set(CMAKE_C_FLAGS "${C_TARGET_FLAG} ${COMPILE_RULE_FLAG} \
    -fno-rtti -funsigned-char -fshort-enums -fshort-wchar \
    -gdwarf-4 -Os -ffunction-sections \
    -Wno-packed -Wno-missing-variable-declarations -Wno-missing-prototypes -Wno-missing-noreturn -Wno-sign-conversion -Wno-nonportable-include-path -Wno-reserved-id-macro -Wno-unused-macros -Wno-documentation-unknown-command -Wno-documentation -Wno-license-management -Wno-parentheses-equality -Wno-reserved-identifier \
    ")
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_ASM_FLAGS "${ASM_TARGET_FLAG} ${COMPILE_RULE_FLAG} -masm=auto -c -gdwarf-4")

#*******************************************************************************************#
# 设置链接选项
set(SECTIONS_SCRIPT_PATH ${CMAKE_HOME_DIRECTORY}/Bsp/${BOARD_NAME}/MDK-ARM/${BOARD_NAME}/${BOARD_NAME}.sct)

set(CMAKE_EXE_LINKER_FLAGS " \
    ${LINKER_TARGET_FLAG} \
    --strict \
    --scatter ${SECTIONS_SCRIPT_PATH} \
    --info sizes --info totals --info unused --info veneers \
    --summary_stderr --info summarysizes --map --load_addr_map_info --xref --callgraph --symbols \
    --list ${BOARD_NAME}.map \
    " )

#*******************************************************************************************#
# 生成可执行文件
add_executable(${BOARD_NAME} ${SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${CMAKE_BINARY_DIR}/bin/${BOARD_NAME}.hex)
set(BIN_FILE ${CMAKE_BINARY_DIR}/bin/${BOARD_NAME}.bin)
set(ELF_FILE ${CMAKE_BINARY_DIR}/bin/${BOARD_NAME}.elf)

# 使用 armclang 自带的 fromelf 工具，实现 elf 转 hex
set(ARMCC_fromelf fromelf.exe)
add_custom_command(TARGET ${BOARD_NAME} POST_BUILD
        # 相当于fromelf.exe" --i32combined --output="xxx/demo.hex" "xxx/demo.elf"
        COMMAND ${ARMCC_fromelf} --i32combined --output="${HEX_FILE}" "${ELF_FILE}"
        COMMENT "Building ${HEX_FILE}"
)
