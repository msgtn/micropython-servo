#pragma once

#include <cstdint>
#include "pico/types.h"

namespace servo {

class PWMServo {
public:
    PWMServo(uint pin_id, float min_us = 544.0f, float max_us = 2400.0f,
             float min_deg = 0.0f, float max_deg = 180.0f, uint32_t freq = 50);
    ~PWMServo();

    // Write angle in degrees (0-180)
    void write(float degrees);

    // Read current angle in degrees
    float read() const;

    // Write angle in radians
    void writeRad(float radians);

    // Read current angle in radians
    float readRad() const;

    // Write pulse width in microseconds
    void writeUs(float us);

    // Read current pulse width in microseconds
    float readUs() const;

    // Turn off the servo (stop sending PWM signal)
    void off();

    // Get the GPIO pin
    uint getPin() const { return pin_id_; }

private:
    uint pin_id_;
    uint slice_num_;
    uint channel_;
    float min_us_;
    float max_us_;
    float slope_;
    float offset_;
    float current_us_;
};

}  // namespace servo
