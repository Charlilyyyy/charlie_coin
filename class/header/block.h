#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <array>
#include "blockheader.h"
#include "transaction.h"

using uint256 = std::array<unsigned char, 32>;

class Block {
public:
    BlockHeader header;                // Block metadata
    std::vector<Transaction> transactions; // All transactions including coinbase
    uint256 blockHash;                 // Computed block hash

    // Constructor: set header fields known before mining
    Block(const BlockHeader& header_);

    // Add a transaction (coinbase first)
    void addTransaction(const Transaction& tx);

    // Serialize entire block (header + transactions)
    std::vector<uint8_t> serialize() const;

    // Compute block hash (double SHA256 of serialized block)
    void computeBlockHash();

    // Print block info
    void printBlock() const;

    // Add in public section
    void updateMerkleRoot();
};

#endif // BLOCK_H
