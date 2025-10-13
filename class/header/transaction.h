#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cstdint>

using uint256 = std::array<unsigned char, 32>;

class Transaction {
    private:
        uint256 tx_id_;           // Transaction hash/ID
        std::string sender_;
        std::string receiver_;
        double amount_;
        uint32_t timestamp_;
        std::string signature_;   // In real blockchain, this would be cryptographic signature - now just use public key hash
};

#endif