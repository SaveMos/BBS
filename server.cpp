#include "communicationLibrary.h"
#include "configuration.h"
#include "utilityFile.h"
#include "connectionInformation.h"
#include "security.h"

#include <mutex>
#include <vector>
#include <thread>
#include <iostream>

#define MAXIMUM_NUMBER_OF_MESSAGES 2000
#define MAXIMUM_NUMBER_OF_THREAD 100
#define MAXIMUM_REQUEST_NUMBER 1000

using namespace std;
// Variabili dei Threads
mutex mutexBBS;
mutex mutexUserList;
mutex mutexOpenSSL; // Required because OpenSSL is not thread-safe.
mutex mutexConnections;

uint32_t threadNumber = 0;
uint32_t numberOfMessages = 0;
uint32_t id_tracker = 0;

vector<messageBBS> messageBoard;
vector<userBBS> userList;
vector<thread> threadMixer; // Vector of the threads descriptors.
vector<connectionInformation> connections;

string List(int n)
{
    string listed = "";
    mutexBBS.lock();
    for (uint32_t i = 0; i < n; i++)
    {
        listed += messageBoard[id_tracker - i].toListed();
    }
    mutexBBS.unlock();
    return listed;
}

messageBBS Get(int mid)
{
    if (mid <= 0 || mid > id_tracker)
    {
        return messageBoard[0]; // not valid id.
    }
    mutexBBS.lock();
    return messageBoard[mid];
    mutexBBS.unlock();
}

void Add(string title, string author, string body)
{
    messageBBS m(0, author, title, body);
    mutexBBS.lock();
    id_tracker++;
    m.setId(id_tracker);
    messageBoard.push_back(m);
    mutexBBS.unlock();
}

bool checkUserList(string inputNickname, userBBS &us)
{
    bool res = false;
    mutexUserList.lock();
    const unsigned int size = userList.size();
    for (unsigned int i = 0; i < size; i++)
    {
        //cout << userList.at(i).getNickname() << " " << inputNickname << endl;
        if (userList[i].getNickname() == inputNickname)
        {
            us = userList[i];
            res= true;
            break;
        }
    }
    mutexUserList.unlock();
    return res;
    
}

bool checkUserList(string inputNickname)
{
    bool res = false;
    mutexUserList.lock();
    const unsigned int size = userList.size();
    for (unsigned int i = 0; i < size; i++)
    {
        if (userList[i].getNickname() == inputNickname)
        {
            res = true;
            break;
        }
    }
    mutexUserList.unlock();
    return res;
}

void refreshConnectionInformation(string inputNickname, int sd , string type)
{
    const unsigned int size = connections.size();
    for (unsigned int i = 0; i < size; i++)
    {
        if (connections[i].getNickname() == inputNickname)
        {
            if(type == "login"){
                connections[i].refreshLogin(sd);
                break;
            } else if (type == "logout"){
                connections[i].refreshLogout();
                break;

            }
        }
    }
}

void updateUserListFile()
{
    // Aggiorna il file della user list.
}

void updateBBSFile()
{
    // Aggiorna il file della BBS.
}

void updateConnectionFile()
{
    // Aggiorna il file della connection list.
}

void registrationThread(int socketDescriptor, bool &result, string &status)
{
    string emailRecv, nickNameRecv, pwdRecv; // Receiving variables.

    emailRecv = receiveString(socketDescriptor); // Receive the email from the client.

    nickNameRecv = receiveString(socketDescriptor); // Receive the nickname from the client.

    pwdRecv = computeHash(receiveString(socketDescriptor)); // Receive the clear password from the client; and immediately compute the hash.

    if (!checkEmailFormat(emailRecv))
    {
        result = false;
        status = "ERROR - Wrong email format";
        return;
    }

    if (emailRecv.length() == 0 || nickNameRecv.length() == 0 || pwdRecv.length() == 0)
    {
        result = false;
        status = "ERROR - Some empty fields";
        return;
    }

    if (!checkUserList(nickNameRecv))
    {
        // Check if the nickname have already been used.

        uint32_t sendChallenge = 1234, recvChallenge = 1234; // The challenge.

        sendIntegerNumber(socketDescriptor, sendChallenge);     // Send the challenge.
        recvChallenge = receiveIntegerNumber(socketDescriptor); // Receive the challenge.

        if (recvChallenge == sendChallenge)
        {
            // Challenge win!
            userBBS userRecv(nickNameRecv, emailRecv,  "");
            userRecv.setPasswordDigest(pwdRecv);

            mutexUserList.lock();
            userList.push_back(userRecv);
            updateUserListFile();
            mutexUserList.unlock();

            connectionInformation c(socketDescriptor, nickNameRecv, getCurrentTimestamp(), getCurrentTimestamp(), true);

            mutexConnections.lock();
            connections.push_back(c);
            updateConnectionFile();
            mutexConnections.unlock();

            result = true;
            status = "User registered!";
            sendIntegerNumber(socketDescriptor , 1);
            return;
        }
    }
    result = false;
    status = "Something went wrong";
    sendIntegerNumber(socketDescriptor , 0);
}

void loginThread(int socketDescriptor, bool &result, string &status)
{
    string nickNameRecv, pwdRecv; // Receiving variables.
    userBBS usr;
    bool res = false;

    nickNameRecv = receiveString(socketDescriptor); // Receive the nickname from the client.
    pwdRecv = computeHash(receiveString(socketDescriptor)); // Receive the clear password from the client; and immediately compute the hash.

   
    if (nickNameRecv.length() == 0 || pwdRecv.length() == 0)
    {
        result = false;
        status = "ERROR - Some empty fields";
        return;
    }
   
    res = checkUserList(nickNameRecv, usr);
 
    if (res)
    {
        // Check if the nickname actually exists
        if (usr.getPasswordDigest() == pwdRecv)
        {
            mutexConnections.lock();
            refreshConnectionInformation(nickNameRecv, socketDescriptor, "login");
            updateConnectionFile();
            mutexConnections.unlock();
            
            result = true;
            status = "Login ok!";
            sendIntegerNumber(socketDescriptor , 1);
            return;
        }
        else
        {
            result = false;
            status = "Wrong password";
            sendIntegerNumber(socketDescriptor , -1);
            return;
        }
    }
    result = false;
    status = "Something went wrong";
    sendIntegerNumber(socketDescriptor , 0);
}

void BBSsession(int socketDescriptor){
    int requestType = 0;
    while(true){
         requestType = receiveIntegerNumber(socketDescriptor); // Get the request type from the client.
                   
        if (requestType == LIST_REQUEST_TYPE){

        } else if (requestType == GET_REQUEST_TYPE){

        } else if (requestType == ADD_REQUEST_TYPE){

        } else if (requestType == LOGOUT_REQUEST_TYPE){
            mutexConnections.lock();
            refreshConnectionInformation(nickNameRecv, socketDescriptor, "logout");
            updateConnectionFile();
            mutexConnections.unlock();
            return;
        }
    }
}

int main()
{
    messageBBS dummy(0, "err", "err", "err");
    messageBoard.push_back(dummy);

    // Variables declaration.
    int ret = 0, new_sd = 0, len = 0, i = 0;
    int request_socket = 0; // Listening socket.
    struct sockaddr_in my_addr, client_addr;
    int fd_max = 0; // Maximum descriptor.
    fd_set read_fs; // Set of copy sockets.
    fd_set master;  // Set of the main sockets.

    request_socket = socket(AF_INET, SOCK_STREAM, 0); // Creo un socket TCP di ascolto per il Server.
    memset(&my_addr, 0, sizeof(my_addr));             // Pulisco la zona di memoria.
    my_addr.sin_family = AF_INET;                     // La famiglia dell'indirizzo IP del mio server.
    my_addr.sin_port = htons(SERVER_PORT);            // The server port.
    my_addr.sin_addr.s_addr = INADDR_ANY;             // Si ascolta su tutte le interfacce di rete della Macchina.
    inet_pton(AF_INET, SERVER_IP, &my_addr.sin_addr);

    cout << "Binding..." << endl;
    ret = bind(request_socket, (struct sockaddr *)&my_addr, sizeof(my_addr)); // The binding operation.
    cout << "Binding ok!" << endl;

    ret = listen(request_socket, MAXIMUM_REQUEST_NUMBER); // Put the socket in the listening state.

    cout << "Listening..." << endl;
    if (ret == -1)
    {
        perror("SERVER ERROR: Error in the listen!\n");
        exit(1);
    }

    FD_ZERO(&master);  // Initialize the master set.
    FD_ZERO(&read_fs); // Initialize the copy set.

    FD_SET(request_socket, &master); // Inserisco nel set il socket di ascolto.
    FD_SET(STANDARD_INPUT, &master); // Insert the standard input socket descriptor.

    fd_max = request_socket;         // L'ultimo inserito è request_socket.
    int requestType = 0;

    bool howItEnded = false; // It becomes true if the procedure goes fine.
    string status = ""; // Explain the result of the procedure.

    while (true)
    {
        read_fs = master;
       // cout << "select" << endl;
        select(fd_max + 1, &read_fs, NULL, NULL, NULL);

        for (i = 0; i <= fd_max; i++)
        {
            if (FD_ISSET(i, &read_fs))
            {
                if (i == request_socket)
                {
                    // It is the request socket that have received the current request.
                    len = sizeof(client_addr);

                    new_sd = accept(request_socket, (struct sockaddr *)&client_addr, (socklen_t *)&len); // The server accept the request.

                    FD_SET(new_sd, &master);

                    if (new_sd > fd_max)
                            fd_max = new_sd;
                    
                    requestType = receiveIntegerNumber(new_sd); // Get the request type from the client.
                   
                    if (requestType == REGISTRATION_REQUEST_TYPE) // Registration.
                    {
                        howItEnded = false; // It becomes true if the procedure goes fine.
                        status = ""; // Explain the result of the procedure.
                        registrationThread(new_sd, howItEnded, status);
                        if(howItEnded){
                            BBSsession(new_sd);
                        }
                       // close(new_sd);
                        continue;
                     }
                      if (requestType == LOGIN_REQUEST_TYPE) // Registration.
                    {
                        howItEnded = false; // It becomes true if the procedure goes fine.
                        status = ""; // Explain the result of the procedure.
                        loginThread(new_sd, howItEnded, status);
                        if(howItEnded){
                            BBSsession(new_sd);
                        }
                       // close(new_sd);
                        continue;
                    }
                }

               FD_CLR(i, &master); // Not valid socket.
               close(i);           // Close the not valid socket.
            }
        }
    }
}
