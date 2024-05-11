
#include <string>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
using namespace std;

string computeHash(string input){
    return input;

    // Buffer per contenere l'hash
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // Calcola l'hash SHA-256 della stringa di input
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.length());
    SHA256_Final(hash, &sha256);

    // Converti l'hash binario in una stringa esadecimale
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();

}