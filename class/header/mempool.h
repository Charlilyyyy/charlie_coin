#pragma once
#include <vector>
#include <mutex>
#include "transaction.h"

class Mempool {
private:
    std::vector<Transaction> transactions;
    mutable std::mutex mtx; // For thread safety

public:
    Mempool();
    ~Mempool();

    void addTransaction(const Transaction& tx);
    void removeTransaction(const uint256& txID);
    std::vector<Transaction> getTransactions() const;
    std::vector<Transaction> pickTransactions(size_t maxCount);
    void printMempool() const;
    void clear();
};
