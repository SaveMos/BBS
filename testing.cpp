#include <iostream>
#include <string>
#include "utilityFile.h"
#include "security.h"
#include "timestampLibrary.h"
#include "configuration.h"

#include "messageStructures/RSAEMessage.h"

using namespace std;

int main(int argc, char *argv[])
{
    /*
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
    
   string ts = "2024-06-20 11:17:03.958";
   string ta = "2024-06-20 11:17:43.958";

   cout << secondDifference(ts , ta) << endl;
   cout << secondDifference(ta , ts) << endl;

   cout << getCurrentTimestamp() << endl;
    
   cout<< generateRandomSalt(4)<<endl;
   cout<< generateRandomSalt(4)<<endl;
   cout<< generateRandomSalt(4)<<endl;
   cout<< generateRandomSalt(4)<<endl;
   cout<< generateRandomSalt(4)<<endl;

   std::string publicKey, privateKey;

    generateRSAKeyPair(publicKey, privateKey);

    std::cout << "Public Key: " << std::endl << publicKey << std::endl;
    std::cout << "Private Key: " << std::endl << privateKey << std::endl;

    generateRSAKeyPair(publicKey, privateKey);

    std::cout << "Public Key: " << std::endl << publicKey << std::endl;
    std::cout << "Private Key: " << std::endl << privateKey << std::endl;

    generateRSAKeyPair(publicKey, privateKey);

    std::cout << "Public Key: " << std::endl << publicKey << std::endl;
    std::cout << "Private Key: " << std::endl << privateKey << std::endl;

    generateRSAKeyPair(publicKey, privateKey);

    std::cout << "Public Key: " << std::endl << publicKey << std::endl;
    std::cout << "Private Key: " << std::endl << privateKey << std::endl;

    generateRSAKeyPair(publicKey, privateKey);

    std::cout << "Public Key: " << std::endl << publicKey << std::endl;
    std::cout << "Private Key: " << std::endl << privateKey << std::endl;
    */

    RSAEMessage mess;
    string conc , check;
    int R = 100;

    mess.setCert("CERTIFICATOAOOO");
    mess.setPublicKey(loadRSAKey(true));
    
    mess.computeDigitalFirm(R);

    cout << "1 Esito: " << mess.verifyDigitalFirm(R) << endl;
    mess.concatenateFields(check);

    mess.deconcatenateAndAssign(check);
    mess.computeDigitalFirm(R);
    cout << "1 Esito: " << mess.verifyDigitalFirm(R) << endl;
    R = 120;
    cout << "0 Esito: " << mess.verifyDigitalFirm(R) << endl;
    mess.concatenateFields(check);

    mess.deconcatenateAndAssign(check);
    R = 120;
    mess.computeDigitalFirm(R);
    cout << "1 Esito: " << mess.verifyDigitalFirm(R) << endl;
    mess.concatenateFields(check);

}