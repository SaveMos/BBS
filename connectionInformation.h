#include <string>
#include "timestampLibrary.h" // Assumo che questa libreria definisca getCurrentTimestamp()
using namespace std;

class connectionInformation {
private:
    int socketDescriptor;
    string nickname;
    string loginTimestamp;
    string lastActivityTimeStamp;
    bool logged;

public:
    // Costruttore
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

    // Metodi 'get' per ottenere i valori degli attributi
    int getSocketDescriptor() const {
        return socketDescriptor;
    }

    string getNickname() const {
        return nickname;
    }

    string getLoginTimestamp() const {
        return loginTimestamp;
    }

    string getLastActivityTimeStamp() const {
        return lastActivityTimeStamp;
    }

    bool isLogged() const {
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
};


/*
 
*/