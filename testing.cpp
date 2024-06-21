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
    bool global_test = true;

    string plaintext = "Hello, AES!";
    string key = "0123456789abcdef0123456789abcdef"; // 32-byte (256-bit) key
    const string iv = "1234567890abcdef";            // 16-byte (128-bit) IV
    string encryptedMessage, decryptedMessage;

    vector<unsigned char> enc_msg = encrypt_AES(plaintext, key);
    // cout << "AES Decrypted Message: " << decrypt_AES(enc_msg, key) << endl;

    encryptedMessage = rsa_encrypt(plaintext, loadRSAKey(PUBLIC_KEY_PATH, true));
    decryptedMessage = rsa_decrypt(encryptedMessage, loadRSAKey(PRIVATE_KEY_PATH, false));
    // cout << "RSA Decrypted Message: " <<  decryptedMessage << endl;

    const EVP_MD *md = EVP_sha256();
    // cout << "HMAC: " << calculateHMAC(key , plaintext , md) << endl;
    // plaintext.append("hjjhhjhjhjh");
    // key = "0123456789abcdef0123456789abcdej";

    const EVP_MD *md1 = EVP_sha256();
    // cout << "HMAC1: " << calculateHMAC(key , plaintext , md1) << endl;

    string ts = "2024-06-20 11:17:03.958";
    string ta = "2024-06-20 11:17:43.958";

    int num = secondDifference(ts, ta);

    if (num != 40)
    {
        cout << "(+) secondDifference NON funziona" << endl;
        global_test = false;
    }

    num = secondDifference(ta, ts);

    if (num != -40)
    {
        cout << "(-) secondDifference NON funziona" << endl;
        global_test = false;
    }


    RSAEMessage mess;
    string conc, check;
    int R = 100;

    mess.setCert("CERTIFICATOAOOO");
    mess.setPublicKey(loadRSAKey(true));
    mess.computeDigitalFirm(R);

    if (!mess.verifyDigitalFirm(R))
    {
        cout << "La firma digitale NON funziona" << endl;
        global_test = false;
    }

    mess.concatenateFields(check);
    mess.deconcatenateAndAssign(check);
    mess.computeDigitalFirm(R);

    if (!mess.verifyDigitalFirm(R))
    {
        cout << "La firma digitale NON funziona" << endl;
        global_test = false;
    }

    R = 120;

    if (mess.verifyDigitalFirm(R))
    {
        cout << "La firma digitale NON funziona" << endl;
        global_test = false;
    }
    mess.concatenateFields(check);

    mess.deconcatenateAndAssign(check);
    R = 120;
    mess.computeDigitalFirm(R);

    if (!mess.verifyDigitalFirm(R))
    {
        cout << "La firma digitale NON funziona" << endl;
        global_test = false;
    }

    mess.concatenateFields(check);

<<<<<<< Updated upstream
    

/*  TEST CONVERSIONE DA EVP_PKEY A STRINGA
   EVP_PKEY *pkey = EVP_PKEY_new();
   EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
   if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore nell'inizializzazione della generazione della chiave");
    }
   if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore nell'impostazione della lunghezza della chiave RSA");
    }
   if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore nella generazione della chiave RSA");
   }

   // Convertire la chiave privata in una stringa
   try {
        std::string privateKeyStr = convertEVP_PKEYToString(pkey);
        std::cout << "Chiave privata: " << std::endl << privateKeyStr << std::endl;
   } catch (const std::exception &e) {
        std::cerr << "Errore: " << e.what() << std::endl;
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return 1;
   }

   // Pulire le risorse
   EVP_PKEY_free(pkey);
   EVP_PKEY_CTX_free(ctx);
   */

  /*TEST CONVERSIONE DA EVP_PKEY A vector<char>
// Esempio di generazione di una chiave RSA
    EVP_PKEY *pkey = EVP_PKEY_new();
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore nell'inizializzazione della generazione della chiave");
    }
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore nell'impostazione della lunghezza della chiave RSA");
    }
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore nella generazione della chiave RSA");
    }

    // Convertire la chiave privata in std::vector<unsigned char>
    try {
        std::vector<unsigned char> keyVec = convertEVP_PKEYToVector(pkey);
        std::cout << "Chiave privata convertita con successo in std::vector<unsigned char>!" << std::endl;

        // Converti i byte in una stringa
        std::string keyStr(keyVec.begin(), keyVec.end());

        // Visualizza la stringa
        std::cout << "Contenuto di keyVec:" << std::endl;
        std::cout << keyStr << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Errore: " << e.what() << std::endl;
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return 1;
    }

    // Pulire le risorse
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    */

=======
    userBBS user;
    user.setNickname("Pini");
    user.setEmail("pini@gmail.com");
    user.setSalt(generateRandomSalt());
    string password = "Pini";
    user.setPasswordDigest(computePasswordHash(password , user.getSalt()));
    if (!checkSaltedPasswordDigest(password  , user.getPasswordDigest() , user.getSalt()))
    {
        cout << "La prima verifica della password utente NON funziona" << endl;
        global_test = false;
    }

    password = password + "a";
    if (checkSaltedPasswordDigest(password , user.getPasswordDigest() , user.getSalt()))
    {
        cout << "La seconda verifica della password utente NON funziona" << endl;
        global_test = false;
    }

    password = " " + password;
    if (checkSaltedPasswordDigest(password , user.getPasswordDigest() , user.getSalt()))
    {
        cout << "La seconda verifica della password utente NON funziona" << endl;
        global_test = false;
    }

    password = password + "-";
    if (checkSaltedPasswordDigest(password , user.getPasswordDigest() , user.getSalt()))
    {
        cout << "La seconda verifica della password utente NON funziona" << endl;
        global_test = false;
    }

    password = "-" + password;
    if (checkSaltedPasswordDigest(password , user.getPasswordDigest() , user.getSalt()))
    {
        cout << "La seconda verifica della password utente NON funziona" << endl;
        global_test = false;
    }

    if(global_test){
        cout << "Tutto OK!" << endl;
    }

    return 0;
>>>>>>> Stashed changes
}