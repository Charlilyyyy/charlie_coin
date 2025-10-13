#ifndef UTXO_H
#define UTXO_H

#include <string>
#include <vector>
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <iostream>

struct UTXOEntry {
    std::string txId;       // Transaction ID
    int index;              // Output index
    std::string address;    // Recipient address
    double amount;          // Amount

    UTXOEntry() = default;
    UTXOEntry(const std::string& tid, int idx, const std::string& addr, double amt)
        : txId(tid), index(idx), address(addr), amount(amt) {}

    // Serialize for RocksDB
    std::string serialize() const;
    static UTXOEntry deserialize(const std::string& data);
};

class UTXOSet {
private:
    rocksdb::DB* db;

    static std::string buildKey(const std::string& txId, int index);

public:
    UTXOSet(const std::string& dbPath);
    ~UTXOSet();

    // Add new UTXO
    void addUTXO(const UTXOEntry& utxo);

    // Remove spent UTXO
    void removeUTXO(const std::string& txId, int index);

    // Check existence
    bool exists(const std::string& txId, int index) const;

    // Get all UTXOs for an address
    std::vector<UTXOEntry> getUTXOsForAddress(const std::string& address) const;

    // Print all UTXOs
    void printAllUTXOs() const;

    // ========== Checkpoint/State Management ==========
    
    // Save the last processed block height (checkpoint)
    void saveCheckpoint(int blockHeight);
    
    // Get the last processed block height (-1 if no checkpoint exists)
    int getCheckpoint() const;
    
    // Clear all UTXOs (for resync from genesis)
    void clearAllUTXOs();
    
    // Get total UTXO count
    int getUTXOCount() const;
};

#endif // UTXO_H
