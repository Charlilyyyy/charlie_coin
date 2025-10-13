#include "./header/utxo.h"
#include <sstream>

// ------------------- UTXOEntry serialization -------------------
std::string UTXOEntry::serialize() const {
    return txId + "|" + std::to_string(index) + "|" + address + "|" + std::to_string(amount);
}

UTXOEntry UTXOEntry::deserialize(const std::string& data) {
    std::istringstream ss(data);
    std::string token;
    UTXOEntry u;

    std::getline(ss, u.txId, '|');
    std::getline(ss, token, '|');
    u.index = std::stoi(token);
    std::getline(ss, u.address, '|');
    std::getline(ss, token, '|');
    u.amount = std::stod(token);

    return u;
}

// ------------------- Helper for key -------------------
std::string UTXOSet::buildKey(const std::string& txId, int index) {
    return txId + ":" + std::to_string(index);
}

// ------------------- UTXOSet methods -------------------
UTXOSet::UTXOSet(const std::string& dbPath) {
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, dbPath, &db);
    if(!status.ok()) {
        std::cerr << "Failed to open RocksDB: " << status.ToString() << std::endl;
        db = nullptr;
    } else {
        std::cout << "RocksDB opened at: " << dbPath << std::endl;
    }
}

UTXOSet::~UTXOSet() {
    delete db;
}

void UTXOSet::addUTXO(const UTXOEntry& utxo) {
    std::string key = buildKey(utxo.txId, utxo.index);
    rocksdb::Status s = db->Put(rocksdb::WriteOptions(), key, utxo.serialize());
    if(!s.ok()) std::cerr << "Add UTXO failed: " << s.ToString() << std::endl;
}

void UTXOSet::removeUTXO(const std::string& txId, int index) {
    std::string key = buildKey(txId, index);
    rocksdb::Status s = db->Delete(rocksdb::WriteOptions(), key);
    if(!s.ok()) std::cerr << "Remove UTXO failed: " << s.ToString() << std::endl;
}

bool UTXOSet::exists(const std::string& txId, int index) const {
    std::string key = buildKey(txId, index);
    std::string value;
    rocksdb::Status s = db->Get(rocksdb::ReadOptions(), key, &value);
    return s.ok();
}

std::vector<UTXOEntry> UTXOSet::getUTXOsForAddress(const std::string& address) const {
    std::vector<UTXOEntry> result;
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        UTXOEntry u = UTXOEntry::deserialize(it->value().ToString());
        if(u.address == address) result.push_back(u);
    }
    delete it;
    return result;
}

void UTXOSet::printAllUTXOs() const {
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    std::cout << "Current UTXO set:\n";
    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        // Skip checkpoint metadata
        if(it->key().ToString() == "_checkpoint_") continue;
        
        UTXOEntry u = UTXOEntry::deserialize(it->value().ToString());
        std::cout << "TxID: " << u.txId 
                  << ", Index: " << u.index 
                  << ", Addr: " << u.address 
                  << ", Amount: " << u.amount << std::endl;
    }
    delete it;
}

// ========== Checkpoint/State Management Implementation ==========

void UTXOSet::saveCheckpoint(int blockHeight) {
    std::string key = "_checkpoint_";
    std::string value = std::to_string(blockHeight);
    rocksdb::Status s = db->Put(rocksdb::WriteOptions(), key, value);
    if(!s.ok()) {
        std::cerr << "Failed to save checkpoint: " << s.ToString() << std::endl;
    } else {
        std::cout << "✓ Checkpoint saved at block height: " << blockHeight << std::endl;
    }
}

int UTXOSet::getCheckpoint() const {
    std::string key = "_checkpoint_";
    std::string value;
    rocksdb::Status s = db->Get(rocksdb::ReadOptions(), key, &value);
    
    if(s.ok()) {
        return std::stoi(value);
    } else if(s.IsNotFound()) {
        return -1; // No checkpoint exists (first run)
    } else {
        std::cerr << "Error reading checkpoint: " << s.ToString() << std::endl;
        return -1;
    }
}

void UTXOSet::clearAllUTXOs() {
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    int count = 0;
    
    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        // Don't delete checkpoint metadata
        if(key != "_checkpoint_") {
            db->Delete(rocksdb::WriteOptions(), key);
            count++;
        }
    }
    delete it;
    
    std::cout << "Cleared " << count << " UTXOs from database" << std::endl;
}

int UTXOSet::getUTXOCount() const {
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    int count = 0;
    
    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        // Skip checkpoint metadata
        if(it->key().ToString() != "_checkpoint_") {
            count++;
        }
    }
    delete it;
    
    return count;
}
