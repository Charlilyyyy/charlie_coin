#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>

#include "./wallet.h"
#include "./transaction.h"
#include "./blockchain.h"
#include "./network.h"
#include "./utxo.h"

class Node {
    private:
        std::string NodeId_;
        std::vector<Wallet> wallet_;
        BlockChain blockchain_;
        std::vector<Transaction> mempool_;
        UTXOSet utxoSet_;
        // Network network_;

        // getters
        std::string getNodeId_() const;
        std::vector<Wallet> getWallets_() const;
        BlockChain getBlockChain_() const;
        std::vector<Transaction> getMempool_() const;
};

#endif