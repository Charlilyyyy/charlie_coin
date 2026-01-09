#include "./header/key_seeder.h"
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <stdexcept>

EVP_PKEY* KeySeeder::generateTestKey() {
    EVP_PKEY* pkey = nullptr;
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!pctx) throw std::runtime_error("Failed to create PKEY context");

    if (EVP_PKEY_keygen_init(pctx) <= 0) throw std::runtime_error("Failed to init keygen");
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_secp256k1) <= 0)
        throw std::runtime_error("Failed to set curve");
    if (EVP_PKEY_keygen(pctx, &pkey) <= 0) throw std::runtime_error("Key generation failed");

    EVP_PKEY_CTX_free(pctx);
    return pkey;
}

void KeySeeder::freeKey(EVP_PKEY* pkey) {
    if (pkey) EVP_PKEY_free(pkey);
}
