#include "./header/transaction.h"
#include <openssl/sha.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

// ------------------- Constructor -------------------
Transaction::Transaction(int32_t version_, uint32_t lockTime_)
    : version(version_), lockTime(lockTime_) 
{
    txid.fill(0);
}

Transaction::Transaction(int64_t rewardSatoshis, const std::vector<uint8_t>& minerScriptPubKey)
    : version(1), lockTime(0)
{
    // Coinbase has no inputs
    vin.clear();

    // One output: to miner
    TxOutput out;
    out.value = rewardSatoshis;
    out.scriptPubKey = minerScriptPubKey;
    vout.push_back(out);

    // Compute txid
    computeTxID();
}


// ------------------- Add Input/Output -------------------
void Transaction::addInput(const TxInput& input) {
    vin.push_back(input);
}

void Transaction::addOutput(const TxOutput& output) {
    vout.push_back(output);
}

// ------------------- Serialize Transaction -------------------
std::vector<uint8_t> Transaction::serialize() const {
    std::vector<uint8_t> out;

    // Version (little-endian)
    out.push_back(version & 0xFF);
    out.push_back((version >> 8) & 0xFF);
    out.push_back((version >> 16) & 0xFF);
    out.push_back((version >> 24) & 0xFF);

    // Input count (varint simplified: assume < 0xFD)
    out.push_back(static_cast<uint8_t>(vin.size()));

    // Inputs
    for (const auto& in : vin) {
        // prevTxID
        out.insert(out.end(), in.prevTxID.begin(), in.prevTxID.end());
        // vout
        out.push_back(in.vout & 0xFF);
        out.push_back((in.vout >> 8) & 0xFF);
        out.push_back((in.vout >> 16) & 0xFF);
        out.push_back((in.vout >> 24) & 0xFF);
        // scriptSig length
        out.push_back(static_cast<uint8_t>(in.scriptSig.size()));
        // scriptSig
        out.insert(out.end(), in.scriptSig.begin(), in.scriptSig.end());
        // sequence
        out.push_back(in.sequence & 0xFF);
        out.push_back((in.sequence >> 8) & 0xFF);
        out.push_back((in.sequence >> 16) & 0xFF);
        out.push_back((in.sequence >> 24) & 0xFF);
    }

    // Output count (varint simplified)
    out.push_back(static_cast<uint8_t>(vout.size()));

    // Outputs
    for (const auto& outp : vout) {
        // value (8 bytes, little-endian)
        for (int i = 0; i < 8; ++i) {
            out.push_back((outp.value >> (8 * i)) & 0xFF);
        }
        // scriptPubKey length
        out.push_back(static_cast<uint8_t>(outp.scriptPubKey.size()));
        // scriptPubKey
        out.insert(out.end(), outp.scriptPubKey.begin(), outp.scriptPubKey.end());
    }

    // lockTime
    out.push_back(lockTime & 0xFF);
    out.push_back((lockTime >> 8) & 0xFF);
    out.push_back((lockTime >> 16) & 0xFF);
    out.push_back((lockTime >> 24) & 0xFF);

    return out;
}

// ------------------- Compute txid -------------------
void Transaction::computeTxID() {
    auto ser = serialize();
    std::array<uint8_t, 32> hash1, hash2;

    // First SHA256
    SHA256(ser.data(), ser.size(), hash1.data());
    // Second SHA256
    SHA256(hash1.data(), hash1.size(), hash2.data());

    txid = hash2;
}

// ------------------- Print Transaction -------------------
void Transaction::printTransaction() const {
    std::cout << "Transaction Info:\n";
    std::cout << "Version: " << version << "\n";
    std::cout << "Inputs (" << vin.size() << "):\n";
    for (size_t i = 0; i < vin.size(); ++i) {
        std::cout << "  Input " << i << ": prevTxID = ";
        for (auto b : vin[i].prevTxID) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
        std::cout << ", vout = " << vin[i].vout << "\n";
    }

    std::cout << "Outputs (" << vout.size() << "):\n";
    for (size_t i = 0; i < vout.size(); ++i) {
        std::cout << "  Output " << i << ": value = " << vout[i].value
                  << ", scriptPubKeyLen = " << vout[i].scriptPubKey.size() << "\n";
    }

    std::cout << "LockTime: " << lockTime << "\n";
    std::cout << "TxID: ";
    for (auto b : txid) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    std::cout << "\n";
}
