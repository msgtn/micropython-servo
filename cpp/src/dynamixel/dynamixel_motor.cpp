#include "dynamixel_motor.hpp"

namespace dynamixel {

DynamixelMotor::DynamixelMotor(PortHandler& port, PacketHandler& packet, uint8_t id, const std::string& name)
    : port_(port)
    , packet_(packet)
    , id_(id)
    , name_(name.empty() ? "motor_" + std::to_string(id) : name) {
}

bool DynamixelMotor::ping() {
    uint16_t model_number = 0;
    uint8_t error = 0;
    int result = packet_.ping(port_, id_, model_number, error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::setTorqueEnable(bool enable) {
    uint8_t error = 0;
    int result = packet_.write1ByteTxRx(port_, id_, xl330::ADDR_TORQUE_ENABLE,
                                        enable ? 1 : 0, error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::getTorqueEnable(bool& enabled) {
    uint8_t data = 0;
    uint8_t error = 0;
    int result = packet_.read1ByteTxRx(port_, id_, xl330::ADDR_TORQUE_ENABLE, data, error);
    if (result == COMM_SUCCESS && error == 0) {
        enabled = (data != 0);
        return true;
    }
    return false;
}

bool DynamixelMotor::setLED(bool on) {
    uint8_t error = 0;
    int result = packet_.write1ByteTxRx(port_, id_, xl330::ADDR_LED, on ? 1 : 0, error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::setGoalPosition(uint32_t position) {
    uint8_t error = 0;
    int result = packet_.write4ByteTxRx(port_, id_, xl330::ADDR_GOAL_POSITION, position, error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::getGoalPosition(uint32_t& position) {
    uint8_t error = 0;
    int result = packet_.read4ByteTxRx(port_, id_, xl330::ADDR_GOAL_POSITION, position, error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::getPresentPosition(uint32_t& position) {
    uint8_t error = 0;
    int result = packet_.read4ByteTxRx(port_, id_, xl330::ADDR_PRESENT_POSITION, position, error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::setGoalVelocity(int32_t velocity) {
    uint8_t error = 0;
    int result = packet_.write4ByteTxRx(port_, id_, xl330::ADDR_GOAL_VELOCITY,
                                        static_cast<uint32_t>(velocity), error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::getPresentVelocity(int32_t& velocity) {
    uint32_t data = 0;
    uint8_t error = 0;
    int result = packet_.read4ByteTxRx(port_, id_, xl330::ADDR_PRESENT_VELOCITY, data, error);
    if (result == COMM_SUCCESS && error == 0) {
        velocity = static_cast<int32_t>(data);
        return true;
    }
    return false;
}

bool DynamixelMotor::setGoalCurrent(int16_t current) {
    uint8_t error = 0;
    int result = packet_.write2ByteTxRx(port_, id_, xl330::ADDR_GOAL_CURRENT,
                                        static_cast<uint16_t>(current), error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::getPresentCurrent(int16_t& current) {
    uint16_t data = 0;
    uint8_t error = 0;
    int result = packet_.read2ByteTxRx(port_, id_, xl330::ADDR_PRESENT_CURRENT, data, error);
    if (result == COMM_SUCCESS && error == 0) {
        current = static_cast<int16_t>(data);
        return true;
    }
    return false;
}

bool DynamixelMotor::setProfileAcceleration(uint32_t acceleration) {
    uint8_t error = 0;
    int result = packet_.write4ByteTxRx(port_, id_, xl330::ADDR_PROFILE_ACCELERATION,
                                        acceleration, error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::setProfileVelocity(uint32_t velocity) {
    uint8_t error = 0;
    int result = packet_.write4ByteTxRx(port_, id_, xl330::ADDR_PROFILE_VELOCITY, velocity, error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::getPresentTemperature(uint8_t& temperature) {
    uint8_t error = 0;
    int result = packet_.read1ByteTxRx(port_, id_, xl330::ADDR_PRESENT_TEMPERATURE,
                                       temperature, error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::getPresentVoltage(uint16_t& voltage) {
    uint8_t error = 0;
    int result = packet_.read2ByteTxRx(port_, id_, xl330::ADDR_PRESENT_INPUT_VOLTAGE,
                                       voltage, error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::getHardwareErrorStatus(uint8_t& status) {
    uint8_t error = 0;
    int result = packet_.read1ByteTxRx(port_, id_, xl330::ADDR_HARDWARE_ERROR_STATUS,
                                       status, error);
    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::readData(uint16_t address, uint8_t size, uint32_t& data) {
    uint8_t error = 0;
    int result;

    switch (size) {
        case 1: {
            uint8_t val = 0;
            result = packet_.read1ByteTxRx(port_, id_, address, val, error);
            data = val;
            break;
        }
        case 2: {
            uint16_t val = 0;
            result = packet_.read2ByteTxRx(port_, id_, address, val, error);
            data = val;
            break;
        }
        case 4:
            result = packet_.read4ByteTxRx(port_, id_, address, data, error);
            break;
        default:
            return false;
    }

    return (result == COMM_SUCCESS && error == 0);
}

bool DynamixelMotor::writeData(uint16_t address, uint8_t size, uint32_t data) {
    uint8_t error = 0;
    int result;

    switch (size) {
        case 1:
            result = packet_.write1ByteTxRx(port_, id_, address,
                                            static_cast<uint8_t>(data), error);
            break;
        case 2:
            result = packet_.write2ByteTxRx(port_, id_, address,
                                            static_cast<uint16_t>(data), error);
            break;
        case 4:
            result = packet_.write4ByteTxRx(port_, id_, address, data, error);
            break;
        default:
            return false;
    }

    return (result == COMM_SUCCESS && error == 0);
}

}  // namespace dynamixel
