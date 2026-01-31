#include "dynamixel_manager.hpp"

namespace dynamixel {

DynamixelManager::DynamixelManager(uart_inst_t* uart, uint tx_pin, uint rx_pin, uint32_t baudrate)
    : port_(uart, tx_pin, rx_pin)
    , packet_()
    , baudrate_(baudrate) {
}

DynamixelManager::~DynamixelManager() {
    close();
}

bool DynamixelManager::init() {
    if (!port_.openPort()) {
        return false;
    }
    return port_.setBaudRate(baudrate_);
}

void DynamixelManager::close() {
    port_.closePort();
}

DynamixelMotor* DynamixelManager::addMotor(uint8_t id, const std::string& name) {
    auto motor = std::make_unique<DynamixelMotor>(port_, packet_, id, name);
    DynamixelMotor* ptr = motor.get();
    motors_[id] = std::move(motor);
    return ptr;
}

DynamixelMotor* DynamixelManager::getMotor(uint8_t id) {
    auto it = motors_.find(id);
    if (it != motors_.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool DynamixelManager::hasMotor(uint8_t id) const {
    return motors_.find(id) != motors_.end();
}

bool DynamixelManager::pingAll() {
    bool success = true;
    for (auto& [id, motor] : motors_) {
        if (!motor->ping()) {
            success = false;
        }
    }
    return success;
}

bool DynamixelManager::enableAll() {
    bool success = true;
    for (auto& [id, motor] : motors_) {
        if (!motor->setTorqueEnable(true)) {
            success = false;
        }
    }
    return success;
}

bool DynamixelManager::disableAll() {
    bool success = true;
    for (auto& [id, motor] : motors_) {
        if (!motor->setTorqueEnable(false)) {
            success = false;
        }
    }
    return success;
}

void DynamixelManager::reconnectAll() {
    for (auto& [id, motor] : motors_) {
        motor->tryReconnect();
    }
}

}  // namespace dynamixel
