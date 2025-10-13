#include "./header/helper.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/bio.h>
#include <regex>
#include <iostream>
#include <stdexcept>

void Helper::generateRSAKeyPair(
    const std::string &privatePath,
    const std::string &publicPath,
    int bits
) {
    BIGNUM *bne = BN_new();
    RSA *rsa = RSA_new();

    if (!bne || !rsa)
    throw std::runtime_error("Memory allocation failed for RSA");

    BN_set_word(bne, RSA_F4); // e = 65537

    if (RSA_generate_key_ex(rsa, bits, bne, nullptr) != 1) {
        RSA_free(rsa);
        BN_free(bne);
        throw std::runtime_error("RSA key generation failed");
    }

    // Write private key
    FILE *priv = fopen(privatePath.c_str(), "wb");
    PEM_write_RSAPrivateKey(priv, rsa, nullptr, nullptr, 0, nullptr, nullptr);
    fclose(priv);

    // Write public key
    FILE *pub = fopen(publicPath.c_str(), "wb");
    PEM_write_RSA_PUBKEY(pub, rsa);
    fclose(pub);

    RSA_free(rsa);
    BN_free(bne);
    std::cout << "✅ RSA keypair saved to " << privatePath << " & " << publicPath << std::endl;
}

std::pair<std::string, std::string> Helper::generateRSAKeyPairString(
    int bits
) {
    BIGNUM *bne = BN_new();
    RSA *rsa = RSA_new();

    if (!bne || !rsa)
    throw std::runtime_error("Memory allocation failed for RSA");

    BN_set_word(bne, RSA_F4);

    if (RSA_generate_key_ex(rsa, bits, bne, nullptr) != 1) {
        RSA_free(rsa);
        BN_free(bne);
        throw std::runtime_error("RSA key generation failed");
    }

    BIO *privBio = BIO_new(BIO_s_mem());
    BIO *pubBio = BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPrivateKey(privBio, rsa, nullptr, nullptr, 0, nullptr, nullptr);
    PEM_write_bio_RSA_PUBKEY(pubBio, rsa);

    char *privData = nullptr, *pubData = nullptr;
    long privLen = BIO_get_mem_data(privBio, &privData);
    long pubLen = BIO_get_mem_data(pubBio, &pubData);

    std::string privKey(privData, privLen);
    std::string pubKey(pubData, pubLen);

    BIO_free(privBio);
    BIO_free(pubBio);
    RSA_free(rsa);
    BN_free(bne);

    std::string cleanPub = Helper::stripPEMHeaders(pubKey);
    std::string cleanPriv = Helper::stripPEMHeaders(privKey);

    return {cleanPub, cleanPriv};
}

std::string Helper::stripPEMHeaders(const std::string& pem) {
    std::string result = pem;

    // Remove BEGIN/END lines
    result = std::regex_replace(result, std::regex("-----BEGIN [^-]+-----\\n"), "");
    result = std::regex_replace(result, std::regex("-----END [^-]+-----\\n"), "");

    // Remove any newlines
    result = std::regex_replace(result, std::regex("\\r?\\n"), "");

    return result;
}