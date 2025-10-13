add_executable(
    main 
    main.cpp
    ../helper/helper.cpp
    ../class/node.cpp
    ../class/wallet.cpp
    ../class/utxo.cpp
)
    

target_include_directories(main PRIVATE)

# Find and link OpenSSL
find_package(OpenSSL REQUIRED)

# RocksDB paths (Homebrew on Apple Silicon)
set(ROCKSDB_INCLUDE_DIR "/opt/homebrew/include")
set(ROCKSDB_LIB_DIR "/opt/homebrew/lib")

target_include_directories(main PRIVATE ${ROCKSDB_INCLUDE_DIR})

target_link_libraries(main PRIVATE 
    OpenSSL::SSL 
    OpenSSL::Crypto
    ${ROCKSDB_LIB_DIR}/librocksdb.dylib
)

# Use C++20
target_compile_features(main PRIVATE cxx_std_20)
