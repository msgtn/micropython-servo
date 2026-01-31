#include "robot.hpp"
#include <algorithm>

// Robot class implementation (PWM servos)

void Robot::addServo(uint motor_id, uint pin_id) {
    motors_[motor_id] = std::make_unique<servo::PWMServo>(pin_id);
}

std::map<uint, float> Robot::readMotorStates() {
    std::map<uint, float> states;
    for (const auto& [id, servo] : motors_) {
        states[id] = servo->read();
    }
    return states;
}

void Robot::writeMotorStates(const std::map<int, float>& cmd_dict) {
    for (const auto& [id, value] : cmd_dict) {
        auto it = motors_.find(static_cast<uint>(id));
        if (it != motors_.end()) {
            // Clamp value to 0-180 degrees
            float clamped = std::max(0.0f, std::min(180.0f, value));
            it->second->write(clamped);
        }
    }
}

servo::PWMServo* Robot::getServo(uint motor_id) {
    auto it = motors_.find(motor_id);
    if (it != motors_.end()) {
        return it->second.get();
    }
    return nullptr;
}

// DynamixelRobot class implementation

DynamixelRobot::DynamixelRobot(uart_inst_t* uart, uint tx_pin, uint rx_pin, uint32_t baudrate)
    : manager_(uart, tx_pin, rx_pin, baudrate) {
}

bool DynamixelRobot::init() {
    return manager_.init();
}

void DynamixelRobot::addMotor(uint8_t motor_id) {
    auto* motor = manager_.addMotor(motor_id);
    if (motor) {
        motor->setTorqueEnable(true);
    }
}

void DynamixelRobot::writeMotorStates(const std::map<int, float>& msg_dict) {
    for (const auto& [motor_id, msg] : msg_dict) {
        auto* motor = manager_.getMotor(static_cast<uint8_t>(motor_id));
        if (motor) {
            motor->setGoalPosition(static_cast<uint32_t>(msg));
        }
    }
}

std::map<int, uint32_t> DynamixelRobot::readMotorStates() {
    std::map<int, uint32_t> states;
    for (auto& [id, motor] : manager_) {
        uint32_t position = 0;
        if (motor->getPresentPosition(position)) {
            states[id] = position;
        }
    }
    return states;
}

bool DynamixelRobot::enableAll() {
    return manager_.enableAll();
}

bool DynamixelRobot::disableAll() {
    return manager_.disableAll();
}

void DynamixelRobot::reconnectAll() {
    manager_.reconnectAll();
}
