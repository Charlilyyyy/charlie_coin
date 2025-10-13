#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <vector>

#include "../../struct/blockheaderStruct.cpp"
#include "./transaction.h"

class Block {
    private:
        BlockHeader blockheader_;
        std::vector<Transaction> txs_;
};

#endif