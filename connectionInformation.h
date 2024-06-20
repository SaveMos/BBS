#include <string>
#include "timestampLibrary.h" // Assumo che questa libreria definisca getCurrentTimestamp()
using namespace std;

#ifndef CONNECTIONINFORMATION_H
#define CONNECTIONINFORMATION_H

#define CONNECTION_VALIDITY_PERIOD 60 * 20 // 20 minutes


class connectionInformation {
private:
    uint64_t socketDescriptor;
    string nickname;
    string loginTimestamp;
    string lastActivityTimeStamp;
    uint8_t logged;

public:
    connectionInformation(){
    }

    connectionInformation(int sd, string nickname, string login, string actv, bool logg) {
        this->socketDescriptor = sd;
        this->nickname = nickname;
        this->loginTimestamp = login;
        this->lastActivityTimeStamp = actv;
        this->logged = logg;
    }

    void refreshLogin(int sd){
        this->socketDescriptor = sd;
        this->loginTimestamp = getCurrentTimestamp();
        this->lastActivityTimeStamp = this->loginTimestamp;
        this->logged = true;
    }

    void refreshLogout(){
        this->refreshLastActionTimestamp();
        this->logged = false;
    }

    void refreshLastActionTimestamp(){
        this->lastActivityTimeStamp = getCurrentTimestamp();
    }

    // Metodi 'get' per ottenere i valori degli attributi
    uint64_t getSocketDescriptor()  {
        return socketDescriptor;
    }

    string getNickname()  {
        return nickname;
    }

    string getLoginTimestamp()  {
        return loginTimestamp;
    }

    string getLastActivityTimeStamp()  {
        return lastActivityTimeStamp;
    }

    uint8_t getLogged()  {
        return logged;
    }

    // Metodi 'set' per impostare i valori degli attributi
    void setSocketDescriptor(int sd) {
        socketDescriptor = sd;
    }

    void setNickname(const string& nick) {
        nickname = nick;
    }

    // In caso si voglia cambiare manualmente il timestamp di login
    void setLoginTimestamp(const string& timestamp) {
        loginTimestamp = timestamp;
    }

    // In caso si voglia cambiare manualmente il timestamp di ultima attività
    void setLastActivityTimeStamp(const string& timestamp) {
        lastActivityTimeStamp = timestamp;
    }

    // In caso si voglia impostare manualmente lo stato di log-in
    void setLogged(bool value) {
        logged = value;
    }

    void setLogged(int value){
        if(value == 0){
            logged = 0;
        }else{
            logged = 1;
        }
    }

    void setLogged(uint64_t value){
        if(value == 0){
            logged = 0;
        }else{
            logged = 1;
        }
    }
    void setLogged(uint8_t value){
       logged = value;
    }

    bool checkValidityOfTheConnection(){
        const string currentTs = getCurrentTimestamp();
        const int diffLast = secondDifference(currentTs , this->lastActivityTimeStamp);
        const int diffLog = secondDifference(currentTs , this->loginTimestamp);
        if(
            diffLog < 0 || 
            diffLast < 0 || 
            diffLast > CONNECTION_VALIDITY_PERIOD
            ){
            return false;
        }else{
            return true;
        }
    }
};

#endif