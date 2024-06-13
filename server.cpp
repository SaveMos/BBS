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
#define SNAPSHOT_PERIOD 30

using namespace std;
// Variabili dei Threads
mutex mutexBBS;
mutex mutexUserList;
mutex mutexOpenSSL; // Required because OpenSSL is not thread-safe.
mutex mutexConnections;

mutex mutexFileStorage;

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
    // numberOfMessages--;           // We got a new message in the board.
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
    const uint64_t size = connections.size();
    for (uint64_t i = 0; i < size; i++)
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

void loadSnapshot()
{
    mutexFileStorage.lock();
    insertUserInVector(userList);
    insertMessageInVector(messageBoard);
    insertConnectionInformationInVector(connections);
    mutexFileStorage.unlock();
}

void doSnapshot()
{
    // this function updates all the files in the fileStorage.
    mutexFileStorage.lock();
    insertUserInFile(userList);
    insertMessageInFile(messageBoard);
    insertConnectionInformationInFile(connections);
    mutexFileStorage.unlock();
}

void periodicSnaphot(){
    while(true){
        this_thread::sleep_for(std::chrono::seconds(SNAPSHOT_PERIOD));
        doSnapshot();
    }
}

void registerationProcedure(int socketDescriptor, bool &result, string &status, string &nickName)
{
    const string emailRecv = receiveString(socketDescriptor); // Receive the email from the client.
    cout << emailRecv << endl;
    const string nickNameRecv = receiveString(socketDescriptor); // Receive the nickname from the client.
    cout << nickNameRecv << endl;
    const string pwdRecv = computeHash(receiveString(socketDescriptor)); // Receive the clear password from the client; and immediately compute the hash.
    cout << pwdRecv << endl;

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
            mutexUserList.unlock();

            connectionInformation c(socketDescriptor, nickNameRecv, getCurrentTimestamp(), getCurrentTimestamp(), true);

            mutexConnections.lock();
            connections.push_back(c);
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

void loginProcedure(int socketDescriptor, bool &result, string &status, string &nickName)
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
            mutexConnections.unlock();
            break;
        }
    }

    doSnapshot();
}

void threadServerCode(int new_sd)
{
    string nickNameSession = "";                          // Temporary variable that keeps the nickname of the user that have just logged in.
    bool howItEnded = false;                              // It becomes true if the procedure goes fine.
    string status = "";                                   // Explain the result of the procedure.
    const int requestType = receiveIntegerNumber(new_sd); // Get the request type from the client.

    if (requestType == REGISTRATION_REQUEST_TYPE) // Registration.
    {
        registerationProcedure(new_sd, howItEnded, status, nickNameSession);
        if (howItEnded)
        {
            // If the registration went fine...
            BBSsession(new_sd, nickNameSession); // The actual session.
        }
        close(new_sd);
    }
    else if (requestType == LOGIN_REQUEST_TYPE) // Registration.
    {

        loginProcedure(new_sd, howItEnded, status, nickNameSession);
        if (howItEnded)
        {
            // If the login procedure went fine...
            BBSsession(new_sd, nickNameSession); // The actual session.
        }
        close(new_sd);
    }
}


int main()
{
    int serverSocket, clientSocket, maxFd, activity;
    sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    fd_set readFds;
    std::vector<std::thread> threads;
    std::mutex clientSocketsMutex; // Mutex for synchronizing access to clientSockets vector.

    loadSnapshot();

    thread snapshotter(periodicSnaphot);
    snapshotter.detach();

    // Creazione del socket del server
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "socket creation failed: " << strerror(errno) << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "bind failed: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1)
    {
        std::cerr << "listen failed: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is listening on port 8080..." << std::endl;

    while (true)
    {
        // Clear and set the file descriptor set
        FD_ZERO(&readFds);
        FD_SET(serverSocket, &readFds);
        maxFd = serverSocket;

        // Wait for activity on the server socket
        activity = select(maxFd + 1, &readFds, nullptr, nullptr, nullptr);
        if (activity < 0 && errno != EINTR)
        {
            std::cerr << "select error: " << strerror(errno) << std::endl;
            close(serverSocket);
            return 1;
        }

        // If something happened on the server socket, it means a new connection
        if (FD_ISSET(serverSocket, &readFds))
        {
            clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
            if (clientSocket == -1)
            {
                std::cerr << "accept failed: " << strerror(errno) << std::endl;
                continue;
            }
            std::cout << "New client connected." << std::endl;

            // Crea un nuovo thread per gestire il client
            std::lock_guard<std::mutex> lock(clientSocketsMutex);
            threads.emplace_back(threadServerCode, clientSocket);
        }
    }

    // Chiudi il server socket
    close(serverSocket);

    for (auto &thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    return 0;
}
