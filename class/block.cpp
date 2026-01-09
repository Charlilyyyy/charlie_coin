#include "./header/block.h"
#include <openssl/sha.h>
#include <iostream>
#include <iomanip>
#include <sstream>

// ------------------- Constructor -------------------
Block::Block(const BlockHeader& header_)
    : header(header_)
{
    blockHash.fill(0);
}

// ------------------- Add Transaction -------------------
void Block::addTransaction(const Transaction& tx) {
    transactions.push_back(tx);
}

// ------------------- Serialize Block -------------------
std::vector<uint8_t> Block::serialize() const {
    std::vector<uint8_t> out;

    // Serialize header first (manual byte copy)
    // Version
    out.push_back(header.version_ & 0xFF);
    out.push_back((header.version_ >> 8) & 0xFF);
    out.push_back((header.version_ >> 16) & 0xFF);
    out.push_back((header.version_ >> 24) & 0xFF);

    // Previous block hash
    out.insert(out.end(), header.prevBlockHash_.begin(), header.prevBlockHash_.end());

    // Merkle root
    out.insert(out.end(), header.merkleRoot_.begin(), header.merkleRoot_.end());

    // Timestamp
    out.push_back(header.timestamp_ & 0xFF);
    out.push_back((header.timestamp_ >> 8) & 0xFF);
    out.push_back((header.timestamp_ >> 16) & 0xFF);
    out.push_back((header.timestamp_ >> 24) & 0xFF);

    // Difficulty range
    out.push_back(header.bits_ & 0xFF);
    out.push_back((header.bits_ >> 8) & 0xFF);
    out.push_back((header.bits_ >> 16) & 0xFF);
    out.push_back((header.bits_ >> 24) & 0xFF);

    // Nonce
    out.push_back(header.nonce_ & 0xFF);
    out.push_back((header.nonce_ >> 8) & 0xFF);
    out.push_back((header.nonce_ >> 16) & 0xFF);
    out.push_back((header.nonce_ >> 24) & 0xFF);

    // Transactions count (simplified varint)
    out.push_back(static_cast<uint8_t>(transactions.size()));

    // Serialize each transaction
    for (const auto& tx : transactions) {
        auto txBytes = tx.serialize();
        out.insert(out.end(), txBytes.begin(), txBytes.end());
    }

    return out;
}

// ------------------- Compute Block Hash -------------------
void Block::computeBlockHash() {
    auto ser = serialize();
    std::array<uint8_t, 32> hash1, hash2;

    // First SHA256
    SHA256(ser.data(), ser.size(), hash1.data());
    // Second SHA256
    SHA256(hash1.data(), hash1.size(), hash2.data());

    blockHash = hash2;
}

// ------------------- Print Block -------------------
void Block::printBlock() const {
    std::cout << "===== Block =====\n";
    std::cout << "Block Header:\n";
    std::cout << "Version: " << header.version_ << "\n";
    std::cout << "PrevBlockHash: ";
    for (auto b : header.prevBlockHash_) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    std::cout << "\nMerkleRoot: ";
    for (auto b : header.merkleRoot_) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    std::cout << "\nTimestamp: " << header.timestamp_ << "\n";
    std::cout << "Difficulty: " << header.bits_ << "\n";
    std::cout << "Nonce: " << header.nonce_ << "\n";

    std::cout << "Transactions (" << transactions.size() << "):\n";
    for (size_t i = 0; i < transactions.size(); ++i) {
        std::cout << "--- Transaction " << i << " ---\n";
        transactions[i].printTransaction();
    }

    std::cout << "Block Hash: ";
    for (auto b : blockHash) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    std::cout << "\n================\n";
}
