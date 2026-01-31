#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include "servo/pwm_servo.hpp"
#include "dynamixel/dynamixel_manager.hpp"

// Robot class for PWM servos
class Robot {
public:
    Robot() = default;
    ~Robot() = default;

    // Add a servo on the specified GPIO pin
    void addServo(uint motor_id, uint pin_id);

    // Read all motor states (returns map of motor_id -> angle in degrees)
    std::map<uint, float> readMotorStates();

    // Write motor states (motor_id -> angle in degrees, 0-180)
    void writeMotorStates(const std::map<int, float>& cmd_dict);

    // Get a specific servo
    servo::PWMServo* getServo(uint motor_id);

private:
    std::map<uint, std::unique_ptr<servo::PWMServo>> motors_;
};

// DynamixelRobot class for Dynamixel servos
class DynamixelRobot {
public:
    DynamixelRobot(uart_inst_t* uart, uint tx_pin, uint rx_pin, uint32_t baudrate = 57600);
    ~DynamixelRobot() = default;

    // Initialize the robot
    bool init();

    // Add a motor by ID
    void addMotor(uint8_t motor_id);

    // Write motor states (motor_id -> goal position)
    void writeMotorStates(const std::map<int, float>& msg_dict);

    // Read motor states (motor_id -> present position)
    std::map<int, uint32_t> readMotorStates();

    // Enable/disable all motors
    bool enableAll();
    bool disableAll();

    // Try to reconnect all motors (blindly re-enable torque)
    void reconnectAll();

    // Get the underlying manager
    dynamixel::DynamixelManager& getManager() { return manager_; }

private:
    dynamixel::DynamixelManager manager_;
};
