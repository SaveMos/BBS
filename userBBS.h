#include <string>
#include <sstream>

using namespace std;

#ifndef USERBBS_H
#define USERBBS_H

class userBBS
{
private:
    string nickname;
    string email;
    string salt;
    string passwordDigest;

public:
    // Costruttore di default
    userBBS() {}

    // Costruttore che inizializza nickname e passwordDigest
    userBBS(string newNickname, string salt, string newPasswordDigest, string newEmail)
    {
        this->nickname = newNickname;
        this->email = newEmail;
        this->salt = salt;
        this->passwordDigest = newPasswordDigest;
    }

    // Metodo set per impostare il valore di nickname
    void setNickname(string newNickname)
    {
        nickname = newNickname;
    }

    // Metodo set per impostare il valore di passwordDigest
    void setPasswordDigest(string newPasswordDigest)
    {
        passwordDigest = newPasswordDigest;
    }

    void setEmail(string newEmail)
    {
        this->email = newEmail;
    }

    void setSalt(string salt)
    {
        this->salt = salt;
    }

    // Metodo get per ottenere il valore di nickname
    string getNickname()
    {
        return this->nickname;
    }

    string getEmail()
    {
        return this->email;
    }
    // Metodo get per ottenere il valore di passwordDigest
    string getPasswordDigest()
    {
        return this->passwordDigest;
    }

    string getSalt()
    {
        return this->salt;
    }

    void concatenateFields(string &str)
    {
        ostringstream oss;
        const char delimeter = '-';
        oss << this->nickname.length() << delimeter
            << this->email.length() << delimeter
            << this->salt.length() << delimeter
            << this->passwordDigest.length() << delimeter
            << this->nickname
            << this->email
            << this->salt
            << this->passwordDigest;
        str = oss.str();
    }

    void deconcatenateAndAssign(string &input)
    {
        if (input.size() <= 15)
        {
            return;
        }

        vector<string> stringVector;
        this->deconcatenateFields(stringVector, input);
        if (stringVector.size() == 4)
        {
            this->nickname = stringVector[0];
            this->email = stringVector[1];
            this->salt = stringVector[2];
            this->passwordDigest = stringVector[3];
        }
    }

    void deconcatenateFields(vector<string> &result, string &input)
    {
        result.clear();
        istringstream iss(input);
        string part;

        // Read the lengths
        int lengthNickname, lengthEmail, lengthSalt, lengthPasswordDigest;
        getline(iss, part, '-');
        lengthNickname = stoi(part);
        getline(iss, part, '-');
        lengthEmail = stoi(part);
        getline(iss, part, '-');
        lengthSalt = stoi(part);
        getline(iss, part, '-');
        lengthPasswordDigest = stoi(part);

        // Read the actual fields based on the lengths
        result.push_back(input.substr(iss.tellg(), lengthNickname));
        iss.seekg(iss.tellg() + streampos(lengthNickname));
        result.push_back(input.substr(iss.tellg(), lengthEmail));
        iss.seekg(iss.tellg() + streampos(lengthEmail));
        result.push_back(input.substr(iss.tellg(), lengthSalt));
        iss.seekg(iss.tellg() + streampos(lengthSalt));
        result.push_back(input.substr(iss.tellg(), lengthPasswordDigest));
    }

};

#endif
