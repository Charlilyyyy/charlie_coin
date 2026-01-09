#ifndef UTXO_H
#define UTXO_H

#include <array>
#include <vector>
#include <cstdint>
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <iostream>

using uint256 = std::array<uint8_t, 32>; // 32-byte txid

struct UTXOEntry {
    uint256 txId;               // Transaction ID (hash)
    uint32_t index;             // Output index
    std::vector<uint8_t> scriptPubKey; // Locking script (recipient)
    int64_t value;              // Amount in satoshis

    UTXOEntry() = default;
    UTXOEntry(const uint256& tid, uint32_t idx, const std::vector<uint8_t>& script, int64_t val)
        : txId(tid), index(idx), scriptPubKey(script), value(val) {}

    // Serialize for RocksDB
    std::vector<uint8_t> serialize() const;
    static UTXOEntry deserialize(const std::vector<uint8_t>& data);
};

class UTXOSet {
private:
    rocksdb::DB* db;

    static std::string buildKey(const uint256& txId, uint32_t index);

public:
    UTXOSet(const std::string& dbPath);
    ~UTXOSet();

    // Add new UTXO
    void addUTXO(const UTXOEntry& utxo);

    // Remove spent UTXO
    void removeUTXO(const uint256& txId, uint32_t index);

    // Check existence
    bool exists(const uint256& txId, uint32_t index) const;

    // Get all UTXOs for a given scriptPubKey
    std::vector<UTXOEntry> getUTXOsForScript(const std::vector<uint8_t>& script) const;

    // Print all UTXOs
    void printAllUTXOs() const;

    // Checkpoint / state management
    void saveCheckpoint(int blockHeight);
    int getCheckpoint() const;
    void clearAllUTXOs();
    int getUTXOCount() const;
};

#endif // UTXO_H
