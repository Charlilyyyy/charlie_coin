#ifndef BLOCKHEADER_H
#define BLOCKHEADER_H

#include <iostream>
#include <string>
#include <array>
#include <cstdint>

using uint256 = std::array<unsigned char, 32>;

struct BlockHeader {
    int32_t version_;               // 4 bytes
    uint256 prev_block_hash_;       // 32 bytes
    uint256 merkle_root_;           // 32 bytes
    uint32_t timestamp_;            // 4 bytes
    uint32_t difficulty_target_;    // 4 bytes (compact)
    uint32_t nonce_;                // 4 bytes
};

#endif