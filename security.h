#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/hmac.h>

#include "utilityFile.h"
#include "configuration.h"

using namespace std;

#ifndef SECURITY_H
#define SECURITY_H

void handleErrors()
{
    ERR_print_errors_fp(stderr);
    abort();
}

string computeHash(string input)
{
    // Initialize EVP for hashing
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha256();
    // Buffer to contain the hash
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // Compute the hash of the input string
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, input.c_str(), input.length());
    EVP_DigestFinal_ex(mdctx, hash, NULL);

    // Free the memory used for EVP
    EVP_MD_CTX_free(mdctx);

    // Convert the binary hash to a hexadecimal string
    stringstream ss;
    for (uint8_t i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

std::string calculateHMAC(const std::string &key, const std::string &message, const EVP_MD *evp_md)
{
    EVP_PKEY *pkey = nullptr;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    unsigned char *result = nullptr;
    size_t result_len = 0;
    std::string hmac;

    if (!ctx)
    {
        throw std::runtime_error("Errore nella creazione del contesto EVP_MD_CTX");
    }

    pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, nullptr, (const unsigned char *)key.c_str(), key.size());
    if (!pkey)
    {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Errore nella creazione della chiave HMAC");
    }

    if (EVP_DigestSignInit(ctx, nullptr, evp_md, nullptr, pkey) != 1)
    {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Errore nell'inizializzazione dell'HMAC con EVP_DigestSignInit");
    }

    if (EVP_DigestSignUpdate(ctx, message.c_str(), message.length()) != 1)
    {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Errore nell'aggiornamento del contesto HMAC con il messaggio");
    }

    if (EVP_DigestSignFinal(ctx, nullptr, &result_len) != 1)
    {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Errore nel recupero della dimensione del risultato HMAC");
    }

    result = (unsigned char *)OPENSSL_malloc(result_len);
    if (!result)
    {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Errore nell'allocazione della memoria per il risultato HMAC");
    }

    if (EVP_DigestSignFinal(ctx, result, &result_len) != 1)
    {
        OPENSSL_free(result);
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw std::runtime_error("Errore nella finalizzazione del calcolo HMAC");
    }

    // Conversione del risultato HMAC in una stringa esadecimale
    char hex_result[result_len * 2 + 1]; // Per memorizzare la rappresentazione esadecimale del risultato

    for (size_t i = 0; i < result_len; i++)
        sprintf(hex_result + i * 2, "%02x", result[i]);

    hex_result[result_len * 2] = '\0'; // Terminazione della stringa

    hmac = hex_result;

    OPENSSL_free(result);
    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    return hmac;
}

std::vector<unsigned char> stringToUnsignedCharVector(const std::string &str)
{
    return std::vector<unsigned char>(str.begin(), str.end());
}

void stringToCharArray(const std::string &str, char *charArray, size_t maxLen)
{
    std::strncpy(charArray, str.c_str(), maxLen - 1);
    charArray[maxLen - 1] = '\0'; // Ensure null termination
}

// Funzione per cifrare una stringa
std::vector<unsigned char> encrypt_AES(const std::string &plaintext, const std::string &key)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        throw std::runtime_error("Errore nella creazione del contesto di cifratura");
    }

    // Genera un IV casuale
    unsigned char iv[EVP_MAX_IV_LENGTH];
    if (!RAND_bytes(iv, EVP_CIPHER_iv_length(EVP_aes_256_cbc())))
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Errore nella generazione dell'IV");
    }

    // Inizializzazione della cifratura
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char *>(key.data()), iv) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Errore nell'inizializzazione della cifratura");
    }

    // Aggiungi padding al plaintext
    int plaintext_len = plaintext.size();
    int ciphertext_len = plaintext_len + EVP_CIPHER_block_size(EVP_aes_256_cbc());
    std::vector<unsigned char> ciphertext(ciphertext_len + EVP_CIPHER_iv_length(EVP_aes_256_cbc()));

    // Copia l'IV nel ciphertext
    memcpy(ciphertext.data(), iv, EVP_CIPHER_iv_length(EVP_aes_256_cbc()));

    int len;
    unsigned char *ciphertext_ptr = ciphertext.data() + EVP_CIPHER_iv_length(EVP_aes_256_cbc());

    if (EVP_EncryptUpdate(ctx, ciphertext_ptr, &len, reinterpret_cast<const unsigned char *>(plaintext.data()), plaintext_len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Errore nella cifratura del testo");
    }
    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext_ptr + len, &len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Errore nella finalizzazione della cifratura");
    }
    ciphertext_len += len;

    // Truncate the ciphertext to the actual size
    ciphertext.resize(ciphertext_len + EVP_CIPHER_iv_length(EVP_aes_256_cbc()));

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext;
}

// Funzione per decifrare una stringa
std::string decrypt_AES(const std::vector<unsigned char> &ciphertext, const std::string &key)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        throw std::runtime_error("Errore nella creazione del contesto di decifratura");
    }

    // Estrai l'IV dal ciphertext
    unsigned char iv[EVP_MAX_IV_LENGTH];
    memcpy(iv, ciphertext.data(), EVP_CIPHER_iv_length(EVP_aes_256_cbc()));

    // Inizializzazione della decifratura
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char *>(key.data()), iv) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Errore nell'inizializzazione della decifratura");
    }

    // Decifra il testo
    int ciphertext_len = ciphertext.size() - EVP_CIPHER_iv_length(EVP_aes_256_cbc());
    std::vector<unsigned char> paddedPlaintext(ciphertext_len);

    int len;
    if (EVP_DecryptUpdate(ctx, paddedPlaintext.data(), &len, ciphertext.data() + EVP_CIPHER_iv_length(EVP_aes_256_cbc()), ciphertext_len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Errore nella decifratura del testo");
    }
    int plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, paddedPlaintext.data() + len, &len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Errore nella finalizzazione della decifratura");
    }
    plaintext_len += len;

    // Truncate the plaintext to the actual size
    paddedPlaintext.resize(plaintext_len);

    EVP_CIPHER_CTX_free(ctx);
    return std::string(paddedPlaintext.begin(), paddedPlaintext.end());
}

// Funzione per convertire std::vector<unsigned char> in EVP_PKEY*
EVP_PKEY *convertToEVP_PKEY(const std::vector<unsigned char> &privateKeyVec)
{
    const unsigned char *keyData = privateKeyVec.data();
    BIO *bio = BIO_new_mem_buf(keyData, privateKeyVec.size());
    if (!bio)
    {
        throw std::runtime_error("Errore nella creazione del BIO");
    }

    // Legge la chiave privata RSA dal BIO
    EVP_PKEY *pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
    if (!pkey)
    {
        BIO_free(bio);
        throw std::runtime_error("Errore nella lettura della chiave privata dal BIO");
    }

    BIO_free(bio);
    return pkey;
}

// Funzione per convertire una stringa contenente una chiave privata in EVP_PKEY*
EVP_PKEY *convertToEVP_PKEY(const std::string &privateKeyStr)
{
    BIO *bio = BIO_new_mem_buf(privateKeyStr.data(), privateKeyStr.size());
    if (!bio)
    {
        throw std::runtime_error("Errore nella creazione del BIO");
    }

    // Legge la chiave privata RSA dal BIO
    EVP_PKEY *pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
    if (!pkey)
    {
        BIO_free(bio);
        throw std::runtime_error("Errore nella lettura della chiave privata dal BIO");
    }

    BIO_free(bio);
    return pkey;
}

// Funzione per caricare una chiave RSA da file
EVP_PKEY *loadRSAKey(const char *path, const bool public_key)
{
    EVP_PKEY *pkey = nullptr;
    FILE *file = fopen(path, "r");

    if (!file)
    {
        throw std::runtime_error("RSA Keys file cannot be opened!");
    }

    if (public_key)
    {
        pkey = PEM_read_PUBKEY(file, nullptr, nullptr, nullptr);
    }
    else
    {
        pkey = PEM_read_PrivateKey(file, nullptr, nullptr, (void*)PRIVATE_ENC);
    }

    fclose(file);

    if (!pkey)
    {
        throw std::runtime_error("RSA Keys cannot be loaded!");
    }

    return pkey;
}

// Funzione per caricare una chiave RSA da file
EVP_PKEY *loadRSAKey(const bool public_key)
{
    EVP_PKEY *pkey = nullptr;

    if (public_key)
    {
        FILE *file = fopen(PUBLIC_KEY_PATH, "r");
        if (!file)
        {
            throw std::runtime_error("RSA Public Key file cannot be opened!");
        }
        pkey = PEM_read_PUBKEY(file, nullptr, nullptr, nullptr);
        if (!pkey)
        {
            throw std::runtime_error("RSA Public Key cannot be loaded!");
        }
        fclose(file);
    }
    else
    {
        FILE *file = fopen(PRIVATE_KEY_PATH, "r");
        if (!file)
        {
            throw std::runtime_error("RSA Private Key file cannot be opened!");
        }
        pkey = PEM_read_PrivateKey(file, nullptr, nullptr, (void*)PRIVATE_ENC);
        if (!pkey)
        {
            throw std::runtime_error("RSA Private Key cannot be loaded!");
        }
        fclose(file);
    }

    return pkey;
}

std::string rsa_encrypt(const std::string &plainText, EVP_PKEY *pkey)
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    EVP_PKEY_free(pkey);
    if (ctx == nullptr)
    {
        throw std::runtime_error("Errore creazione contesto EVP_PKEY_CTX per cifratura");
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore inizializzazione operazione di cifratura");
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore impostazione padding RSA");
    }

    size_t outlen;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outlen, (const unsigned char *)plainText.c_str(), plainText.length()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore determinazione dimensione buffer per cifratura");
    }

    std::string cipherText(outlen, '\0');
    if (EVP_PKEY_encrypt(ctx, (unsigned char *)cipherText.data(), &outlen, (const unsigned char *)plainText.c_str(), plainText.length()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore cifratura dati");
    }

    EVP_PKEY_CTX_free(ctx);

    return cipherText;
}

std::string rsa_decrypt(const std::string &cipherText, EVP_PKEY *pkey)
{

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    EVP_PKEY_free(pkey);
    if (ctx == nullptr)
    {
        throw std::runtime_error("Errore creazione contesto EVP_PKEY_CTX per decifratura");
    }

    if (EVP_PKEY_decrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore inizializzazione operazione di decifratura");
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore impostazione padding RSA");
    }

    size_t outlen;
    if (EVP_PKEY_decrypt(ctx, nullptr, &outlen, (const unsigned char *)cipherText.c_str(), cipherText.length()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore determinazione dimensione buffer per decifratura");
    }

    std::string plainText(outlen, '\0');
    if (EVP_PKEY_decrypt(ctx, (unsigned char *)plainText.data(), &outlen, (const unsigned char *)cipherText.c_str(), cipherText.length()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Errore decifratura dati");
    }

    EVP_PKEY_CTX_free(ctx);

    return plainText;
}

#endif
