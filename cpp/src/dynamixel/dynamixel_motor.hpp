#pragma once

#include <cstdint>
#include <string>
#include "port_handler.hpp"
#include "packet_handler.hpp"
#include "dynamixel_defs.hpp"

namespace dynamixel {

class DynamixelMotor {
public:
    DynamixelMotor(PortHandler& port, PacketHandler& packet, uint8_t id, const std::string& name = "");
    ~DynamixelMotor() = default;

    uint8_t getId() const { return id_; }
    const std::string& getName() const { return name_; }

    // Connection state
    bool isConnected() const { return connected_; }
    void setConnected(bool connected) { connected_ = connected; }

    // Try to reconnect: ping and re-enable torque
    bool tryReconnect();

    // Ping the motor
    bool ping();

    // Torque control
    bool setTorqueEnable(bool enable);
    bool getTorqueEnable(bool& enabled);

    // LED control
    bool setLED(bool on);

    // Position control
    bool setGoalPosition(uint32_t position);
    bool getGoalPosition(uint32_t& position);
    bool getPresentPosition(uint32_t& position);

    // Velocity control
    bool setGoalVelocity(int32_t velocity);
    bool getPresentVelocity(int32_t& velocity);

    // Current control
    bool setGoalCurrent(int16_t current);
    bool getPresentCurrent(int16_t& current);

    // Profile settings
    bool setProfileAcceleration(uint32_t acceleration);
    bool setProfileVelocity(uint32_t velocity);

    // Status
    bool getPresentTemperature(uint8_t& temperature);
    bool getPresentVoltage(uint16_t& voltage);
    bool getHardwareErrorStatus(uint8_t& status);

    // Generic read/write using XL330 addresses
    bool readData(uint16_t address, uint8_t size, uint32_t& data);
    bool writeData(uint16_t address, uint8_t size, uint32_t data);

private:
    PortHandler& port_;
    PacketHandler& packet_;
    uint8_t id_;
    std::string name_;
    bool connected_ = false;
};

}  // namespace dynamixel
