#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <iostream>
#include <vector>

#include "./block.h"

class BlockChain {
    private:
        std::vector<Block> blocks_;
        uint32_t currentDifficulty_;
        const int DIFFICULTY_ADJUSTMENT_INTERVAL = 5;  // Adjust every 5 blocks
        const int TARGET_BLOCK_TIME = 2;  // 2 seconds target

    public:
        std::vector<Block> getBlocks_() const;
};

#endif