#pragma once

#include <cstdint>
#include "pico/types.h"

namespace sensors {

class HCSR04 {
public:
    // Default timeout based on sensor limit range (400cm)
    static constexpr uint32_t DEFAULT_TIMEOUT_US = 500 * 2 * 30;  // ~30000us

    HCSR04(uint trigger_pin, uint echo_pin, uint32_t timeout_us = DEFAULT_TIMEOUT_US);
    ~HCSR04() = default;

    // Get distance in millimeters (integer math, no floating point)
    int32_t distanceMm();

    // Get distance in centimeters (floating point)
    float distanceCm();

    // Get raw pulse time in microseconds
    int32_t getPulseTime();

private:
    int32_t sendPulseAndWait();

    uint trigger_pin_;
    uint echo_pin_;
    uint32_t timeout_us_;
};

}  // namespace sensors
