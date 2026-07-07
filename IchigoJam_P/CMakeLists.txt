cmake_minimum_required(VERSION 3.13)

# SDKの読込みは project() より前
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(IchigoJam_P C CXX ASM)

pico_sdk_init()

add_executable(IchigoJam_P
    src/main.c
)

target_include_directories(IchigoJam_P PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_compile_definitions(IchigoJam_P PRIVATE
    GIVETAKEJAM_NO_DVI=1
    GIVETAKEJAM_NO_SLEEP=1
)

target_link_libraries(IchigoJam_P PUBLIC
    pico_stdlib
    pico_multicore
    pico_util
    pico_sync
    tinyusb_host
    tinyusb_board
    hardware_dma
    hardware_flash
    hardware_watchdog
    hardware_adc
    hardware_pwm
    hardware_i2c
)

# USB CDCは現時点では使わず、従来どおりUART出力
pico_enable_stdio_uart(IchigoJam_P 1)
pico_enable_stdio_usb(IchigoJam_P 0)

# UF2 / ELF / BIN / HEX を生成
pico_add_extra_outputs(IchigoJam_P)
