
#include "communicationLibrary.h"
#include "configuration.h"
#include "userBBS.h"
#include <mutex>
#include <vector>
#include <thread>
#include <iostream>
#include <chrono>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include "messagePackingLibrary.h"
using namespace std;

void Client_RSAE(int sd, uint64_t R, string K)
{
    RSAEMessage messageRSAE;

    string recvReq = receiveString(sd);

    messageRSAE.deconcatenateAndAssign(recvReq);

    if (messageRSAE.verifyDigitalFirm(R))
    {
        // Digital signature is ok
        sendString(sd, rsa_encrypt(K, convertStringToPublicEVP_PKEY(messageRSAE.getPublicKey()))); // Send the crypted key.
    }
    {
        // Not ok, abort connection
    }
}

void Check_and_print_the_received_ContentMessage(string recvString, string K, string &content, bool print = false)
{
    ContentMessage msg;
    if (verifyContentMessageHMAC(recvString, msg)) // Verify the HMAC of the received message.
    {
        // Valid HMAC.
        content = ContentMessageGetContent(msg, K); // Extract the content.
        if (print)
        {
            cout << content << endl; // Print the list result sent by the server.
        }
    }
    else
    {
        // Not valid HMAC.
    }
}

void Check_and_print_the_received_ContentMessage(string recvString, string K, bool print = false)
{
    string content;
    Check_and_print_the_received_ContentMessage(recvString, K, content, print);
}

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

    if (connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Errore nella Connessione\n");
        exit(1);
    }

    cout << "************ BBS *************" << endl;

    string requestString = "";
    string p_mail = "";
    string p_nick = "";
    string p_pwd = "";
    int res = 0;
    bool valid = false;
    string status = "";

    uint64_t counter = 0;

    do
    {
        cout << "Insert 'reg' for register or insert 'log' for login" << endl;
        // we use the getline to get also the white spaces
        getline(cin, requestString);
    } while (requestString != "reg" && requestString != "log");

    SimpleMessage helloMsg;
    const uint64_t R = generate_secure_random_64_unsigned_int();
    const string K = generateRandomKey(64);
    // sendString(sd , "ciao" , K);

    if (requestString == "reg")
    {
        string status;
        string req = "reg-" + to_string(R);
        req = packSimpleMessage(req);
        sendString(sd, req);   // SIMPLE MESSAGE - Send the registration request.
        Client_RSAE(sd, R, K); // RSAE Procedure.

        do
        {
            cout << "Insert email" << endl;
            cin >> p_mail;
            cout << "Insert nickname" << endl;
            cin >> p_nick;
            cout << "Insert password" << endl;
            cin >> p_pwd;

            // Create the new user structure.
            userBBS ut;
            ut.setNickname(p_nick);
            ut.setSalt("SALE");
            ut.setEmail(p_mail);
            ut.setPasswordDigest(p_pwd);
            ut.setCounter(0);
            ut.concatenateFields(req);

            req = packContentMessage(req, K);
        
            sendString(sd, req, K); // CONTENT MESSAGE - Send the credentials to the server.

            req = receiveString(sd, K); // CONTENT MESSAGE - Receive the result of the wanted operation.
            ContentMessage msg;
            if (verifyContentMessageHMAC(req, msg)) // Verify the HMAC of the received message.
            {
                // Valid HMAC.
                req = ContentMessageGetContent(msg, K); // Extract the content.
                if (req != "ok")
                {
                    cout << req << endl; // Print the list result sent by the server.
                    valid = false;
                }
                else
                {
                    valid = true;
                }
            }
            else
            {
                // Not valid HMAC.
                throw std::runtime_error("Error: HMAC doesn't correspond, the message could have been alterated");
            }

        } while (!valid);

        // --------------- CHALLENGE --------------------------------------------
        req = receiveString(sd, K); // CONTENT MESSAGE - Receive the challenge.
        ContentMessage msg;
        if (verifyContentMessageHMAC(req , msg))
        {
            cout << "Received challenge: " << ContentMessageGetContent(msg , K) << endl; // Print the challenge.
            cout << "Send the same value to the server" << endl;
            cin >> req;

            sendString(sd, packContentMessage(req, K), K); // CONTENT MESSAGE - Send the response to the challenge.
        }
        {
            // The client muore.
        }
        // --------------- FINE CHALLENGE --------------------------------------------
    }
    else if (requestString == "log")
    {
        bool valid = false;
        string status;

        string req = "log-" + to_string(R);
        sendString(sd, packSimpleMessage(req)); // SIMPLE MESSAGE - Send the login request.

        Client_RSAE(sd, R, K);

        do
        {
            // Insert credentials
            cout << "Insert nickname" << endl;
            cin >> p_nick;
            cout << "Insert password" << endl;
            cin >> p_pwd;

            string recvString = p_nick + "-" + p_pwd;

            sendString(sd, packContentMessage(recvString, K), K); // CONTENT MESSAGE - Sending credentials.

            recvString = receiveString(sd, K); // CONTENT MESSAGE - Receive the result of the login operation.
     
            ContentMessage msg;
            if (verifyContentMessageHMAC(recvString, msg)) // Verify the HMAC of the received message.
            {
                // Valid HMAC.
                recvString = ContentMessageGetContent(msg, K); // Extract the content.
              
                vector<string> requestParts = divideString(recvString, '-');

                if (requestParts[0] != "ok")
                {
                    counter = 0;
                    cout << recvString << endl; // Print the list result sent by the server.
                    valid = false;
                }
                else
                {
                    counter = stoull(requestParts[1]);
                    valid = true;
                }
            }
            else
            {
                // Not valid HMAC.
                valid = false;
            }

        } while (!valid);
    }

    res = receiveIntegerNumber(sd); // INTEGER MESSAGE - Receive the result of the login/registration process.

    if (res == 1)
    {

        if ((requestString == "log") || (requestString == "Log") || (requestString == "LOG"))
        {
            cout << "\nWelcome back!\n"
                 << endl; // The user already register, so he came back.
        }
        else
        {
            cout << "\nWelcome!\n"
                 << endl; // The user have just register himself.
        }

        vector<string> requestParts;

        while (true)
        {
            // BBS Session
            cout << "----------------------------------\nAvailable commands:\n1)list-n\n2)get-n\n3)add\n4)logout\n\nDigit the wanted operation...\n----------------------------------" << endl;
            do
            {
                getline(cin, requestString); // Receive the type of operation wanted, even white spaces are inserted in the string.
            } while (requestString.length() <= 2);

            requestParts = divideString(requestString, '-'); // Divide the string on the "-".

            if ((requestParts[0] == "logout") || (requestParts[0] == "Logout") || (requestParts[0] == "LOGOUT"))
            {
                if (requestString.length() >= 7)
                {
                    cout << "ERROR - Command not valid" << endl;
                    continue;
                }
                else
                {
                    counter++;
                    string recvString = "logout-" + to_string(counter);
                    recvString = packContentMessage(recvString, K);
                    sendString(sd, recvString, K);
                    cout << "Bye bye!\n----------------------------------" << endl;
                    return 0;
                }
            }
            else if ((requestParts[0] == "list") || (requestParts[0] == "List") || (requestParts[0] == "LIST"))
            {
                // If the command doesn't have the parameter, is not valid
                if (requestParts.size() < 2)
                {
                    cout << "ERROR - Not valid command" << endl;
                    continue;
                }

                // if the parameter is not a number, it is not valid, it is not valid even if the number starts with 0
                if ((!all_of(requestParts[1].begin(), requestParts[1].end(), ::isdigit)) || requestParts[1].at(0) == '0')
                {
                    cout << "ERROR - Not valid command" << endl;
                    continue;
                }

                unsigned int howMany = stoi(requestParts[1]);

                if (howMany <= 0)
                {
                    cout << "ERROR - Not valid parameter, must be a positive integer!" << endl;
                    continue; // Not valid parameter.
                }

                string recvString = to_string(LIST_REQUEST_TYPE) + "-" + to_string(howMany) + "-" + to_string(counter + 1);
                recvString = packContentMessage(recvString, K);
                sendString(sd, recvString, K); // CONTENT MESSAGE - Send the wanted operation.

                recvString = receiveString(sd, K); // CONTENT MESSAGE - Receive the result of the wanted operation.
                Check_and_print_the_received_ContentMessage(recvString, K, true);

                counter++; // se ok, incrementa
            }
            else if ((requestParts[0] == "get") || (requestParts[0] == "Get") || (requestParts[0] == "GET"))
            {
                // If the command doesn't have the parameter, is not valid
                if (requestParts.size() < 2)
                {
                    cout << "ERROR - Not valid command" << endl;
                    continue;
                }

                // if the parameter is not a number, it is not valid, it is not valid even if the number starts with 0
                if ((!all_of(requestParts[1].begin(), requestParts[1].end(), ::isdigit)) || requestParts[1].at(0) == '0')
                {
                    cout << "ERROR - Not valid command" << endl;
                    continue;
                }

                unsigned int targetId = stoi(requestParts[1]);

                if (targetId <= 0)
                {
                    cout << "ERROR - Not valid identificator, must be an existent id!" << endl;
                    continue; // Not valid parameter.
                }

                string recvString = to_string(GET_REQUEST_TYPE) + "-" + to_string(targetId) + "-" + to_string(counter + 1);
                recvString = packContentMessage(recvString, K);
                sendString(sd, recvString, K); // CONTENT MESSAGE - Send the wanted operation.

                recvString = receiveString(sd, K); // CONTENT MESSAGE - Receive the result of the wanted operation.
                Check_and_print_the_received_ContentMessage(recvString, K, true);

                counter++; // se ok, incrementa
            }
            else if ((requestParts[0] == "add") || (requestParts[0] == "Add") || (requestParts[0] == "ADD"))
            {

                if (requestString.length() >= 4)
                {
                    cout << "ERROR - Command not valid" << endl;
                    continue;
                }
                else
                {
                    string title, body;
                    cout << "Insert the title of the post: ";
                    do
                    {
                        title = insertLineFromKeyboard();
                    } while (title.length() == 0);

                    cout << "Insert the body of the post: ";
                    do
                    {
                        body = insertLineFromKeyboard();
                    } while (body.length() == 0);

                    string recvString = to_string(ADD_REQUEST_TYPE) + "-" + (title + '|' + body) + "-" + to_string(counter + 1);
                    recvString = packContentMessage(recvString, K);
                    sendString(sd, recvString, K); // CONTENT MESSAGE - Send the wanted operation.

                    if (receiveIntegerNumber(sd) == 1) // INTEGER NUMBER - Receive the result of the ADD operation.
                    {
                        cout << "Ok the post has been inserted!" << endl;
                        counter++;
                    }
                    else
                    {
                        cout << "Something went wrong!" << endl;
                    }
                }
            }
            else
            {
                cout << "ERROR - Command not valid" << endl;
            }
        }
    }
    else if (res == -1)
    {
        cout << "\nThe challenge sent is wrong!\n"
             << endl;
    }
    else
    {

        cout << "\nSomething went wrong!\n"
             << endl;
    }

    return 0;
}
