#ifndef TX_H
#define TX_H

#include <iostream>

struct TxInputStruct {
    std::string prevTxHash;
    int outputIndex;
    std::string signature;
};

struct TxOutputStruct {
    double amount;
    std::string recipientPublicKey;
};

#endif
