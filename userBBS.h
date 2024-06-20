#include <string>
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

    void concatenateFields(string &ret)
    {
        ret = this->nickname + "|" + this->passwordDigest + "|" + this->salt + "|" + this->email + "\n";
    }

    void deconcatenateFields(vector<string> &ret, string &input)
    {
        const char delimiter = '|';
        size_t pos = input.find(delimiter); // Find the position of the first delimiter character

        while (pos != std::string::npos)
        {
            std::string parte = input.substr(0, pos); // get the first substring before the delimiter
            ret.push_back(parte);                     // add the substring to the vector
            input = input.substr(pos + 1);            // remove the extracted substring from the original string
            pos = input.find(delimiter);              // find the new position of the next delimiter character
        }
        ret.push_back(input); // add the last substring to the vector
    }

    void deconcatenateAndAssign(string &input)
    {
        vector<string> fields;
        this->deconcatenateFields(fields, input);
        this->setNickname(fields.at(0));
        this->setPasswordDigest(fields[1]);
        this->setSalt(fields[2]);
        this->setEmail(fields[3]);
    }

    bool checkPassword(string& inputPassword)
    {
        string tempInputPassword = inputPassword + this->salt ;
        if (this->computeHash(tempInputPassword) == this->passwordDigest)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    string computeHash(string& inputString)
    {
        return inputString;
    }
};

#endif
