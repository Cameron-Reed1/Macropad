# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
# set(PICO_SDK_PATH "/home/cameron/makerstuff/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Macropad C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include("libraries/pico_neopixels/pico_neopixel.cmake")
include("libraries/SH1106/SH1106_oled.cmake")

# Add executable. Default name is the project name, version 0.1

add_executable(Macropad src/main.cpp src/Macropad.cpp src/MacropadState.cpp src/MacropadUsbCallbacks.cpp src/RotaryEncoder.cpp src/Keys.cpp src/RippleAnim.cpp src/usb_descriptors.c src/Macro.cpp src/Config.cpp src/states/common.cpp src/states/ConfigState.cpp src/states/DebugState.cpp src/states/GenericState.cpp src/states/LinkState.cpp src/states/MenuState.cpp src/states/PowerOffState.cpp src/states/TeamsState.cpp src/states/TimerState.cpp)

pico_set_program_name(Macropad "Macropad")
pico_set_program_version(Macropad "0.3")

pico_enable_stdio_uart(Macropad 0)
pico_enable_stdio_usb(Macropad 0)


# Add any user requested libraries
target_link_libraries(Macropad
        pico_stdlib
        pico_malloc
        pico_mem_ops
        pico_bootrom
        pico_neopixel
        pico_multicore
        pico_usb_reset_interface
        hardware_i2c
        hardware_pwm
        hardware_spi
        hardware_sync
        hardware_flash
        hardware_watchdog
        SH1106_oled
        tinyusb_device
        tinyusb_board
        )

target_include_directories(Macropad PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

pico_add_extra_outputs(Macropad)

install(CODE "execute_process(COMMAND ${CMAKE_SOURCE_DIR}/install.sh \"${CMAKE_BINARY_DIR}\")")
