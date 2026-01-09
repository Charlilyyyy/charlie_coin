#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cstdint>
#include <openssl/evp.h> // For EVP_PKEY
#include <openssl/sha.h> // For SHA256

using uint256 = std::array<unsigned char, 32>;

class Transaction {
public:
    // Constructor for creating a transaction
    // privateKey is used to generate a real signature
    Transaction(const std::string& sender,
                const std::string& receiver,
                double amount,
                EVP_PKEY* privateKey);

    // Getters
    uint256 getTxID() const;
    std::string getSender() const;
    std::string getReceiver() const;
    double getAmount() const;
    uint32_t getTimestamp() const;
    std::array<uint8_t, 64> getSignature() const;
    std::string getSignatureHex() const;

    // Utility function to print transaction details
    void printTransaction() const;

private:
    // Transaction data
    uint256 tx_id_;                 // SHA256 hash of transaction data
    std::string sender_;            // sender address (derived from public key)
    std::string receiver_;          // receiver address
    double amount_;
    uint32_t timestamp_;
    std::array<uint8_t, 64> signature_;  // ECDSA signature (64 bytes: r||s)

    // Internal functions
    uint256 generateTxIDFromData() const;               // compute txID as SHA256 of transaction fields
    std::array<uint8_t, 64> generateSignature(EVP_PKEY* privateKey);  // generate ECDSA signature
};

#endif
