# Find OpenSSL
find_package(OpenSSL REQUIRED)

# RocksDB paths (Homebrew Apple Silicon)
set(ROCKSDB_INCLUDE_DIR "/opt/homebrew/include")
set(ROCKSDB_LIB_DIR "/opt/homebrew/lib")

# Include directories for your classes and helpers
set(MY_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/class
    ${CMAKE_SOURCE_DIR}/helper
    ${ROCKSDB_INCLUDE_DIR}
)

# Add executable
add_executable(main
    main.cpp
    ${CMAKE_SOURCE_DIR}/helper/helper.cpp
    ${CMAKE_SOURCE_DIR}/class/node.cpp
    ${CMAKE_SOURCE_DIR}/class/wallet.cpp
    ${CMAKE_SOURCE_DIR}/class/utxo.cpp
    ${CMAKE_SOURCE_DIR}/class/transaction.cpp
    ${CMAKE_SOURCE_DIR}/class/blockheader.cpp
    ${CMAKE_SOURCE_DIR}/class/block.cpp
    ${CMAKE_SOURCE_DIR}/class/key_seeder.cpp
    ${CMAKE_SOURCE_DIR}/class/merkle.cpp
)

# Include directories
target_include_directories(main PRIVATE ${MY_INCLUDE_DIRS})

# Link libraries
target_link_libraries(main PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    ${ROCKSDB_LIB_DIR}/librocksdb.dylib
)
