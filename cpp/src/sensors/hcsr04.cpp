#include "hcsr04.hpp"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

namespace sensors {

HCSR04::HCSR04(uint trigger_pin, uint echo_pin, uint32_t timeout_us)
    : trigger_pin_(trigger_pin)
    , echo_pin_(echo_pin)
    , timeout_us_(timeout_us) {

    // Initialize trigger pin as output
    gpio_init(trigger_pin_);
    gpio_set_dir(trigger_pin_, GPIO_OUT);
    gpio_put(trigger_pin_, 0);

    // Initialize echo pin as input
    gpio_init(echo_pin_);
    gpio_set_dir(echo_pin_, GPIO_IN);
}

int32_t HCSR04::sendPulseAndWait() {
    // Stabilize the sensor
    gpio_put(trigger_pin_, 0);
    sleep_us(5);

    // Send 10us trigger pulse
    gpio_put(trigger_pin_, 1);
    sleep_us(10);
    gpio_put(trigger_pin_, 0);

    // Wait for echo to go high (with timeout)
    uint64_t start_wait = time_us_64();
    while (!gpio_get(echo_pin_)) {
        if (time_us_64() - start_wait > timeout_us_) {
            // Timeout waiting for echo start
            return -1;
        }
    }

    // Measure pulse width (time echo stays high)
    uint64_t pulse_start = time_us_64();
    while (gpio_get(echo_pin_)) {
        if (time_us_64() - pulse_start > timeout_us_) {
            // Timeout during measurement - return max range
            constexpr int32_t MAX_RANGE_CM = 500;
            return static_cast<int32_t>(MAX_RANGE_CM * 29.1f);  // 1cm each 29.1us
        }
    }
    uint64_t pulse_end = time_us_64();

    return static_cast<int32_t>(pulse_end - pulse_start);
}

int32_t HCSR04::getPulseTime() {
    return sendPulseAndWait();
}

int32_t HCSR04::distanceMm() {
    int32_t pulse_time = sendPulseAndWait();

    if (pulse_time < 0) {
        return -1;  // Error
    }

    // To calculate the distance we get the pulse_time and divide it by 2
    // (the pulse walks the distance twice) and by 29.1 because
    // the sound speed on air (343.2 m/s), that's equivalent to
    // 0.34320 mm/us that is 1mm each 2.91us
    // pulse_time // 2 // 2.91 -> pulse_time // 5.82 -> pulse_time * 100 // 582
    return (pulse_time * 100) / 582;
}

float HCSR04::distanceCm() {
    int32_t pulse_time = sendPulseAndWait();

    if (pulse_time < 0) {
        return -1.0f;  // Error
    }

    // To calculate the distance we get the pulse_time and divide it by 2
    // (the pulse walks the distance twice) and by 29.1 because
    // the sound speed on air (343.2 m/s), that's equivalent to
    // 0.034320 cm/us that is 1cm each 29.1us
    return (static_cast<float>(pulse_time) / 2.0f) / 29.1f;
}

}  // namespace sensors
