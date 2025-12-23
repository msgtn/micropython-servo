#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include "port_handler.hpp"
#include "packet_handler.hpp"
#include "dynamixel_motor.hpp"

namespace dynamixel {

class DynamixelManager {
public:
    DynamixelManager(uart_inst_t* uart, uint tx_pin, uint rx_pin, uint32_t baudrate = 57600);
    ~DynamixelManager();

    // Initialize the port
    bool init();

    // Close the port
    void close();

    // Add a motor by ID
    DynamixelMotor* addMotor(uint8_t id, const std::string& name = "");

    // Get a motor by ID
    DynamixelMotor* getMotor(uint8_t id);

    // Check if motor exists
    bool hasMotor(uint8_t id) const;

    // Ping all motors
    bool pingAll();

    // Enable/disable torque for all motors
    bool enableAll();
    bool disableAll();

    // Get the port and packet handlers
    PortHandler& getPortHandler() { return port_; }
    PacketHandler& getPacketHandler() { return packet_; }

    // Iterator access to motors
    auto begin() { return motors_.begin(); }
    auto end() { return motors_.end(); }
    auto begin() const { return motors_.begin(); }
    auto end() const { return motors_.end(); }

private:
    PortHandler port_;
    PacketHandler packet_;
    uint32_t baudrate_;
    std::map<uint8_t, std::unique_ptr<DynamixelMotor>> motors_;
};

}  // namespace dynamixel
