
#include "communicationLibrary.h"
#include "configuration.h"
#include "userBBS.h"
#include <mutex>
#include <vector>
#include <thread>
#include <iostream>
#include <chrono>
#include <string>
#include "security.h"
#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

//load the RSA server public key from the file
EVP_PKEY* loadServerPublicKey() {
    return loadRSAKey(PUBLIC_KEY_PATH, true);
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
     //---------------------------------------------------------------------------------------------------------
    // load the public key of the server
    EVP_PKEY *serverPublicKey = loadServerPublicKey();
    //cout<<"server public key"<<serverPublicKey<<endl;
    // generates a random AES key of 32 bytes
    std::vector<unsigned char> aesKey(32);
    if (!RAND_bytes(aesKey.data(), aesKey.size())) {
        throw std::runtime_error("Error in the generation of the AES key");
    }
   
    cout <<"aeskey: "<<std::string(aesKey.begin(), aesKey.end())<<endl;
    //cout<<"aes key client "<<aesKey<<endl;
    // encrypt AES key with the RSA public key of the server
    std::string encryptedAesKey = rsa_encrypt(std::string(aesKey.begin(), aesKey.end()), serverPublicKey);
    
    //cout<<"encrypted aes key"<<encryptedAesKey<<endl;
    // send the encrypted AES key to the server
    sendString(sd, encryptedAesKey);
    //---------------------------------------------------------------------------------------------------------

    cout << "************ BBS *************" << endl;

    string requestString = "";
    string p_mail = "";
    string p_nick = "";
    string p_pwd = "";
    int res = 0;
    bool valid = false;
    string status = "";

    do{
        cout << "Insert 'reg' for register or insert 'log' for login" << endl;
        //we use the getline to get also the white spaces
        getline(cin, requestString);
    }while (requestString != "reg" && requestString != "log"); 

    if (requestString == "reg"){

        // Registration test
        sendIntegerNumber(sd, REGISTRATION_REQUEST_TYPE); // I want to registrate, so i send 0.

        do{
            cout << "Insert email" << endl;
            cin >> p_mail;
            //---------------------------------------------------------------------------------------------------------
            // encrypt the message with AES
            std::vector<unsigned char> encryptedMail = encrypt_AES(p_mail, std::string(aesKey.begin(), aesKey.end()));
             
            cout<<"enc email: "<<std::string(encryptedMail.begin(), encryptedMail.end())<<endl;
            // compute the HMAC of the encrypted message
            const EVP_MD *evp_md = EVP_sha256();
            std::string hmac = calculateHMAC(std::string(aesKey.begin(), aesKey.end()), std::string(encryptedMail.begin(), encryptedMail.end()), evp_md);
            
            cout<<"hmac calcolato dal client: "<<hmac<<endl;
            sendString(sd, std::string(encryptedMail.begin(), encryptedMail.end())); // Send the email
            sendString(sd, hmac);          //send hmac to the server
            //---------------------------------------------------------------------------------------------------------

           status = receiveString(sd);
           if(status !="ok"){

                cout<<status<<endl;

           }else{
                valid = true;
           }

        }while(!valid);

        valid = false;

        do{
            cout << "Insert nickname" << endl;
            cin >> p_nick;
            sendString(sd, p_nick); // Send the nickname.

           status = receiveString(sd);
           if(status !="ok"){

                cout<<status<<endl;

           }else{
                valid = true;
           }

        }while(!valid);
       
       valid = false;

        do{
            cout << "Insert password" << endl;
            cin >> p_pwd;
            sendString(sd, p_pwd); // Send the password.

           status = receiveString(sd);
           if(status !="ok"){

                cout<<status<<endl;

           }else{
                valid = true;
           }

        }while(!valid);
        

        // CHALLENGE
        int requestType = receiveIntegerNumber(sd);
        sendIntegerNumber(sd, requestType);

    }else if (requestString == "log"){
        // Login Test
        sendIntegerNumber(sd, LOGIN_REQUEST_TYPE); // I want to login, so i send 1.

        do{
            cout<<"Insert nickname"<<endl;
            cin>>p_nick;
            sendString(sd, p_nick); // Send the nickname.

           status = receiveString(sd);
           if(status !="ok"){

                cout<<status<<endl;

           }else{
                valid = true;
           }

        }while(!valid);

        valid = false;

        do{
            cout<<"Insert password"<<endl;
            cin>>p_pwd;
            sendString(sd, p_pwd); // Send the password.

           status = receiveString(sd);
           if(status !="ok"){

                cout<<status<<endl;

           }else{
                valid = true;
           }

        }while(!valid);
    }

    res = receiveIntegerNumber(sd); // Receive the result of the login/registration process.

    if (res == 1){

        if ((requestString == "log") || (requestString == "Log") || (requestString == "LOG")){
            
            cout << "\nWelcome back!\n"<< endl; // The user already register, so he came back.

        }else{
            
            cout << "\nWelcome!\n"<< endl; // The user have just register himself.
        }

        vector<string> requestParts;

        while (true)
        {
            cout << "----------------------------------\nAvailable commands:\n1)list-n\n2)get-n\n3)add\n4)logout\n\nDigit the wanted operation...\n----------------------------------" << endl;
            do{
                // Even white spaces are inserted in the string
                getline(cin, requestString); // Receive the type of operation wanted
            }while(requestString.length() <= 2);

            requestParts = divideString(requestString, '-'); // Divide the string on the '-'

            if ((requestParts[0] == "logout") || (requestParts[0] == "Logout") || (requestParts[0] == "LOGOUT"))
            {
                if (requestString.length() >= 7)
                {

                    cout << "ERROR - Command not valid" << endl;
                    continue;
                }
                else
                {
                    sendIntegerNumber(sd, LOGOUT_REQUEST_TYPE);
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
                sendIntegerNumber(sd, LIST_REQUEST_TYPE); // We want to see the id of the latest posts.
                sendIntegerNumber(sd, howMany);           // Send the number of wanted posts.
                cout << receiveString(sd) << endl;
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
                sendIntegerNumber(sd, GET_REQUEST_TYPE); // We want to download a post.
                sendIntegerNumber(sd, targetId);         // Send the id of the wanted post.
                /*
                string messageGetted = receiveString(sd);
                if(messageGetted == "err - err - err\n\n"){
                    cout << "The message doesn't exists"<< endl;
                }else{
                    cout << "\n"
                        << messageGetted << endl;
                }
                */
               cout << "\n"
                        << receiveString(sd) << endl;
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

                    sendIntegerNumber(sd, ADD_REQUEST_TYPE); // We want to add a new post.
                    sendString(sd, title + '-' + body);      // Send the content of the post.
                    int res = receiveIntegerNumber(sd);      // Receive the result of the operation.
                    if (res == 1)
                    {
                        cout << "Ok the post has been inserted!" << endl;
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
    else{
       
        cout << "\nSomething went wrong!\n"<< endl;
    }

    return 0;
}
