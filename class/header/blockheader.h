#pragma once
#include <cstdint>
#include <array>

class BlockHeader {
public:
    int32_t version_;                     // 4 bytes
    std::array<uint8_t, 32> prevBlockHash_; // 32 bytes
    std::array<uint8_t, 32> merkleRoot_;    // 32 bytes
    uint32_t timestamp_;                  // 4 bytes
    uint32_t bits_;                       // 4 bytes (difficulty)
    uint32_t nonce_;                      // 4 bytes

    // Constructor
    BlockHeader(int32_t version,
                const std::array<uint8_t, 32>& prevBlockHash,
                const std::array<uint8_t, 32>& merkleRoot,
                uint32_t timestamp,
                uint32_t bits,
                uint32_t nonce = 0);

    // Serialize to 80-byte array (little-endian)
    std::array<uint8_t, 80> serialize() const;

    // Compute double SHA-256 hash (Bitcoin-style)
    std::array<uint8_t, 32> hash() const;
};
