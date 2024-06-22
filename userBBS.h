#include <string>
#include <vector>
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
    uint64_t counter;

public:
    // Costruttore di default
    userBBS() : counter(0) {}

    // Costruttore che inizializza nickname, salt, passwordDigest, email e counter
    userBBS(string newNickname, string salt, string newPasswordDigest, string newEmail, uint64_t newCounter = 0)
    {
        this->nickname = newNickname;
        this->email = newEmail;
        this->salt = salt;
        this->passwordDigest = newPasswordDigest;
        this->counter = newCounter;
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

    void setCounter(uint64_t c)
    {
        this->counter = c;
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

    uint64_t getCounter()
    {
        return this->counter;
    }

    void incrCounter(uint64_t howMuch = 1)
    {
        this->counter += howMuch;
    }

    void concatenateFields(string &str)
    {
        ostringstream oss;
        const char delimiter = '-';
        oss << this->nickname.length() << delimiter
            << this->email.length() << delimiter
            << this->salt.length() << delimiter
            << this->passwordDigest.length() << delimiter
            << this->nickname
            << this->email
            << this->salt
            << this->passwordDigest
            << delimiter << this->counter;
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
        if (stringVector.size() == 5)
        {
            this->nickname = stringVector[0];
            this->email = stringVector[1];
            this->salt = stringVector[2];
            this->passwordDigest = stringVector[3];
            this->counter = stoull(stringVector[4]);
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
        iss.seekg(iss.tellg() + streampos(lengthPasswordDigest));

        // Read the counter
        getline(iss, part, '-');
        result.push_back(part);
    }
};

#endif
