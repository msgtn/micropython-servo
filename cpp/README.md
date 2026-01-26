# Servo Controller (C++ / Pico SDK)

C++ implementation of the MicroPython servo controller for Raspberry Pi Pico. Controls Dynamixel XL330-M288 servos and standard PWM servos.

## Prerequisites

1. **Pico SDK**: Install the Raspberry Pi Pico SDK
   ```bash
   git clone https://github.com/raspberrypi/pico-sdk.git
   cd pico-sdk
   git submodule update --init
   export PICO_SDK_PATH=/path/to/pico-sdk
   ```

2. **ARM Toolchain**: Install the ARM GCC toolchain
   ```bash
   # Ubuntu/Debian
   sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi

   # macOS (with Homebrew)
   brew install cmake arm-none-eabi-gcc
   ```

## Building

```bash
cd cpp
mkdir build

# PWM servos (default)
cmake -B build . && cmake --build build

# Dynamixel servos
cmake -B build -DUSE_DYNAMIXEL=ON . && cmake --build build

# Without distance sensor output
cmake -B build -DUSE_DISTANCE_SENSOR=OFF . && cmake --build build

# Dynamixel servos without distance sensor
cmake -B build -DUSE_DYNAMIXEL=ON -DUSE_DISTANCE_SENSOR=OFF . && cmake --build build

cd build
make -j4
```

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `USE_DYNAMIXEL` | OFF | Use Dynamixel servos instead of PWM servos |
| `USE_DISTANCE_SENSOR` | ON | Enable HC-SR04 distance sensor output |

This produces `servo_controller.uf2` in the build directory.

## Flashing

### Method 1: BOOTSEL (Manual)

1. Hold the **BOOTSEL** button on the Pico
2. Connect the Pico to USB while holding the button
3. Release the button - Pico mounts as a USB drive
4. Copy the UF2 file:
   ```bash
   cp servo_controller.uf2 /media/$USER/RPI-RP2/
   ```

The Pico will automatically reboot and run the firmware.

### Method 2: picotool (Recommended)

picotool allows flashing without manually entering BOOTSEL mode.

**Install picotool:**
```bash
# Ubuntu/Debian
sudo apt install picotool

# From source (if not in package manager)
git clone https://github.com/raspberrypi/picotool.git
cd picotool
mkdir build && cd build
cmake .. && make
sudo make install
```

**Load firmware:**
```bash
# Force device into BOOTSEL mode and load firmware
picotool load -f servo_controller.uf2

# Reboot into application mode after loading
picotool reboot
```

**Combined load and reboot:**
```bash
sudo picotool load -f servo_controller.uf2 && sudo picotool reboot -f
```

**Other useful commands:**
```bash
picotool info          # Show device info
picotool info -a       # Show all info including program details
picotool reboot -f -u  # Force reboot into BOOTSEL mode
```

Note: You may need `sudo` for picotool commands, or set up udev rules for non-root access.

## Pin Configuration

| Function | GPIO Pin | Notes |
|----------|----------|-------|
| UART1 TX (commands) | 4 | Command input from host |
| UART1 RX (commands) | 5 | 115200 baud |
| UART0 TX (Dynamixel) | 0 | Dynamixel bus |
| UART0 RX (Dynamixel) | 1 | 57600 baud |
| HC-SR04 Trigger | 14 | Distance sensor (optional) |
| HC-SR04 Echo | 15 | Use 1k resistor protection |
| LED | 25 | Status indicator |

## Command Protocol

Send commands via UART1 (pins 4/5) or USB serial in the format:
```
motor_id=position&motor_id=position...
```

Examples:
```
1=512                    # Set motor 1 to position 512
1=1024&2=2048&3=512     # Set multiple motors
```

For Dynamixel XL330-M288:
- Position range: 0-4095
- Center position: 2048

## Project Structure

```
cpp/
├── CMakeLists.txt          # Build configuration
├── pico_sdk_import.cmake   # Pico SDK import
├── README.md               # This file
└── src/
    ├── main.cpp            # Entry point
    ├── robot.hpp/cpp       # Robot classes
    ├── dynamixel/          # Dynamixel protocol
    │   ├── dynamixel_defs.hpp
    │   ├── port_handler.hpp/cpp
    │   ├── packet_handler.hpp/cpp
    │   ├── dynamixel_motor.hpp/cpp
    │   └── dynamixel_manager.hpp/cpp
    ├── servo/              # PWM servo
    │   └── pwm_servo.hpp/cpp
    └── sensors/            # Distance sensor
        └── hcsr04.hpp/cpp
```

## Distance Sensor

The HC-SR04 distance sensor is enabled by default. Distance readings (in mm) are output every 100ms via both USB serial and UART1.

To disable it:

```bash
cmake -B build -DUSE_DISTANCE_SENSOR=OFF . && cmake --build build
```

## Using PWM Servos Instead

To use standard PWM servos instead of Dynamixel, modify `main.cpp`:

```cpp
// Replace DynamixelRobot with Robot
Robot robot;
robot.addServo(1, 2);  // Motor ID 1 on GPIO 2
robot.addServo(2, 3);  // Motor ID 2 on GPIO 3
// etc.

// Commands use degrees (0-180) instead of Dynamixel positions
// "1=90" sets motor 1 to 90 degrees
```

## Differences from MicroPython Version

- **Single motor model**: Only XL330-M288 addresses are hardcoded (no JSON parsing)
- **No asyncio**: Uses straightforward polling loop
- **Performance**: Lower latency, more precise timing
- **Binary size**: ~100KB vs ~1MB+ for MicroPython
