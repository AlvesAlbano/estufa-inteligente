# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(estufa-inteligente C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(estufa-inteligente 
estufa-inteligente.c
WIFI.c
MQTT.c
Oled.c
ssd1306.c
cJSON.c # https://github.com/DaveGamble/cJSON
joystick.c
botao.c
temperatura-interna.c
tomada-decisoes.c
flash.c
lfs.c
lfs_util.c
)

pico_set_program_name(estufa-inteligente "estufa-inteligente")
pico_set_program_version(estufa-inteligente "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(estufa-inteligente 1)
pico_enable_stdio_usb(estufa-inteligente 1)

# Add the standard library to the build
target_link_libraries(estufa-inteligente
        pico_stdlib
        )

# Add the standard include files to the build
target_include_directories(estufa-inteligente PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/headers
        ${CMAKE_CURRENT_LIST_DIR}/src/headers
        ${CMAKE_CURRENT_LIST_DIR}/libs/headers
)

target_link_libraries(estufa-inteligente 
        pico_stdlib
)

target_sources(estufa-inteligente PRIVATE
    ${PICO_LWIP_PATH}/src/apps/mqtt/mqtt.c
)

# Add any user requested libraries
target_link_libraries(estufa-inteligente 
        pico_cyw43_arch_lwip_threadsafe_background
        hardware_i2c
        hardware_adc
        hardware_pwm
        hardware_clocks
        )

pico_add_extra_outputs(estufa-inteligente)