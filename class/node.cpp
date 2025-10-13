#include <string>
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/iterator.h>
#include "./header/node.h"

// getters
std::string Node::getNodeId_() const {
    return NodeId_;
}

std::vector<Wallet> Node::getWallets_() const {
    std::vector<Wallet> wallets;
    
    // RocksDB setup
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = false;  // Don't create if it doesn't exist
    
    // Open the database (assuming it's in a "node_db" directory)
    rocksdb::Status status = rocksdb::DB::Open(options, "./node_db", &db);
    
    if (!status.ok()) {
        std::cerr << "Unable to open database: " << status.ToString() << std::endl;
        return wallets;  // Return empty vector if DB can't be opened
    }
    
    // Create iterator to traverse all wallet entries
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    
    // Iterate through all keys with "wallet:" prefix
    std::string wallet_prefix = "wallet:";
    for (it->Seek(wallet_prefix); it->Valid() && it->key().ToString().find(wallet_prefix) == 0; it->Next()) {
        std::string serialized_wallet = it->value().ToString();
        
        try {
            Wallet wallet = Wallet::deserialize(serialized_wallet);
            wallets.push_back(wallet);
        } catch (const std::exception& e) {
            std::cerr << "Error deserializing wallet: " << e.what() << std::endl;
        }
    }
    
    // Check for errors during iteration
    if (!it->status().ok()) {
        std::cerr << "Error during iteration: " << it->status().ToString() << std::endl;
    }
    
    delete it;
    delete db;
    
    return wallets;
}

BlockChain Node::getBlockChain_() const {
    return blockchain_;
}

std::vector<Transaction> Node::getMempool_() const {
    return mempool_;
}

