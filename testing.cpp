#include <iostream>
#include <string>
#include "utilityFile.h"
#include "security.h"
#include "timestampLibrary.h"
#include "configuration.h"

using namespace std;

int main(int argc, char *argv[])
{
    string plaintext = "Hello, AES!";
    string key = "0123456789abcdef0123456789abcdef"; // 32-byte (256-bit) key
    const string iv = "1234567890abcdef";                  // 16-byte (128-bit) IV
    string encryptedMessage , decryptedMessage;

    vector<unsigned char> enc_msg = encrypt_AES(plaintext, key);
    cout << "AES Decrypted Message: " << decrypt_AES(enc_msg, key) << endl;

    encryptedMessage = rsa_encrypt(plaintext , loadRSAKey(PUBLIC_KEY_PATH , true));
    decryptedMessage = rsa_decrypt(encryptedMessage , loadRSAKey(PRIVATE_KEY_PATH, false));
    cout << "RSA Decrypted Message: " <<  decryptedMessage << endl;


    const EVP_MD* md = EVP_sha256();
    cout << "HMAC: " << calculateHMAC(key , plaintext , md) << endl;
    //plaintext.append("hjjhhjhjhjh");
    //key = "0123456789abcdef0123456789abcdej";
   
    const EVP_MD* md1 = EVP_sha256();
    cout << "HMAC1: " << calculateHMAC(key , plaintext , md1) << endl;
}