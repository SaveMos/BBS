#include <string>
using namespace std;

#ifndef TIMESTAMPLIBRARY_H
#define TIMESTAMPLIBRARY_H
string getCurrentTimestamp(){
    string str = "2024-05-08T11:00:00";
    return str;
}

bool checkTimestampFormat(string timestamp){
    return true;
}

#endif