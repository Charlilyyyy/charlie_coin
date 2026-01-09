#include "./header/transaction.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdexcept>
#include <algorithm>

// Constructor: generates txID and signature
Transaction::Transaction(const std::string& sender,
                         const std::string& receiver,
                         double amount,
                         EVP_PKEY* privateKey)
    : sender_(sender), receiver_(receiver), amount_(amount)
{
    timestamp_ = static_cast<uint32_t>(time(nullptr));

    // Generate transaction hash
    tx_id_ = generateTxIDFromData();

    // Sign transaction using private key
    signature_ = generateSignature(privateKey);
}

// Compute SHA256 of serialized transaction data
std::array<uint8_t, 32> Transaction::generateTxIDFromData() const {
    std::ostringstream oss;
    oss << sender_ << receiver_ << amount_ << timestamp_;
    std::string txData = oss.str();

    std::array<uint8_t, 32> hash;
    SHA256(reinterpret_cast<const unsigned char*>(txData.data()), txData.size(), hash.data());

    return hash;
}

// Generate ECDSA signature of txID using sender's private key
std::array<uint8_t, 64> Transaction::generateSignature(EVP_PKEY* privateKey) {
    if (!privateKey) throw std::runtime_error("Private key is null");

    std::array<uint8_t, 64> sig{};
    
    // Get EC_KEY from EVP_PKEY
    const EC_KEY* ecKey = EVP_PKEY_get0_EC_KEY(privateKey);
    if (!ecKey) throw std::runtime_error("Failed to get EC_KEY from EVP_PKEY");

    // Sign txID hash
    ECDSA_SIG* ecdsaSig = ECDSA_do_sign(tx_id_.data(), tx_id_.size(), const_cast<EC_KEY*>(ecKey));
    if (!ecdsaSig) throw std::runtime_error("ECDSA signing failed");

    // Extract r and s
    const BIGNUM* r;
    const BIGNUM* s;
    ECDSA_SIG_get0(ecdsaSig, &r, &s);

    int r_len = BN_num_bytes(r);
    int s_len = BN_num_bytes(s);

    if (r_len > 32 || s_len > 32) {
        ECDSA_SIG_free(ecdsaSig);
        throw std::runtime_error("Signature component too long for 32 bytes");
    }

    // Zero-fill sig first
    sig.fill(0);

    // Copy r and s to sig buffer, right-aligned
    BN_bn2binpad(r, sig.data() + (32 - r_len), r_len);        // r
    BN_bn2binpad(s, sig.data() + 32 + (32 - s_len), s_len);   // s

    ECDSA_SIG_free(ecdsaSig);
    return sig;
}


// Getter for transaction ID
uint256 Transaction::getTxID() const { return tx_id_; }

// Getter for sender address
std::string Transaction::getSender() const { return sender_; }

// Getter for receiver address
std::string Transaction::getReceiver() const { return receiver_; }

// Getter for amount
double Transaction::getAmount() const { return amount_; }

// Getter for timestamp
uint32_t Transaction::getTimestamp() const { return timestamp_; }

// Getter for raw signature
std::array<uint8_t, 64> Transaction::getSignature() const { return signature_; }

// Getter for signature in hex string
std::string Transaction::getSignatureHex() const {
    std::ostringstream oss;
    for (auto byte : signature_) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    return oss.str();
}

// Print transaction details
void Transaction::printTransaction() const {
    std::cout << "Sender: " << sender_ << "\n"
              << "Receiver: " << receiver_ << "\n"
              << "Amount: " << amount_ << "\n"
              << "Timestamp: " << timestamp_ << "\n"
              << "TxID: ";
    for (auto b : tx_id_) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    std::cout << "\nSignature: " << getSignatureHex() << "\n";
}
