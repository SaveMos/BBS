
#include "communicationLibrary.h"
#include "configuration.h"
#include "userBBS.h"
#include <mutex>
#include <vector>
#include <thread>
#include <iostream>
#include <chrono>

using namespace std;

int main()
{
    // Variables declaration.
    struct sockaddr_in server_addr; // Socket of the server.
    int sd, ret;

    // Dichiarazione Set
    
    int fd_max;
    fd_set read_fs;
    fd_set master;
    

    // Client program.
    sd = socket(AF_INET, SOCK_STREAM, 0);         // The main socket descriptor.
    memset(&server_addr, 0, sizeof(server_addr)); // Pulisco la zona di memoria.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        printf("Errore nella Connessione\n");
        exit(1);
    }

    cout<<"************ WELCOME *************"<<endl;
 
    string requestString = "none";
    string p_mail = "pini@gmail.com";
    string p_nick = "Pini";
    string p_pwd = "Pini";

    do{
        cout<<"Insert 'reg' for register or insert 'log' for login"<<endl;
        cin >> requestString;
    }while(requestString != "reg" && requestString != "log");   //48 is the ASCII code of character 1

    if (requestString == "reg"){

        // Registration test
        sendIntegerNumber(sd, REGISTRATION_REQUEST_TYPE); // I want to registrate, so i send 0.

        sendString(sd, p_mail);
        sendString(sd, p_nick);
        sendString(sd, p_pwd);

        int requestType = receiveIntegerNumber(sd);
        sendIntegerNumber(sd, requestType);
    }
    else if (requestString == "log"){

        // Login Test
        sendIntegerNumber(sd, LOGIN_REQUEST_TYPE); // I want to login, so i send 1.
        sendString(sd, p_nick);
        sendString(sd, p_pwd);
    }

    close(sd);

    // The client procedure

    while(true){
        cout << "Commands:\n -List \n-Get \n-Add" << endl;
        cin >> requestString;
    }

    return 0;
}
