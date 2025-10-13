#ifndef WALLET_H
#define WALLET_H

#include <iostream>

#include "../../struct/walletStruct.cpp"

class Wallet {
    private:
        std::string pubKey_, privKey_, address_;

    public:
        // construtor
        Wallet(WalletStruct walletStruct) : 
            pubKey_(walletStruct.pubKey_), privKey_(walletStruct.privKey_), address_(walletStruct.address_) {}
        // getters
        std::string getPubKey_() const;
        std::string getPrivKey_() const;
        std::string getAddress_() const;

        // setters
        bool setPubKey_(WalletStruct walletStruct);
        bool setPrivKey_(WalletStruct walletStruct);
        bool setAddress_(WalletStruct walletStruct);

        // other methods
        std::string serialize() const;

        static Wallet deserialize(const std::string& data);
};

#endif