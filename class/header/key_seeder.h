#ifndef KEY_SEEDER_H
#define KEY_SEEDER_H

#include <openssl/evp.h>

class KeySeeder {
public:
    // Generate temporary EC key pair (for testing)
    static EVP_PKEY* generateTestKey();
    
    // Free the key
    static void freeKey(EVP_PKEY* pkey);
};

#endif
