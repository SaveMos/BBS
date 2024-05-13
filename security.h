
#include <string>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>
using namespace std;

#ifndef SECURITY_H
#define SECURITY_H

string computeHash(string input, string algorithm = "SHA256"){
    
    //initialize EVP for hashing
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_sha256();
    //buffer to contain the hash
    unsigned char hash[SHA256_DIGEST_LENGTH];

    //compute the hash of the input string
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, input.c_str(), input.length());
    EVP_DigestFinal_ex(mdctx, hash, NULL);

    //free the memory used for EVP
    EVP_MD_CTX_free(mdctx);

    //convert the binary hash in a hexadecimal string    
    stringstream ss;
    for (uint8_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

#endif