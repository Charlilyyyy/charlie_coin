#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "block.h"
#include "transaction.h"
#include "utxo.h"
#include "mempool.h"
#include "merkle.h"

class Node {
private:
    std::vector<Block> blockchain;   // Chain of blocks
    Mempool mempool;                 // Unconfirmed transactions
    UTXOSet utxoSet;                 // Track spendable outputs
    uint32_t blockBits;              // Mining difficulty (bits)

    // Helper: create coinbase transaction for miner reward
    Transaction createCoinbaseTx(const std::vector<uint8_t>& minerScriptPubKey, int64_t rewardSatoshis);

public:
    Node(const std::string& utxoDBPath, uint32_t difficultyBits);
    ~Node();

    // Receive a new transaction into mempool
    void receiveTransaction(const Transaction& tx);

    // Build a block from mempool and mine it
    void createAndMineBlock(const std::vector<uint8_t>& minerScriptPubKey, size_t maxTxPerBlock = 10);

    // Add a mined or received block to blockchain
    bool addBlock(const Block& block);

    // Print the blockchain
    void printBlockchain() const;

    // Print mempool
    void printMempool() const;
};
