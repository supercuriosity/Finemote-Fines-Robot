# 更换芯片需要修改的部分
set(BOARD_NAME Robomaster_C)        #对应Bsp包项目名，即开发板板名
set(CHIP_MODEL STM32F407xx)     #开发板使用的芯片型号，用于添加HAL使用的芯片定义


# 以下部分在不同芯片间通用
#*******************************************************************************************#
# 获取芯片地址描述信息
# 获取当前 MCU 的 section 描述，及存储空间和起始地址的描述（注意：需要先用 keil 打开编译一下当前项目才能生成）
# 设置Sections脚本路径,CubeMX生成 MDK 后先编译一下，就会在MDK-ARM 目录下生成工程名命名的文件夹
# 在那个目录里会生成以项目名命名的sct文件。 由于在本地调用，不需要加 global 全局变量标记，更简洁
set(SECTIONS_SCRIPT_PATH ${CMAKE_HOME_DIRECTORY}/Bsp/${BOARD_NAME}/MDK-ARM/${BOARD_NAME}/${BOARD_NAME}.sct)

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

include_directories(
        .
        Drivers/CMSIS/Include
        Drivers/CMSIS/Device/ST/STM32F4xx/Include
        Drivers/STM32F4xx_HAL_Driver/Inc
        Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
        Core/Inc
)

# 获取源文件，一并添加公共部分的源文件
file(GLOB_RECURSE SOURCES ${COMMON_SOURCE_FILES}
        "Drivers/*.*"
        "Core/*.*"
        "MDK-ARM/startup_*.s"
        "./*.cpp"
        )

# 将由 CubeMX 生成的 GCC 编译环境中的会干扰ARMCC环境的文件，放在 EXCLUDE_SRCS 自定义列表中
file(GLOB_RECURSE EXCLUDE_SRCS
        "Core/Startup/*.*"
        "Core/Src/syscalls.c"
        "Core/Src/sysmem.c"
        "STM32*_FLASH.ld"
        "STM32*_RAM.ld"
        )

# 从源文件列表(SOURCES)中剔除干扰文件(EXCLUDE_SRCS)
list(REMOVE_ITEM SOURCES ${EXCLUDE_SRCS})

add_executable(${BOARD_NAME} ${SOURCES} ${LINKER_SCRIPT})



string(TOUPPER ${BOARD_NAME} UPPER_BOARD_NAME)
add_definitions(-DUSE_HAL_DRIVER -D${CHIP_MODEL} -D__${UPPER_BOARD_NAME})


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

## 使用了 gcc 的 arm-none-eabi-objcopy 工具实现 elf 转 hex 和 bin， 跟 armclang 不是一脉
#set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${HEX_FILE}
#        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE}
#        COMMENT "Building ${HEX_FILE} Building ${BIN_FILE}")


