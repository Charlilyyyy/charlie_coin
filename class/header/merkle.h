#pragma once
#include <vector>
#include <array>
#include <openssl/sha.h>
#include <algorithm>

using uint256 = std::array<unsigned char, 32>;

class MerkleTree {
public:
    // Compute Bitcoin-style double SHA256 Merkle root
    static uint256 computeMerkleRoot(const std::vector<uint256>& txHashes);
};
