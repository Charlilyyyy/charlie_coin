#include "./header/merkle.h"
#include <iostream>

// Double SHA256 helper
uint256 doubleSHA256(const std::vector<unsigned char>& data) {
    uint256 hash1, hash2;
    SHA256(data.data(), data.size(), hash1.data());
    SHA256(hash1.data(), hash1.size(), hash2.data());
    return hash2;
}

// Combine two hashes (concatenate and double SHA256)
uint256 hashPair(const uint256& a, const uint256& b) {
    std::vector<unsigned char> combined(64);
    std::copy(a.begin(), a.end(), combined.begin());
    std::copy(b.begin(), b.end(), combined.begin() + 32);
    return doubleSHA256(combined);
}

uint256 MerkleTree::computeMerkleRoot(const std::vector<uint256>& txHashes) {
    if (txHashes.empty()) {
        uint256 empty{};
        empty.fill(0);
        return empty;
    }

    std::vector<uint256> currentLevel = txHashes;

    while (currentLevel.size() > 1) {
        std::vector<uint256> nextLevel;

        for (size_t i = 0; i < currentLevel.size(); i += 2) {
            if (i + 1 < currentLevel.size()) {
                // hash pair of two nodes
                nextLevel.push_back(hashPair(currentLevel[i], currentLevel[i+1]));
            } else {
                // if odd number, duplicate last hash
                nextLevel.push_back(hashPair(currentLevel[i], currentLevel[i]));
            }
        }

        currentLevel = nextLevel;
    }

    return currentLevel[0]; // Merkle root
}
