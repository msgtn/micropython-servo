#pragma once

#include <cstdint>
#include <cstddef>
#include "hardware/uart.h"

namespace dynamixel {

constexpr uint32_t LATENCY_TIMER = 16;
constexpr uint32_t DEFAULT_BAUDRATE = 1000000;

class PortHandler {
public:
    PortHandler(uart_inst_t* uart_instance, uint tx_pin, uint rx_pin);
    ~PortHandler();

    bool openPort();
    void closePort();
    void clearPort();

    bool setBaudRate(uint32_t baudrate);
    uint32_t getBaudRate() const { return baudrate_; }

    size_t readPort(uint8_t* buffer, size_t length);
    size_t writePort(const uint8_t* packet, size_t length);

    void setPacketTimeout(size_t packet_length);
    void setPacketTimeoutMillis(double msec);
    bool isPacketTimeout();

    bool isUsing() const { return is_using_; }
    void setUsing(bool using_port) { is_using_ = using_port; }

private:
    double getCurrentTime();
    double getTimeSinceStart();
    bool setupPort(uint32_t baudrate);

    uart_inst_t* uart_;
    uint tx_pin_;
    uint rx_pin_;
    bool is_open_;
    uint32_t baudrate_;
    double packet_start_time_;
    double packet_timeout_;
    double tx_time_per_byte_;
    bool is_using_;
};

}  // namespace dynamixel
