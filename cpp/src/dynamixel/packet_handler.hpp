#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include "port_handler.hpp"
#include "dynamixel_defs.hpp"

namespace dynamixel {

class PacketHandler {
public:
    PacketHandler() = default;
    ~PacketHandler() = default;

    float getProtocolVersion() const { return 2.0f; }

    // CRC calculation
    uint16_t updateCRC(uint16_t crc_accum, const uint8_t* data_blk_ptr, size_t data_blk_size);

    // Packet transmission/reception
    int txPacket(PortHandler& port, uint8_t* txpacket);
    int rxPacket(PortHandler& port, uint8_t* rxpacket, size_t& rx_length);
    int txRxPacket(PortHandler& port, uint8_t* txpacket, uint8_t* rxpacket, size_t& rx_length, uint8_t& error);

    // High-level operations
    int ping(PortHandler& port, uint8_t id, uint16_t& model_number, uint8_t& error);

    // Read operations
    int read1ByteTxRx(PortHandler& port, uint8_t id, uint16_t address, uint8_t& data, uint8_t& error);
    int read2ByteTxRx(PortHandler& port, uint8_t id, uint16_t address, uint16_t& data, uint8_t& error);
    int read4ByteTxRx(PortHandler& port, uint8_t id, uint16_t address, uint32_t& data, uint8_t& error);

    // Write operations
    int write1ByteTxRx(PortHandler& port, uint8_t id, uint16_t address, uint8_t data, uint8_t& error);
    int write2ByteTxRx(PortHandler& port, uint8_t id, uint16_t address, uint16_t data, uint8_t& error);
    int write4ByteTxRx(PortHandler& port, uint8_t id, uint16_t address, uint32_t data, uint8_t& error);

private:
    void addStuffing(uint8_t* packet, size_t& packet_length);
    void removeStuffing(uint8_t* packet, size_t& packet_length);

    int readTxRx(PortHandler& port, uint8_t id, uint16_t address, uint16_t length,
                 uint8_t* data, uint8_t& error);
    int writeTxRx(PortHandler& port, uint8_t id, uint16_t address, uint16_t length,
                  const uint8_t* data, uint8_t& error);

    static const uint16_t crc_table_[256];
};

}  // namespace dynamixel
