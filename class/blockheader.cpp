#include "./header/blockheader.h"
#include <cstring>  // for memcpy
#include <array>
#include <openssl/sha.h>  // For SHA256

// Constructor
BlockHeader::BlockHeader(int32_t version,
                         const std::array<uint8_t, 32>& prevBlockHash,
                         uint32_t timestamp,
                         uint32_t bits,
                         uint32_t nonce)
{
    version_ = version;
    prevBlockHash_ = prevBlockHash;
    timestamp_ = timestamp;
    bits_ = bits;
    nonce_ = nonce;
}

// Helper function to serialize int32/uint32 to little-endian
static void writeLE(uint32_t val, uint8_t* out) {
    out[0] = val & 0xFF;
    out[1] = (val >> 8) & 0xFF;
    out[2] = (val >> 16) & 0xFF;
    out[3] = (val >> 24) & 0xFF;
}

std::array<uint8_t, 80> BlockHeader::serialize() const {
    std::array<uint8_t, 80> data{};
    uint8_t* ptr = data.data();

    // version
    writeLE(static_cast<uint32_t>(version_), ptr);
    ptr += 4;

    // prevBlockHash (reversed for little-endian)
    for (int i = 0; i < 32; ++i)
        ptr[i] = prevBlockHash_[31 - i];
    ptr += 32;

    // merkleRoot (reversed for little-endian)
    for (int i = 0; i < 32; ++i)
        ptr[i] = merkleRoot_[31 - i];
    ptr += 32;

    // timestamp
    writeLE(timestamp_, ptr);
    ptr += 4;

    // bits
    writeLE(bits_, ptr);
    ptr += 4;

    // nonce
    writeLE(nonce_, ptr);

    return data;
}

// Double SHA-256 hash
std::array<uint8_t, 32> BlockHeader::hash() const {
    std::array<uint8_t, 80> serialized = serialize();
    std::array<uint8_t, 32> hash1, hash2;

    SHA256(serialized.data(), serialized.size(), hash1.data());
    SHA256(hash1.data(), hash1.size(), hash2.data());

    // Bitcoin uses reversed hash for display
    std::array<uint8_t, 32> finalHash;
    for (int i = 0; i < 32; ++i)
        finalHash[i] = hash2[31 - i];

    return finalHash;
}
