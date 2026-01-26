#include "port_handler.hpp"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <cstring>

namespace dynamixel {

PortHandler::PortHandler(uart_inst_t* uart_instance, uint tx_pin, uint rx_pin)
    : uart_(uart_instance)
    , tx_pin_(tx_pin)
    , rx_pin_(rx_pin)
    , is_open_(false)
    , baudrate_(DEFAULT_BAUDRATE)
    , packet_start_time_(0.0)
    , packet_timeout_(0.0)
    , tx_time_per_byte_(0.0)
    , is_using_(false) {
}

PortHandler::~PortHandler() {
    closePort();
}

bool PortHandler::openPort() {
    return setBaudRate(baudrate_);
}

void PortHandler::closePort() {
    if (is_open_) {
        uart_deinit(uart_);
        is_open_ = false;
    }
}

void PortHandler::clearPort() {
    // Drain any pending bytes
    while (uart_is_readable(uart_)) {
        uart_getc(uart_);
    }
}

bool PortHandler::setBaudRate(uint32_t baudrate) {
    baudrate_ = baudrate;
    return setupPort(baudrate);
}

size_t PortHandler::readPort(uint8_t* buffer, size_t length) {
    size_t bytes_read = 0;

    while (bytes_read < length && uart_is_readable(uart_)) {
        buffer[bytes_read++] = uart_getc(uart_);
    }

    return bytes_read;
}

size_t PortHandler::writePort(const uint8_t* packet, size_t length) {
    uart_write_blocking(uart_, packet, length);
    // Wait for transmission to fully complete before switching to RX mode
    uart_tx_wait_blocking(uart_);
    return length;
}

void PortHandler::discardEcho(size_t length) {
    // On half-duplex bus, we receive our own transmission as echo
    // Read and discard exactly 'length' bytes with timeout
    size_t discarded = 0;
    uint64_t start = time_us_64();
    uint64_t timeout_us = (length * 200) + 5000;  // ~200us per byte at 57600 + margin

    while (discarded < length) {
        if (uart_is_readable(uart_)) {
            uart_getc(uart_);
            discarded++;
        } else if (time_us_64() - start > timeout_us) {
            break;  // Timeout - echo may not be present
        }
    }
}

void PortHandler::setPacketTimeout(size_t packet_length) {
    packet_start_time_ = getCurrentTime();
    packet_timeout_ = (tx_time_per_byte_ * static_cast<double>(packet_length))
                    + (LATENCY_TIMER * 2.0) + 2.0;
}

void PortHandler::setPacketTimeoutMillis(double msec) {
    packet_start_time_ = getCurrentTime();
    packet_timeout_ = msec;
}

bool PortHandler::isPacketTimeout() {
    if (getTimeSinceStart() > packet_timeout_) {
        packet_timeout_ = 0;
        return true;
    }
    return false;
}

double PortHandler::getCurrentTime() {
    return static_cast<double>(time_us_64()) / 1000.0;  // Convert to milliseconds
}

double PortHandler::getTimeSinceStart() {
    double time_since = getCurrentTime() - packet_start_time_;
    if (time_since < 0.0) {
        packet_start_time_ = getCurrentTime();
        time_since = 0.0;
    }
    return time_since;
}

bool PortHandler::setupPort(uint32_t baudrate) {
    // Initialize UART
    uart_init(uart_, baudrate);

    // Set up GPIO pins for UART
    gpio_set_function(tx_pin_, GPIO_FUNC_UART);
    gpio_set_function(rx_pin_, GPIO_FUNC_UART);

    // Set UART format: 8 data bits, 1 stop bit, no parity
    uart_set_format(uart_, 8, 1, UART_PARITY_NONE);

    // Enable UART FIFO
    uart_set_fifo_enabled(uart_, true);

    is_open_ = true;
    tx_time_per_byte_ = (1000.0 / static_cast<double>(baudrate)) * 10.0;

    return true;
}

}  // namespace dynamixel
