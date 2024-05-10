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

void deconcatenateFields(vector<string> &ret, string &input){

    char delimiter = '|';
        
    size_t pos = input.find(delimiter);   // Find the position of the first delimiter character

    while (pos != std::string::npos){
            
        std::string parte = input.substr(0, pos); // get the first substring before the delimiter 
        ret.push_back(parte); // add the substring to the vector
        input = input.substr(pos + 1); // remove the extracted substring from the original string
        pos = input.find(delimiter);  // find the new position of the next delimiter character
    }
        ret.push_back(input); // add the last substring to the vector
}
    void deconcatenateAndAssign(string &input){
        vector<string> fields;
        this->deconcatenateFields(fields , input);
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
