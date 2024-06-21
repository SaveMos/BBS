#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "../security.h"
#include "../utility.h"

using namespace std;

#ifndef RSAEMESSAGE_H
#define RSAEMESSAGE_H

class RSAEMessage
{
private:
    size_t PublicKeyDim;
    size_t DigitalFirmDim;
    size_t CertDim;
    string PublicKey;
    string DigitalFirm;
    string Cert;

public:
    // Costruttore di default
    RSAEMessage() : PublicKeyDim(0), DigitalFirmDim(0), CertDim(0), PublicKey(""), DigitalFirm(""), Cert("") {}

    // Costruttore parametrizzato semplice
    RSAEMessage(const string &PublicKey, const string &DigitalFirm, const string &Cert)
        : PublicKeyDim(PublicKey.size()), DigitalFirmDim(DigitalFirm.size()), CertDim(Cert.size()),
          PublicKey(PublicKey), DigitalFirm(DigitalFirm), Cert(Cert) {}

    // Getter e Setter per PublicKeyDim
    size_t getPublicKeyDim() const
    {
        return PublicKeyDim;
    }

    void setPublicKeyDim(size_t PublicKeyDim)
    {
        this->PublicKeyDim = PublicKeyDim;
    }

    // Getter e Setter per DigitalFirmDim
    size_t getDigitalFirmDim() const
    {
        return DigitalFirmDim;
    }

    void setDigitalFirmDim(size_t DigitalFirmDim)
    {
        this->DigitalFirmDim = DigitalFirmDim;
    }

    // Getter e Setter per CertDim
    size_t getCertDim() const
    {
        return CertDim;
    }

    void setCertDim(size_t CertDim)
    {
        this->CertDim = CertDim;
    }

    // Getter e Setter per PublicKey
    string getPublicKey() const
    {
        return PublicKey;
    }

    void setPublicKey(const string &PublicKey)
    {
        this->PublicKey = PublicKey;
        this->PublicKeyDim = PublicKey.size();
    }

    void setPublicKey(string &PublicKey)
    {
        this->PublicKey = PublicKey;
        this->PublicKeyDim = PublicKey.size();
    }

    void setPublicKey(EVP_PKEY* kpub)
    {
        this->PublicKey = convertPublicEVP_PKEYToString(kpub);
        this->PublicKeyDim = PublicKey.size();
        cout << this->PublicKeyDim << endl;
    }

    // Getter e Setter per DigitalFirm
    string getDigitalFirm() const
    {
        return DigitalFirm;
    }

    void setDigitalFirm(const string &DigitalFirm)
    {
        this->DigitalFirm = DigitalFirm;
        this->DigitalFirmDim = DigitalFirm.size();
    }

    // Getter e Setter per Cert
    string getCert() const
    {
        return Cert;
    }

    void setCert(const string &Cert)
    {
        this->Cert = Cert;
        this->CertDim = Cert.size();
    }

    // Metodo per concatenare i campi in una stringa
    void concatenateFields(string &str) const
    {
        ostringstream oss;
        const char delimiter = '-';
        oss << PublicKeyDim << delimiter
            << DigitalFirmDim << delimiter
            << CertDim << delimiter
            << PublicKey
            << DigitalFirm
            << Cert;
        str = oss.str();
    }

    // Metodo per deconcatenare i campi da una stringa
    void deconcatenateFields(vector<string> &result, const string &input) const
    {
        result.clear();
        istringstream iss(input);
        string part;

        // Leggi le dimensioni
        getline(iss, part, '-');
        size_t lengthPublicKey = stoi(part);
        getline(iss, part, '-');
        size_t lengthDigitalFirm = stoi(part);
        getline(iss, part, '-');
        size_t lengthCert = stoi(part);

        // Leggi i campi
        result.push_back(input.substr(iss.tellg(), lengthPublicKey));
        iss.seekg(iss.tellg() + streampos(lengthPublicKey));
        result.push_back(input.substr(iss.tellg(), lengthDigitalFirm));
        iss.seekg(iss.tellg() + streampos(lengthDigitalFirm));
        result.push_back(input.substr(iss.tellg(), lengthCert));
    }

    // Metodo per deconcatenare e assegnare i campi da una stringa
    void deconcatenateAndAssign(const string &input)
    {
        vector<string> stringVector;
        this->deconcatenateFields(stringVector, input);
        if (stringVector.size() == 3)
        {
            this->PublicKey = stringVector[0];
            this->DigitalFirm = stringVector[1];
            this->Cert = stringVector[2];
            this->PublicKeyDim = PublicKey.size();
            this->DigitalFirmDim = DigitalFirm.size();
            this->CertDim = Cert.size();
        }
    }

    void computeDigitalFirm(int R , EVP_PKEY* privKey){
        const string conc = to_string(R) + this->getPublicKey();
        vector<unsigned char> firm = createDigitalSignature(conc , privKey);
        this->DigitalFirm = vectorUnsignedCharToString(firm);
        
    }

    void computeDigitalFirm(uint64_t R, EVP_PKEY* privKey){
        const string conc = to_string(R) + this->getPublicKey();
        this->setDigitalFirm(rsa_decrypt(conc , privKey));
    }

    bool verifyDigitalFirm(int R){
        const string conc = to_string(R) + this->getPublicKey();
        return verifyDigitalSignature(conc , this->getDigitalFirm() , convertStringToPublicEVP_PKEY(this->getPublicKey()));
    }

    bool verifyDigitalFirm(uint64_t R){
        const string conc = to_string(R) + this->getPublicKey();
        return verifyDigitalSignature(conc , this->getDigitalFirm() , convertStringToPublicEVP_PKEY(this->getPublicKey()));
    }
};
#endif
