#include <iostream>
#include <string>
#include "utilityFile.h"

#include "timestampLibrary.h"
#include "configuration.h"
#include "messagePackingLibrary.h"
#include "security.h"


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
/*
    mess.setCert("CERTIFICATOAOOO");
    mess.setPublicKey(loadRSAKey(true));
    mess.setDigitalFirm(createDigitalSignature(mess.getPublicKey()))

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
    userBBS user;
    user.setNickname("Pini");
    user.setEmail("pini@gmail.com");
    user.setSalt(generateRandomSalt());
    string password = "Pini";
    user.setPasswordDigest(computeHash(password, user.getSalt()));
    if (!checkSaltedPasswordDigest(password, user.getPasswordDigest(), user.getSalt()))
    {
        cout << "La prima verifica della password utente NON funziona" << endl;
        global_test = false;
    }

    password = password + "a";
    if (checkSaltedPasswordDigest(password, user.getPasswordDigest(), user.getSalt()))
    {
        cout << "La seconda verifica della password utente NON funziona" << endl;
        global_test = false;
    }

    password = " " + password;
    if (checkSaltedPasswordDigest(password, user.getPasswordDigest(), user.getSalt()))
    {
        cout << "La seconda verifica della password utente NON funziona" << endl;
        global_test = false;
    }

    password = password + "-";
    if (checkSaltedPasswordDigest(password, user.getPasswordDigest(), user.getSalt()))
    {
        cout << "La seconda verifica della password utente NON funziona" << endl;
        global_test = false;
    }

    password = "-" + password;
    if (checkSaltedPasswordDigest(password, user.getPasswordDigest(), user.getSalt()))
    {
        cout << "La seconda verifica della password utente NON funziona" << endl;
        global_test = false;
    }
*/
    string aes_key = "Pippo";
    string msg = "Pluto";
    string c = vectorUnsignedCharToString(encrypt_AES(msg , aes_key));

    if (decrypt_AES(c , aes_key) != msg)
    {
        cout << "La prima verifica di AES NON funziona!" << endl;
        global_test = false;
    }

    if (!(decrypt_AES(c , aes_key) != (msg + " ")))
    {
        cout << "La prima verifica di AES NON funziona!" << endl;
        global_test = false;
    }

    if (global_test)
    {
        cout << "Tutto OK!" << endl;
    }


    




    return 0;
}