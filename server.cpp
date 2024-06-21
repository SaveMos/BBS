#include "communicationLibrary.h"
#include "configuration.h"
#include "utilityFile.h"
#include "connectionInformation.h"
#include "security.h"
#include <openssl/evp.h>
#include <mutex>
#include <vector>
#include <thread>
#include <iostream>

#define MAXIMUM_NUMBER_OF_THREAD 100
#define MAXIMUM_REQUEST_NUMBER 1000
#define SNAPSHOT_PERIOD 30

#define CONNECTION_INFO_CHECK_PERIOD 15
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


EVP_PKEY *serverPrivateKey;
std::string encryptedAesKey;
std::string aesKey;


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

void insertNewMessage(messageBBS m, uint8_t mode = 0)
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

void doConnectionListCheck(){
    mutexConnections.lock();
    const size_t size =  connections.size();
    for(size_t i = 0 ; i < size; i++){
        if(!connections[i].checkValidityOfTheConnection() && connections[i].getLogged() == true){
            // Connessione scaduta, l'utente ha chiuso il programma senza fare logout o è inattivo da troppo tempo.
            connections[i].setLogged(false);
        }
    }
    mutexConnections.unlock();
}

void periodicCheckOfTheConnectionsList()
{
    while(true){
        this_thread::sleep_for(std::chrono::seconds(CONNECTION_INFO_CHECK_PERIOD));
        doConnectionListCheck();
    }
}

bool getConnectionInfoOfUser(string inputNickname, connectionInformation& conn){
    bool found = false;
    mutexConnections.lock();
    const size_t size =  connections.size();
    for(size_t i = 0 ; i < size; i++){
        if(connections[i].getNickname() == inputNickname){
            conn = connections[i];
            found = true;
            break;
        }
    }
    mutexConnections.unlock();
    return found;
}


bool checkUserList(string inputNickname, userBBS &us)
{
    bool res = false;
    mutexUserList.lock();
    const uint64_t size = userList.size();
    for (uint64_t i = 0; i < size; i++)
    {
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
    const uint64_t size = userList.size();
    for (uint64_t i = 0; i < size; i++)
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

bool checkUserEmail(string inputEmail)
{
    bool res = false;
    mutexUserList.lock();
    const uint64_t size = userList.size();
    
    for (uint64_t i = 0; i < size; i++)
    {
        if (userList[i].getEmail() == inputEmail)
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
    const size_t size = connections.size();
    for (size_t i = 0; i < size; i++)
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
    numberOfMessages = messageBoard.size();
    id_tracker = numberOfMessages - 1;
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

uint8_t checkUserConnectionInfoAtLoginOrRegistration(string& nickname){
    connectionInformation conn;
   
    if(getConnectionInfoOfUser(nickname , conn)){
        // There exists a connection of this user...
        if(conn.getLogged()){
            // The user is already logged!
            return 1;
        }

        if(!conn.checkValidityOfTheConnection()){
            // The connection info has errors!
            return 2;

        }
    }
    return 0;
}

void registerationProcedure(int socketDescriptor, bool &result, string &status, string &nickName)
{
    string emailRecv; //= receiveString(socketDescriptor); // Receive the email from the client.
    //cout << emailRecv << endl;
    string nickNameRecv; // = receiveString(socketDescriptor); // Receive the nickname from the client.
    //cout << nickNameRecv << endl;
    string pwdRecv; // = computeHash(receiveString(socketDescriptor)); // Receive the clear password from the client; and immediately compute the hash.
    //cout << pwdRecv << endl;
    string pwdClear;
    bool valid = false;

    do{
        
        // Receive the encrypted message and HMAC from the client
        std::string encryptedMessage = receiveString(socketDescriptor);
        std::string receivedHmac = receiveString(socketDescriptor);
        //cout<<"encrypted email received from the client: "<<encryptedMessage<<endl;
        // Decript the message with AES
        std::vector<unsigned char> encryptedMessageVec(encryptedMessage.begin(), encryptedMessage.end());
        emailRecv = decrypt_AES(encryptedMessageVec, aesKey);
        
        cout<<"enc email: "<<std::string(encryptedMessage.begin(), encryptedMessage.end())<<endl;
       
        // Verify HMAC of encrypted message
        const EVP_MD *evp_md = EVP_sha256();
        std::string calculatedHmac = calculateHMAC(aesKey, std::string(encryptedMessage.begin(), encryptedMessage.end()), evp_md);

        cout<<"calculatedHmac: "<<calculatedHmac<<"\n"<<"receivedHmac: "<<receivedHmac<<endl;

        if (calculatedHmac != receivedHmac) {
            throw std::runtime_error("Error: HMAC doesn't correspond, the message could have been alterated");
        }

        //emailRecv = receiveString(socketDescriptor); // Receive the email from the client.
        cout << emailRecv << endl;


        if (!checkEmailFormat(emailRecv)){
            result = false;
            status = "ERROR - Wrong email format";    
        }else if(checkUserEmail(emailRecv)){
            result = false;
            status = "ERROR - This email already exists";
        }else{
            valid = true;
            status = "ok";
        }

        sendString(socketDescriptor, status);

    }while(!valid);

    valid = false;
    
    do{
        nickNameRecv = receiveString(socketDescriptor); // Receive the nickname from the client.
        cout << nickNameRecv << endl;

        if (nickNameRecv.length() < 3){
            result = false;
            status = "ERROR - Nickname must be at least of 3 characters";    
        }else if(checkUserList(nickNameRecv)){
            result = false;
            status = "ERROR - This nickname already exists";
        }else{
            valid = true;
            status = "ok";
        }

        sendString(socketDescriptor, status);

    }while(!valid);
   
    valid = false;

    do{
        pwdClear = receiveString(socketDescriptor);
        pwdRecv = computeHash(pwdClear); // Receive the clear password from the client; and immediately compute the hash.
        cout << pwdRecv << endl;

        if (pwdClear.length() < 5){
            result = false;
            status = "ERROR - The password must be at least of 5 characters";
            sendString(socketDescriptor, status);    
        }else{
            valid = true;
            status = "ok";
            sendString(socketDescriptor, status);

            // Check if the nickname have already been used.
            int sendChallenge = generate_secure_random_int();
            int recvChallenge; // The challenge.

            sendChallenge = abs(sendChallenge);
            sendIntegerNumber(socketDescriptor, sendChallenge);     // Send the challenge.

            recvChallenge = receiveIntegerNumber(socketDescriptor); // Receive the challenge.
           

            if (recvChallenge == sendChallenge){
                // Challenge win!
                userBBS userRecv(nickNameRecv, "", emailRecv,  generateRandomSalt(4));
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
            }else{
                cout<<"challenge not valid, client disconnected"<<endl;
                sendIntegerNumber(socketDescriptor, -1);
                close(socketDescriptor);
                return;
            }
        }

    }while(!valid);
    
    result = false;
    status = "Something went wrong!";
    sendIntegerNumber(socketDescriptor, 0);
}

void loginProcedure(int socketDescriptor, bool &result, string &status, string &nickName)
{
    string nickNameRecv, pwdRecv; // Receiving variables.
    userBBS usr;
    bool res = false;
    bool valid = false;

    do{
        nickNameRecv = receiveString(socketDescriptor);         // Receive the nickname from the client.

        if (nickNameRecv.length() == 0){
            result = false;
            status = "ERROR - Empty nickname!";
        }else if(!checkUserList(nickNameRecv, usr)){
            result = false;
            status = "ERROR - This nickname doesn't exists!";
        }else{
            valid = true;
            status = "ok";
        }

        sendString(socketDescriptor, status);

    }while(!valid);

    valid = false;

    do{
        string pwd = receiveString(socketDescriptor);
        pwdRecv = computeHash(pwd); // Receive the clear password from the client; and immediately compute the hash.

        if (pwdRecv.length() == 0){
            result = false;
            status = "ERROR - Empty password!";
            sendString(socketDescriptor, status);
        }else if(usr.getPasswordDigest() != pwdRecv){
            result = false;
            status = "ERROR - Wrong password!";
            sendString(socketDescriptor, status);
        }else{
            valid = true;
            status = "ok";
            sendString(socketDescriptor, status);
            
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

    }while(!valid);

    
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
    //---------------------------------------------------------------------------------------------------------
    // Receive the encrypted AES key from the client
    encryptedAesKey = receiveString(new_sd);
    //cout<<"encrypted aes key received from the client: "<<encryptedAesKey<<endl;
    // Decrypt AES key with the RSA private key of the server
    aesKey = rsa_decrypt(encryptedAesKey, serverPrivateKey);
    //-----------------------------------------------------------------------------------------------------------

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

//load the RSA server private key from the file
EVP_PKEY* loadServerPrivateKey() {
    return loadRSAKey(PRIVATE_KEY_PATH, false);
}

int main()
{
    int serverSocket, clientSocket, maxFd, activity;
    sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    fd_set readFds;
    std::vector<std::thread> threads;
    std::mutex clientSocketsMutex; // Mutex for synchronizing access to clientSockets vector.

    //load each file in the respective vector
    loadSnapshot();

    /*
    for(int i = 0; i < messageBoard.size(); i++){
        cout<<messageBoard[i].getId()<<" - "<<messageBoard[i].getAuthor()<<" - "<<messageBoard[i].getTitle()<<" - "<<messageBoard[i].getBody()<<endl;
    }

     for(int i = 0; i < userList.size(); i++){
        cout<<userList[i].getNickname()<<" - "<<userList[i].getPasswordDigest()<<" - "<<userList[i].getEmail()<<endl;
    }
    */
    thread snapshotter(periodicSnaphot);
    snapshotter.detach();

    thread connectionListChecker(periodicCheckOfTheConnectionsList);
    connectionListChecker.detach();

    // Creazione del socket del server
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "socket creation failed: " << strerror(errno) << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

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

    //---------------------------------------------------------------------------------------------------------
    // load the private key of the server
    serverPrivateKey = loadServerPrivateKey();
    //cout<<"server privare key: "<<serverPrivateKey<<endl;
    //---------------------------------------------------------------------------------------------------------

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
