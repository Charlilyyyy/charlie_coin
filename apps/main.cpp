#include <iostream>
#include <ctime>
#include <rocksdb/db.h>
#include <rocksdb/options.h>

#include "../helper/header/helper.h"
#include "../helper/header/template.h"
#include "../class/header/wallet.h"
#include "../class/header/utxo.h"
#include "../struct/walletStruct.cpp"

int main(){
    std::cout << "Starting up .. " << std::endl;
    std::cout << "Charlie Coin Software" << std::endl << std::endl;

    std::cout << "My node id : 1" << std::endl;
    std::cout << "Loading peer nodes" << std::endl;
    std::cout << "Peer nodes is : [1,2,3]" << std::endl;

    std::pair<std::string, std::string> keys = Helper::generateRSAKeyPairString(2048);
    // std::cout << keys << std::endl;

    // wallet done

    // WalletStruct walletStruct;
    // walletStruct.pubKey_ = keys.first;
    // walletStruct.privKey_ = keys.second;
    // walletStruct.address_ = "wallet_address_123";
    // Wallet wallet1(walletStruct);
    // std::cout << wallet1.getPubKey_() << std::endl;
    // std::cout << wallet1.getPrivKey_() << std::endl;
    // std::cout << wallet1.getAddress_() << std::endl;

    // ========== UTXO State Management Demo ==========
    std::cout << "\n========== UTXO & Checkpoint System ==========\n" << std::endl;
    
    UTXOSet utxoSet("utxo_db");

    // Check if we have a checkpoint (previous state)
    int lastProcessedBlock = utxoSet.getCheckpoint();
    
    if(lastProcessedBlock == -1) {
        // COLD START: First run, no checkpoint exists
        std::cout << "🔵 COLD START: No checkpoint found. Starting from genesis block..." << std::endl;
        std::cout << "📦 Simulating blockchain sync from block 0...\n" << std::endl;
        
        // Simulate processing blocks 0-2 (calculating UTXO from blockchain)
        std::cout << "Processing Block 0 (Genesis)..." << std::endl;
        utxoSet.addUTXO({"genesis_tx", 0, "Alice", 100.0});
        utxoSet.saveCheckpoint(0);
        
        std::cout << "Processing Block 1..." << std::endl;
        utxoSet.addUTXO({"tx1", 0, "Bob", 30.0});
        utxoSet.addUTXO({"tx1", 1, "Alice", 70.0});
        utxoSet.removeUTXO("genesis_tx", 0); // Alice spent her genesis UTXO
        utxoSet.saveCheckpoint(1);
        
        std::cout << "Processing Block 2..." << std::endl;
        utxoSet.addUTXO({"tx2", 0, "Charlie", 20.0});
        utxoSet.addUTXO({"tx2", 1, "Bob", 10.0});
        utxoSet.saveCheckpoint(2);
        
        std::cout << "\n✅ Initial sync complete!" << std::endl;
        
    } else {
        // WARM START: Checkpoint exists, resume from there
        std::cout << "🟢 WARM START: Checkpoint found at block " << lastProcessedBlock << std::endl;
        std::cout << "📦 Loading existing UTXO state from database...\n" << std::endl;
        
        // Simulate: Let's say blockchain now has block 3, we only process new blocks
        int currentBlockchainHeight = 3; // This would come from your blockchain
        
        if(currentBlockchainHeight > lastProcessedBlock) {
            std::cout << "⚡ New blocks detected! Processing blocks " 
                      << (lastProcessedBlock + 1) << " to " << currentBlockchainHeight << "...\n" << std::endl;
            
            // Only process NEW blocks (no need to recalculate from genesis!)
            for(int block = lastProcessedBlock + 1; block <= currentBlockchainHeight; block++) {
                std::cout << "Processing Block " << block << "..." << std::endl;
                // Process block transactions and update UTXO
                // This is much faster than recalculating from block 0!
                utxoSet.saveCheckpoint(block);
            }
            std::cout << "\n✅ Sync complete!" << std::endl;
        } else {
            std::cout << "✅ Already synced to latest block!" << std::endl;
        }
    }
    
    // Show final state
    std::cout << "\n========== Current UTXO State ==========\n" << std::endl;
    utxoSet.printAllUTXOs();
    std::cout << "\nTotal UTXOs: " << utxoSet.getUTXOCount() << std::endl;
    std::cout << "Last synced block: " << utxoSet.getCheckpoint() << std::endl;
    
    std::cout << "\n💡 TIP: Run the program again to see WARM START in action!" << std::endl;
    std::cout << "💡 The program will load existing UTXO state instead of recalculating from genesis.\n" << std::endl;
    //
}