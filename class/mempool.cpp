#include "./header/mempool.h"
#include <algorithm>
#include <iostream>

// ------------------- Constructor / Destructor -------------------
Mempool::Mempool() = default;
Mempool::~Mempool() = default;

// ------------------- Add Transaction -------------------
void Mempool::addTransaction(const Transaction& tx) {
    std::lock_guard<std::mutex> lock(mtx);
    transactions.push_back(tx);
}

// ------------------- Remove Transaction -------------------
void Mempool::removeTransaction(const uint256& txID) {
    std::lock_guard<std::mutex> lock(mtx);
    transactions.erase(
        std::remove_if(transactions.begin(), transactions.end(),
            [&](const Transaction& t) { return t.txid == txID; }),
        transactions.end()
    );
}

// ------------------- Get All Transactions -------------------
std::vector<Transaction> Mempool::getTransactions() const {
    std::lock_guard<std::mutex> lock(mtx);
    return transactions;
}

// ------------------- Pick First N Transactions -------------------
std::vector<Transaction> Mempool::pickTransactions(size_t maxCount) {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<Transaction> picked;
    size_t count = std::min(maxCount, transactions.size());
    picked.insert(picked.end(), transactions.begin(), transactions.begin() + count);
    return picked;
}

// ------------------- Print Mempool -------------------
void Mempool::printMempool() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "=== Mempool (" << transactions.size() << " txs) ===\n";
    for (size_t i = 0; i < transactions.size(); ++i) {
        std::cout << "--- Transaction " << i << " ---\n";
        transactions[i].printTransaction();
    }
}

// ------------------- Clear Mempool -------------------
void Mempool::clear() {
    std::lock_guard<std::mutex> lock(mtx);
    transactions.clear();
}
