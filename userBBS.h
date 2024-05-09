#include <string>
using namespace std;

class userBBS {
private:
    string nickname;
    string email;
    string passwordDigest;

public:
    // Costruttore di default
    userBBS() {}

    // Costruttore che inizializza nickname e passwordDigest
    userBBS(string newNickname,string newPasswordDigest, string newEmail){
        this->nickname = newNickname;
        this->email = newEmail;
        this->passwordDigest = newPasswordDigest;
    }

    // Metodo set per impostare il valore di nickname
    void setNickname(string newNickname) {
        nickname = newNickname;
    }

    // Metodo set per impostare il valore di passwordDigest
    void setPasswordDigest(string newPasswordDigest) {
        passwordDigest = newPasswordDigest;
    }

    
    void setEmail(string newEmail) {
        this->email = newEmail;
    }

    // Metodo get per ottenere il valore di nickname
    string getNickname()  {
        return this->nickname;
    }

    string getEmail()  {
        return this->email;
    }

    // Metodo get per ottenere il valore di passwordDigest
    string getPasswordDigest()  {
        return this->passwordDigest;
    }

    void concatenateFields(string &ret){
        ret = this->nickname +  "|"  + this->passwordDigest + "|" + this->email + "\n";
    }

    void deconcatenateAndAssign(string &input){
        vector<string> fields;
        deconcatenateFields(fields , input, '|');
        this->setNickname(fields.at(0));
        this->setPasswordDigest(fields[1]);
        this->setEmail(fields[2]);
    }

    bool checkPassword(string inputPassword){
        if(this->computeHash(inputPassword) == this->passwordDigest){
            return true;
        }else{
            return false;
        }
    }

    string computeHash(string inputString){
        return inputString;
    }
};
