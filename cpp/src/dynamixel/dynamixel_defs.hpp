#pragma once

#include <cstdint>

namespace dynamixel {

// Broadcast ID
constexpr uint8_t BROADCAST_ID = 0xFE;  // 254
constexpr uint8_t MAX_ID = 0xFC;        // 252

// Instruction for DXL Protocol
constexpr uint8_t INST_PING = 1;
constexpr uint8_t INST_READ = 2;
constexpr uint8_t INST_WRITE = 3;
constexpr uint8_t INST_REG_WRITE = 4;
constexpr uint8_t INST_ACTION = 5;
constexpr uint8_t INST_FACTORY_RESET = 6;
constexpr uint8_t INST_CLEAR = 16;
constexpr uint8_t INST_SYNC_WRITE = 131;  // 0x83
constexpr uint8_t INST_BULK_READ = 146;   // 0x92
// Protocol 2.0 only
constexpr uint8_t INST_REBOOT = 8;
constexpr uint8_t INST_STATUS = 85;       // 0x55
constexpr uint8_t INST_SYNC_READ = 130;   // 0x82
constexpr uint8_t INST_BULK_WRITE = 147;  // 0x93

// Communication Result
constexpr int COMM_SUCCESS = 0;
constexpr int COMM_PORT_BUSY = -1000;
constexpr int COMM_TX_FAIL = -1001;
constexpr int COMM_RX_FAIL = -1002;
constexpr int COMM_TX_ERROR = -2000;
constexpr int COMM_RX_WAITING = -3000;
constexpr int COMM_RX_TIMEOUT = -3001;
constexpr int COMM_RX_CORRUPT = -3002;
constexpr int COMM_NOT_AVAILABLE = -9000;

// Packet buffer sizes
constexpr size_t TXPACKET_MAX_LEN = 1024;
constexpr size_t RXPACKET_MAX_LEN = 1024;

// Protocol 2.0 Packet structure indices
constexpr uint8_t PKT_HEADER0 = 0;
constexpr uint8_t PKT_HEADER1 = 1;
constexpr uint8_t PKT_HEADER2 = 2;
constexpr uint8_t PKT_RESERVED = 3;
constexpr uint8_t PKT_ID = 4;
constexpr uint8_t PKT_LENGTH_L = 5;
constexpr uint8_t PKT_LENGTH_H = 6;
constexpr uint8_t PKT_INSTRUCTION = 7;
constexpr uint8_t PKT_ERROR = 8;
constexpr uint8_t PKT_PARAMETER0 = 8;

// Protocol 2.0 Error bits
constexpr uint8_t ERRNUM_RESULT_FAIL = 1;
constexpr uint8_t ERRNUM_INSTRUCTION = 2;
constexpr uint8_t ERRNUM_CRC = 3;
constexpr uint8_t ERRNUM_DATA_RANGE = 4;
constexpr uint8_t ERRNUM_DATA_LENGTH = 5;
constexpr uint8_t ERRNUM_DATA_LIMIT = 6;
constexpr uint8_t ERRNUM_ACCESS = 7;
constexpr uint8_t ERRBIT_ALERT = 128;

// Byte manipulation macros
inline uint16_t DXL_MAKEWORD(uint8_t a, uint8_t b) {
    return static_cast<uint16_t>((a & 0xFF) | ((b & 0xFF) << 8));
}

inline uint32_t DXL_MAKEDWORD(uint16_t a, uint16_t b) {
    return static_cast<uint32_t>((a & 0xFFFF) | ((b & 0xFFFF) << 16));
}

inline uint16_t DXL_LOWORD(uint32_t l) {
    return static_cast<uint16_t>(l & 0xFFFF);
}

inline uint16_t DXL_HIWORD(uint32_t l) {
    return static_cast<uint16_t>((l >> 16) & 0xFFFF);
}

inline uint8_t DXL_LOBYTE(uint16_t w) {
    return static_cast<uint8_t>(w & 0xFF);
}

inline uint8_t DXL_HIBYTE(uint16_t w) {
    return static_cast<uint8_t>((w >> 8) & 0xFF);
}

// XL330-M288 Control Table Addresses
namespace xl330 {

// EEPROM Area
constexpr uint16_t ADDR_MODEL_NUMBER = 0;
constexpr uint16_t ADDR_MODEL_INFORMATION = 2;
constexpr uint16_t ADDR_FIRMWARE_VERSION = 6;
constexpr uint16_t ADDR_ID = 7;
constexpr uint16_t ADDR_BAUD_RATE = 8;
constexpr uint16_t ADDR_RETURN_DELAY_TIME = 9;
constexpr uint16_t ADDR_DRIVE_MODE = 10;
constexpr uint16_t ADDR_OPERATING_MODE = 11;
constexpr uint16_t ADDR_SECONDARY_ID = 12;
constexpr uint16_t ADDR_PROTOCOL_TYPE = 13;
constexpr uint16_t ADDR_HOMING_OFFSET = 20;
constexpr uint16_t ADDR_MOVING_THRESHOLD = 24;
constexpr uint16_t ADDR_TEMPERATURE_LIMIT = 31;
constexpr uint16_t ADDR_MAX_VOLTAGE_LIMIT = 32;
constexpr uint16_t ADDR_MIN_VOLTAGE_LIMIT = 34;
constexpr uint16_t ADDR_PWM_LIMIT = 36;
constexpr uint16_t ADDR_CURRENT_LIMIT = 38;
constexpr uint16_t ADDR_VELOCITY_LIMIT = 44;
constexpr uint16_t ADDR_MAX_POSITION_LIMIT = 48;
constexpr uint16_t ADDR_MIN_POSITION_LIMIT = 52;
constexpr uint16_t ADDR_STARTUP_CONFIGURATION = 60;
constexpr uint16_t ADDR_PWM_SLOPE = 62;
constexpr uint16_t ADDR_SHUTDOWN = 63;

// RAM Area
constexpr uint16_t ADDR_TORQUE_ENABLE = 64;
constexpr uint16_t ADDR_LED = 65;
constexpr uint16_t ADDR_STATUS_RETURN_LEVEL = 68;
constexpr uint16_t ADDR_REGISTERED_INSTRUCTION = 69;
constexpr uint16_t ADDR_HARDWARE_ERROR_STATUS = 70;
constexpr uint16_t ADDR_VELOCITY_I_GAIN = 76;
constexpr uint16_t ADDR_VELOCITY_P_GAIN = 78;
constexpr uint16_t ADDR_POSITION_D_GAIN = 80;
constexpr uint16_t ADDR_POSITION_I_GAIN = 82;
constexpr uint16_t ADDR_POSITION_P_GAIN = 84;
constexpr uint16_t ADDR_FEEDFORWARD_2ND_GAIN = 88;
constexpr uint16_t ADDR_FEEDFORWARD_1ST_GAIN = 90;
constexpr uint16_t ADDR_BUS_WATCHDOG = 98;
constexpr uint16_t ADDR_GOAL_PWM = 100;
constexpr uint16_t ADDR_GOAL_CURRENT = 102;
constexpr uint16_t ADDR_GOAL_VELOCITY = 104;
constexpr uint16_t ADDR_PROFILE_ACCELERATION = 108;
constexpr uint16_t ADDR_PROFILE_VELOCITY = 112;
constexpr uint16_t ADDR_GOAL_POSITION = 116;
constexpr uint16_t ADDR_REALTIME_TICK = 120;
constexpr uint16_t ADDR_MOVING = 122;
constexpr uint16_t ADDR_MOVING_STATUS = 123;
constexpr uint16_t ADDR_PRESENT_PWM = 124;
constexpr uint16_t ADDR_PRESENT_CURRENT = 126;
constexpr uint16_t ADDR_PRESENT_VELOCITY = 128;
constexpr uint16_t ADDR_PRESENT_POSITION = 132;
constexpr uint16_t ADDR_VELOCITY_TRAJECTORY = 136;
constexpr uint16_t ADDR_POSITION_TRAJECTORY = 140;
constexpr uint16_t ADDR_PRESENT_INPUT_VOLTAGE = 144;
constexpr uint16_t ADDR_PRESENT_TEMPERATURE = 146;

// Data sizes
constexpr uint8_t SIZE_MODEL_NUMBER = 2;
constexpr uint8_t SIZE_MODEL_INFORMATION = 4;
constexpr uint8_t SIZE_FIRMWARE_VERSION = 1;
constexpr uint8_t SIZE_ID = 1;
constexpr uint8_t SIZE_BAUD_RATE = 1;
constexpr uint8_t SIZE_TORQUE_ENABLE = 1;
constexpr uint8_t SIZE_LED = 1;
constexpr uint8_t SIZE_GOAL_PWM = 2;
constexpr uint8_t SIZE_GOAL_CURRENT = 2;
constexpr uint8_t SIZE_GOAL_VELOCITY = 4;
constexpr uint8_t SIZE_GOAL_POSITION = 4;
constexpr uint8_t SIZE_PRESENT_PWM = 2;
constexpr uint8_t SIZE_PRESENT_CURRENT = 2;
constexpr uint8_t SIZE_PRESENT_VELOCITY = 4;
constexpr uint8_t SIZE_PRESENT_POSITION = 4;
constexpr uint8_t SIZE_PRESENT_INPUT_VOLTAGE = 2;
constexpr uint8_t SIZE_PRESENT_TEMPERATURE = 1;

// Position limits
constexpr uint32_t POSITION_MIN = 0;
constexpr uint32_t POSITION_MAX = 4095;

}  // namespace xl330

}  // namespace dynamixel
