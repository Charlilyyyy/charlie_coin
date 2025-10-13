#include <iostream>

#include "../struct/walletStruct.cpp"
#include "./header/wallet.h"

// getters
std::string Wallet::getPubKey_() const{
    return pubKey_;
}

std::string Wallet::getPrivKey_() const{
    return privKey_;
}

std::string Wallet::getAddress_() const{
    return address_;
}

// setters
bool Wallet::setPubKey_(WalletStruct walletStruct){
    pubKey_ = walletStruct.pubKey_;
    return true;
}

bool Wallet::setPrivKey_(WalletStruct walletStruct){
    privKey_ = walletStruct.privKey_;
    return true;
}

bool Wallet::setAddress_(WalletStruct walletStruct){
    address_ = walletStruct.address_;
    return true;
}

// other methods
std::string Wallet::serialize() const {
    return address_ + "|" + pubKey_ + "|" + privKey_;
}

Wallet Wallet::deserialize(const std::string& data) {
    WalletStruct walletStruct;
    
    size_t pos1 = data.find('|');
    size_t pos2 = data.find('|', pos1 + 1);
    
    walletStruct.address_ = data.substr(0, pos1);
    walletStruct.pubKey_ = data.substr(pos1 + 1, pos2 - pos1 - 1);
    walletStruct.privKey_ = data.substr(pos2 + 1);
    
    return Wallet(walletStruct);
}