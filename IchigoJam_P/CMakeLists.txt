cmake_minimum_required(VERSION 3.13)

# SDKの読込みは project() より前
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(IchigoJam_P C CXX ASM)

pico_sdk_init()

if(PICO_PLATFORM MATCHES "^rp2350")
    # RP2350版はPicoDVIを使わず、HSTX DVI側を使う。
    set(GIVETAKEJAM_IS_RP2350 1)
else()
    # RP2040版は従来どおりPicoDVIを使う。
    set(GIVETAKEJAM_IS_RP2350 0)

    set(DVI_DEFAULT_SERIAL_CONFIG "pico_sock_cfg" CACHE STRING
        "Select a default pin configuration from common_dvi_pin_configs.h" FORCE)

    include_directories(
        PicoDVI/software/include
    )

    add_subdirectory(PicoDVI/software/libdvi)
    add_subdirectory(PicoDVI/software/libsprite)
endif()

add_executable(IchigoJam_P
    src/main.c
)

if(NOT PICO_PLATFORM MATCHES "^rp2350")
    pico_set_linker_script(IchigoJam_P ${CMAKE_CURRENT_LIST_DIR}/memmap_ichigojam_p.ld)
endif()

target_include_directories(IchigoJam_P PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/src
)

if(PICO_PLATFORM MATCHES "^rp2350")
    target_compile_definitions(IchigoJam_P PRIVATE
        GIVETAKEJAM_NO_DVI=1
        GIVETAKEJAM_NO_SLEEP=1
    )
else()
    target_compile_definitions(IchigoJam_P PRIVATE
        DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
    )
endif()

target_link_libraries(IchigoJam_P PUBLIC
    pico_stdlib
    pico_multicore
    pico_flash
    pico_util
    pico_sync
    tinyusb_host
    tinyusb_board
    hardware_dma
    hardware_flash
    hardware_watchdog
    hardware_adc
    hardware_pwm
    hardware_pio
    hardware_clocks
    hardware_i2c
    hardware_exception
)

if(NOT PICO_PLATFORM MATCHES "^rp2350")
    target_link_libraries(IchigoJam_P PUBLIC
        libdvi
        libsprite
        hardware_sleep
    )
endif()

# USB CDCは現時点では使わず、従来どおりUART出力
pico_enable_stdio_uart(IchigoJam_P 1)
pico_enable_stdio_usb(IchigoJam_P 0)

# UF2 / ELF / BIN / HEX を生成
pico_add_extra_outputs(IchigoJam_P)