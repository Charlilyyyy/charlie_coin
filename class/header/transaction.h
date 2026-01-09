#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <cstdint>
#include <vector>
#include <array>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "./utxo.h"  // For uint256

using uint256 = std::array<unsigned char, 32>;

// ----------------- Transaction Input -----------------
struct TxInput {
    uint256 prevTxID;           // Previous transaction hash
    uint32_t vout;              // Index of output in previous tx
    std::vector<uint8_t> scriptSig;  // Signature script (unlocking)
    uint32_t sequence;          // Normally 0xFFFFFFFF

    TxInput() : vout(0), sequence(0xFFFFFFFF) {}
};

// ----------------- Transaction Output -----------------
struct TxOutput {
    int64_t value;                 // Amount in satoshis
    std::vector<uint8_t> scriptPubKey; // Locking script (recipient)
};

// ----------------- Transaction -----------------
class Transaction {
public:
    int32_t version;                 // Transaction version
    std::vector<TxInput> vin;        // Inputs
    std::vector<TxOutput> vout;      // Outputs
    uint32_t lockTime;               // Locktime
    uint256 txid;                     // Transaction ID (hash)

    // Constructors
    Transaction(int32_t version = 1, uint32_t lockTime = 0);

    // Coinbase constructor: no inputs, only one output to miner
    Transaction(int64_t rewardSatoshis, const std::vector<uint8_t>& minerScriptPubKey);

    // Add input/output
    void addInput(const TxInput& input);
    void addOutput(const TxOutput& output);

    // Serialize transaction (for txid computation or network)
    std::vector<uint8_t> serialize() const;

    // Compute double SHA256 txid from serialized tx
    void computeTxID();

    // Utility: print transaction info
    void printTransaction() const;
};

#endif // TRANSACTION_H
