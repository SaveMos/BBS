#include "communicationLibrary.h"
#include "configuration.h"
#include "utilityFile.h"
#include "connectionInformation.h"
#include "security.h"

#include <mutex>
#include <vector>
#include <thread>
#include <iostream>

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
    if (n <= 0)
    {
        return listed;
    }
    mutexBBS.lock();
    if (numberOfMessages < n)
    {
        n = numberOfMessages;
    }

    uint32_t id = 0, i;
    for (i = 0; i < n; i++)
    {
        id = messageBoard[id_tracker - i].getId();
        if (id == 0)
        {
            continue; // Not valid post, so we go on.
        }
        listed += to_string(id) + " - " + messageBoard[id_tracker - i].getTitle() + '\n';
    }
    mutexBBS.unlock();

    if (listed == "")
    {
        listed = "No available messages";
    }
    return listed;
}

messageBBS Get(int mid)
{
    messageBBS m;
    mutexBBS.lock();
    if (mid <= 0 || mid > id_tracker)
    {
        m = messageBoard[0]; // not valid id.
    }
    else
    {
        if (messageBoard[mid].getId() == 0)
        {
            m = messageBoard[0]; // not valid message.
        }
        else
        {
            m = messageBoard[mid];
        }
    }
    mutexBBS.unlock();

    return m;
}

void insertNewMessage(messageBBS m, int mode = 0)
{
    // This function should insert the message in the perfect spot.
    if (mode == 0)
    {
        messageBoard.push_back(m); // Insert the new message in the back.
    }
}

void Add(string title, string author, string body)
{
    messageBBS m(0, author, title, body);
    mutexBBS.lock();
    id_tracker++;       // Autoincremental id.
    numberOfMessages++; // We got a new message in the board.
    m.setId(id_tracker);
    insertNewMessage(m);
    mutexBBS.unlock();
}


void Remove(uint32_t id)
{
    // To do.
    mutexBBS.lock();
    numberOfMessages--;           // We got a new message in the board.
    messageBoard.at(id).setId(0); // Invalidate the message
    mutexBBS.unlock();
}

void periodicCheckOfTheBoard()
{
    // This function should check for delete non valid messages.
}

void periodicCheckOfTheConnectionsList()
{
    // This function should delete the too old connection object of the users.
}

bool checkUserList(string inputNickname, userBBS &us)
{
    bool res = false;
    mutexUserList.lock();
    const unsigned int size = userList.size();
    for (unsigned int i = 0; i < size; i++)
    {
        // cout << userList.at(i).getNickname() << " " << inputNickname << endl;
        if (userList[i].getNickname() == inputNickname)
        {
            us = userList[i];
            res = true;
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

void refreshConnectionInformation(string inputNickname, int sd, string type)
{
    const unsigned int size = connections.size();
    for (unsigned int i = 0; i < size; i++)
    {
        if (connections[i].getNickname() == inputNickname)
        {
            if (type == "login")
            {
                connections[i].refreshLogin(sd);
                break;
            }
            else if (type == "logout")
            {
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

void registrationThread(int socketDescriptor, bool &result, string &status, string &nickName)
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
        status = "ERROR - Some empty fields!";
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
            userBBS userRecv(nickNameRecv, emailRecv, "");
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
            sendIntegerNumber(socketDescriptor, 1);
            nickName = nickNameRecv; // save the nickname for later.
            return;
        }
    }
    result = false;
    status = "Something went wrong!";
    sendIntegerNumber(socketDescriptor, 0);
}

void loginThread(int socketDescriptor, bool &result, string &status, string &nickName)
{
    string nickNameRecv, pwdRecv; // Receiving variables.
    userBBS usr;
    bool res = false;

    nickNameRecv = receiveString(socketDescriptor);         // Receive the nickname from the client.
    pwdRecv = computeHash(receiveString(socketDescriptor)); // Receive the clear password from the client; and immediately compute the hash.

    if (nickNameRecv.length() == 0 || pwdRecv.length() == 0)
    {
        result = false;
        status = "ERROR - Some empty fields!";
        return;
    }

    res = checkUserList(nickNameRecv, usr);

    if (res)
    {
        // Check if the nickname actually exists
        if (usr.getPasswordDigest() == pwdRecv)
        {
            // if the password of the choosen nickname is correct.
            mutexConnections.lock();
            refreshConnectionInformation(nickNameRecv, socketDescriptor, "login");
            updateConnectionFile();
            mutexConnections.unlock();

            result = true;
            status = "Login ok!";
            sendIntegerNumber(socketDescriptor, 1);

            nickName = nickNameRecv;
            return;
        }
        else
        {
            result = false;
            status = "Wrong password!";
            sendIntegerNumber(socketDescriptor, -1);
            return;
        }
    }
    result = false;
    status = "Something went wrong!";
    sendIntegerNumber(socketDescriptor, 0);
}

void BBSsession(int socketDescriptor, string nickNameRecv)
{
    int requestType = 0;
    while (true)
    {
        requestType = receiveIntegerNumber(socketDescriptor); // Get the request type from the client.

        if (requestType == LIST_REQUEST_TYPE)
        {
            sendString(socketDescriptor, List(receiveIntegerNumber(socketDescriptor)));
        }
        else if (requestType == GET_REQUEST_TYPE)
        {
            sendString(socketDescriptor, Get(receiveIntegerNumber(socketDescriptor)).toListed());
        }
        else if (requestType == ADD_REQUEST_TYPE)
        {
            vector<string> requestParts = divideString(receiveString(socketDescriptor), '-');
            cout << "nickname: " << nickNameRecv << endl;
            Add(requestParts[0], nickNameRecv, requestParts[1]);
            sendIntegerNumber(socketDescriptor, 1);
        }
        else if (requestType == LOGOUT_REQUEST_TYPE)
        {
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

    Add("Esempio1", "Esempio1", "Esempio1");
    Add("Esempio2", "Esempio2", "Esempio2");
    Add("Esempio3", "Esempio3", "Esempio3");
    Add("Esempio4", "Esempio4", "Esempio4");
    Add("Esempio5", "Esempio5", "Esempio5");
    Add("Esempio6", "Esempio6", "Esempio6");

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

    fd_max = request_socket; // L'ultimo inserito è request_socket.
    int requestType = 0;

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
                        string nickNameSession = ""; // Temporary variable that keeps the nickname of the user that have just logged in.
                        bool howItEnded = false;     // It becomes true if the procedure goes fine.
                        string status = "";          // Explain the result of the procedure.
                        registrationThread(new_sd, howItEnded, status, nickNameSession);
                        if (howItEnded)
                        {
                            // If the registration went fine...
                            BBSsession(new_sd, nickNameSession); // The actual session.
                        }
                        close(new_sd);
                        continue;
                    }
                    if (requestType == LOGIN_REQUEST_TYPE) // Registration.
                    {
                        string nickNameSession = ""; // Temporary variable that keeps the nickname of the user that have just logged in.
                        bool howItEnded = false;     // It becomes true if the procedure goes fine.
                        string status = "";          // Explain the result of the procedure.
                        loginThread(new_sd, howItEnded, status, nickNameSession);
                        if (howItEnded)
                        {
                            // If the login procedure went fine...
                            BBSsession(new_sd, nickNameSession); // The actual session.
                        }
                        close(new_sd);
                        continue;
                    }
                }
                FD_CLR(i, &master); // Not valid socket.
                close(i);           // Close the not valid socket.
            }
        }
    }
}
