#ifndef HELPER_H
#define HELPER_H

#include <iostream>

class Helper {
    public:
        // Generate RSA keypair and save to PEM files
        static void generateRSAKeyPair(const std::string &privatePath,
            const std::string &publicPath,
            int bits = 2048);

        // New: return keys as strings
        static std::pair<std::string, std::string> generateRSAKeyPairString(int bits = 2048);

        static std::string stripPEMHeaders(const std::string& pem);
};

#endif