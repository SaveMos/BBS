#include <iostream>
#include <string>
#include "utilityFile.h"
#include "security.h"
#include "timestampLibrary.h"

using namespace std;

int main(int argc, char *argv[])
{
    string plaintext = "Hello, AES!";
    string key = "0123456789abcdef0123456789abcdef"; // 32-byte (256-bit) key
    string iv = "1234567890abcdef";                  // 16-byte (128-bit) IV

    cout << plaintext << endl;
    vector<unsigned char> enc_msg = encrypt_AES(plaintext, key);
    cout << enc_msg.data() << endl;
    cout << decrypt_AES(enc_msg, key) << endl;

    string encryptedMessage = rsa_encrypt(plaintext , loadRSAKey("keyStorage/rsa_pubkey.pem" , true));
    cout << "Encrypted Message: " << encryptedMessage << endl;

    string decryptedMessage = rsa_decrypt(encryptedMessage , loadRSAKey("keyStorage/rsa_privkey.pem", false));
    cout << "Decrypted Message: " << decryptedMessage << endl;

    const EVP_MD* md = EVP_sha256();
    cout << calculateHMAC(key , plaintext , md);
}