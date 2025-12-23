cmake_minimum_required(VERSION 3.13)

# Include the Pico SDK
include(pico_sdk_import.cmake)

project(servo_controller C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# Add executable with all source files
add_executable(servo_controller
    src/main.cpp
    src/robot.cpp
    src/dynamixel/port_handler.cpp
    src/dynamixel/packet_handler.cpp
    src/dynamixel/dynamixel_motor.cpp
    src/dynamixel/dynamixel_manager.cpp
    src/servo/pwm_servo.cpp
    src/sensors/hcsr04.cpp
)

# Include directories
target_include_directories(servo_controller PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link required libraries
target_link_libraries(servo_controller
    pico_stdlib
    hardware_uart
    hardware_pwm
    hardware_gpio
)

# Enable USB output, disable UART output for stdio
pico_enable_stdio_usb(servo_controller 1)
pico_enable_stdio_uart(servo_controller 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(servo_controller)
