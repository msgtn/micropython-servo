#include "pwm_servo.hpp"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include <cmath>

namespace servo {

// Convert degrees to radians
static inline float toRadians(float degrees) {
    return degrees * (M_PI / 180.0f);
}

// Convert radians to degrees
static inline float toDegrees(float radians) {
    return radians * (180.0f / M_PI);
}

PWMServo::PWMServo(uint pin_id, float min_us, float max_us,
                   float min_deg, float max_deg, uint32_t freq)
    : pin_id_(pin_id)
    , min_us_(min_us)
    , max_us_(max_us)
    , current_us_(0.0f) {

    // Calculate slope and offset for angle to pulse width conversion
    // Matches MicroPython: self._slope = (min_us-max_us)/(math.radians(min_deg)-math.radians(max_deg))
    float min_rad = toRadians(min_deg);
    float max_rad = toRadians(max_deg);
    slope_ = (min_us - max_us) / (min_rad - max_rad);
    offset_ = min_us;

    // Set up GPIO for PWM function
    gpio_set_function(pin_id_, GPIO_FUNC_PWM);

    // Get PWM slice and channel for this GPIO
    slice_num_ = pwm_gpio_to_slice_num(pin_id_);
    channel_ = pwm_gpio_to_channel(pin_id_);

    // Configure PWM for servo control
    // Matches MicroPython: self.pwm.freq(freq)
    // We use 1us resolution: wrap = 1000000 / freq (e.g., 20000 for 50Hz)
    // Clock divider = sys_clk / 1MHz to get 1us per count
    uint32_t sys_clk = clock_get_hz(clk_sys);
    float divider = static_cast<float>(sys_clk) / 1000000.0f;
    uint16_t wrap = 1000000 / freq;

    pwm_set_clkdiv(slice_num_, divider);
    pwm_set_wrap(slice_num_, wrap - 1);

    // Start with PWM disabled (0 duty cycle)
    pwm_set_chan_level(slice_num_, channel_, 0);

    // Enable PWM
    pwm_set_enabled(slice_num_, true);
}

PWMServo::~PWMServo() {
    off();
    pwm_set_enabled(slice_num_, false);
}

void PWMServo::write(float degrees) {
    writeRad(toRadians(degrees));
}

float PWMServo::read() const {
    return toDegrees(readRad());
}

void PWMServo::writeRad(float radians) {
    writeUs(radians * slope_ + offset_);
}

float PWMServo::readRad() const {
    return (current_us_ - offset_) / slope_;
}

void PWMServo::writeUs(float us) {
    current_us_ = us;
    // Matches MicroPython: self.pwm.duty_ns(int(self.current_us*1000.0))
    // With 1us resolution, level = us directly (equivalent to duty_ns / 1000)
    uint16_t level = static_cast<uint16_t>(us);
    pwm_set_chan_level(slice_num_, channel_, level);
}

float PWMServo::readUs() const {
    return current_us_;
}

void PWMServo::off() {
    pwm_set_chan_level(slice_num_, channel_, 0);
}

}  // namespace servo
