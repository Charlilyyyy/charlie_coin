#include "./header/node.h"
#include <ctime>
#include <iomanip>
#include <sstream>

// ------------------- Constructor / Destructor -------------------
Node::Node(const std::string& utxoDBPath, uint32_t difficultyBits)
    : utxoSet(utxoDBPath), blockBits(difficultyBits) {}

Node::~Node() = default;

// ------------------- Receive Transaction -------------------
void Node::receiveTransaction(const Transaction& tx) {
    mempool.addTransaction(tx);
}

// ------------------- Print Mempool -------------------
void Node::printMempool() const {
    mempool.printMempool();
}

// ------------------- Print Blockchain -------------------
void Node::printBlockchain() const {
    std::cout << "=== Blockchain (" << blockchain.size() << " blocks) ===\n";
    for (size_t i = 0; i < blockchain.size(); ++i) {
        std::cout << "--- Block " << i << " ---\n";
        blockchain[i].printBlock();
    }
}

// ------------------- Create CoinBase Transaction -------------------
Transaction Node::createCoinbaseTx(const std::vector<uint8_t>& minerScriptPubKey, int64_t rewardSatoshis) {
    return Transaction(rewardSatoshis, minerScriptPubKey);
}

// ------------------- Create and Mine Block -------------------
void Node::createAndMineBlock(const std::vector<uint8_t>& minerScriptPubKey, size_t maxTxPerBlock) {
    // Pick transactions from mempool
    std::vector<Transaction> pickedTxs = mempool.pickTransactions(maxTxPerBlock);

    // Add coinbase tx for miner reward (50 BTC = 50 * 1e8 satoshis)
    Transaction coinbaseTx = createCoinbaseTx(minerScriptPubKey, 50 * 100000000);
    pickedTxs.insert(pickedTxs.begin(), coinbaseTx);

    // Compute Merkle root
    std::vector<uint256> txHashes;
    for (const auto& tx : pickedTxs) txHashes.push_back(tx.txid);
    std::array<uint8_t, 32> merkleRoot = MerkleTree::computeMerkleRoot(txHashes);

    // Prepare block header
    std::array<uint8_t, 32> prevHash{};
    if (!blockchain.empty()) prevHash = blockchain.back().blockHash;

    BlockHeader header(
        1,              // version
        prevHash,       // previous block hash
        static_cast<uint32_t>(time(nullptr)), // timestamp
        blockBits       // difficulty
    );

    header.merkleRoot_ = merkleRoot;

    // Create block
    Block newBlock(header);

    // Add transactions
    for (const auto& tx : pickedTxs) newBlock.addTransaction(tx);

    // Mine block
    std::cout << "Mining block...\n";
    // 6️⃣ Mine block (simple Proof-of-Work)
    uint32_t nonce = 0;
    std::array<uint8_t, 32> hash;
    while (true) {
        header.nonce_ = nonce;
        newBlock.computeBlockHash();
        hash = newBlock.blockHash;

        // Simple difficulty check: first N bytes must be zero
        bool success = true;
        for (size_t i = 0; i < blockBits / 8; ++i) {
            if (hash[i] != 0) { success = false; break; }
        }
        if (success) break;

        ++nonce;
    }

    // Update blockchain
    blockchain.push_back(newBlock);

    // Remove mined transactions from mempool (skip coinbase)
    for (size_t i = 1; i < pickedTxs.size(); ++i) {
        mempool.removeTransaction(pickedTxs[i].txid);
        // TODO: update UTXO set for inputs/outputs
    }

    std::cout << "Block mined successfully!\n";
}

// ------------------- Add Block -------------------
bool Node::addBlock(const Block& block) {
    // Simple validation: check previous hash
    if (!blockchain.empty() && block.header.prevBlockHash_ != blockchain.back().blockHash) {
        std::cerr << "Block rejected: prev hash mismatch\n";
        return false;
    }

    blockchain.push_back(block);
    return true;
}
