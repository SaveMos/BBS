#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "userBBS.h"
#include "messageBBS.h"
#include "connectionInformation.h"

using namespace std;

#ifndef UTILITYFILE_H
#define UTILITYFILE_H

// deconcatenate fields of a message or user
void deconcatenateFields(vector<string> &ret, string &input)
{
    char delimiter = '-';
    size_t pos = input.find(delimiter); // Find the position of the first delimiter character

    while (pos != std::string::npos)
    {

        std::string parte = input.substr(0, pos); // get the first substring before the delimiter
        ret.push_back(parte);                     // add the substring to the vector
        input = input.substr(pos + 1);            // remove the extracted substring from the original string
        pos = input.find(delimiter);              // find the new position of the next delimiter character
    }
    ret.push_back(input); // add the last substring to the vector
}

// clear the entire content of the file
void clearFileContent(const string &filename)
{
    ofstream userFile(filename, ios::trunc); // open the file in trunc mode
    if (userFile.is_open())
    {
        userFile.close(); // close the empty file
    }
    else
    {
        cout << "Error during the file opening" << endl;
    }
}

// insert all the users inside the file of users
void insertUserInFile(vector<userBBS> userList){
    string filenameGeneric("fileStorage/userFile.txt");
    string filenamePasswords("fileStorage/userPasswords.txt");
    // Delete the content of the entire file
    clearFileContent(filenameGeneric);
    clearFileContent(filenamePasswords);

    fstream userFile;
    userFile.open(filenameGeneric, std::fstream::app); // Open the file in append.
    if (userFile.is_open())
    {
        // ad a row in the file for each user
        for (uint64_t i = 0; i < userList.size(); i++)
        {
            userFile << userList.at(i).getNickname() + "-" +
                            userList.at(i).getSalt() + "-" +
                            userList.at(i).getEmail() + "-" +
                            userList.at(i).getCounter()<< endl;
        }
    }
    else
    {
        cout << "Error during the file opening" << endl;
    }
    userFile.close(); // Close the file.

    userFile.open(filenamePasswords, std::fstream::app); // Open the file in append.
    if (userFile.is_open())
    {
        // ad a row in the file for each user
        for (uint64_t i = 0; i < userList.size(); i++)
        {
            userFile << userList.at(i).getNickname() + "-" + userList.at(i).getPasswordDigest() << endl;
        }
    }
    else
    {
        cout << "Error during the file opening" << endl;
    }
    userFile.close(); // Close the file.
}


uint64_t checkUserListIndex(vector<userBBS> &userList , string inputNickname)
{
    uint64_t index = 0;
    const size_t size = userList.size();
    for (size_t i = 0; i < size; i++)
    {
        if (userList[i].getNickname() == inputNickname)
        {
            index = i;
            break;
        }
    }
    return index;
}


// insert all the users from the file into the vector
void insertUserInVector(vector<userBBS> &userList){
    userList.clear();
    ifstream filenameGeneric("fileStorage/userFile.txt");
    if (filenameGeneric.is_open())
    {
        string line;
        vector<string> ret;

        // for each line of the file extract all the attributes of a user and insert it into the vector
        while (getline(filenameGeneric, line))
        {
            deconcatenateFields(ret, line);
            // Assign each attribute to the user
            userBBS newUser;
            newUser.setNickname(ret.at(0));
            newUser.setSalt(ret.at(1));
            newUser.setEmail(ret.at(2));
            newUser.setCounter(ret.at(3));

            userList.push_back(newUser); // tail-insert the user inside the user list.
            ret.clear();
        }
    }
    else
    {
        cout << "Error during the file opening" << endl;
    }
    filenameGeneric.close(); // Close the file

    ifstream filenamePasswords("fileStorage/userPasswords.txt");
    if (filenamePasswords.is_open())
    {
        string line;
        vector<string> ret;
        uint64_t index;

        // for each line of the file extract all the attributes of a user and insert it into the vector
        while (getline(filenamePasswords, line))
        {
            deconcatenateFields(ret, line);
            index = checkUserListIndex(userList, ret.at(0));  
            userList[index].setPasswordDigest(ret.at(1));
            ret.clear();
        }
    }
    else
    {
        cout << "Error during the file opening" << endl;
    }
    filenamePasswords.close(); // Close the file
}

// insert all the messages inside the file of messages
void insertMessageInFile(vector<messageBBS> messageList)
{

    string filename("fileStorage/messageFile.txt");
    // delete the content of the entire file
    clearFileContent(filename);

    fstream messageFile;
    messageFile.open(filename, std::fstream::app); // open the file in append

    if (messageFile.is_open())
    {
        // ad a row in the file for each message
        for (uint64_t i = 0; i < messageList.size(); i++)
        {
            messageFile << to_string(messageList.at(i).getId()) + "-" + messageList.at(i).getAuthor() + "-" + messageList.at(i).getTitle() + "-" + messageList.at(i).getBody() << endl;
        }
    }
    else
    {
        cout << "Error during the file opening" << endl;
    }

    messageFile.close();
}

// insert all the messages from the file into the vector
void insertMessageInVector(vector<messageBBS> &messageList)
{
    ifstream filename("fileStorage/messageFile.txt");
    if (filename.is_open())
    {

        string line;
        vector<string> ret;

        // for each line of the file extract all the attributes of a message and insert it into the vector
        while (getline(filename, line))
        {
            // Here we must decrypt the line.
            deconcatenateFields(ret, line);
            // assign each attribute to the user
            messageBBS newMessage;
            newMessage.setId(stoi(ret.at(0))); // convert a string into a u_int32_t for the id field
            newMessage.setAuthor(ret.at(1));
            newMessage.setTitle(ret.at(2));
            newMessage.setBody(ret.at(3));

            // insert inside the vector
            messageList.push_back(newMessage);

            ret.clear();
        }
    }
    else
    {
        cout << "Error during the file opening" << endl;
    }

    filename.close(); // close file
}

// insert all the messages inside the file of messages
void insertConnectionInformationInFile(vector<connectionInformation> connList)
{
    string filename("fileStorage/connectionInfo.txt");
    clearFileContent(filename); // delete the content of the entire file

    fstream connFile;
    connFile.open(filename, std::fstream::app); // open the file in append

    if (connFile.is_open())
    {
        // ad a row in the file for each message
        for (uint64_t i = 0; i < connList.size(); i++)
        {
            connFile << to_string(connList.at(i).getSocketDescriptor()) + "-" +
                            connList.at(i).getNickname() + "-" +
                            connList.at(i).getLoginTimestamp() + "-" +
                            connList.at(i).getLastActivityTimeStamp() + "-" +
                            to_string(connList.at(i).getLogged())
                     << endl;
        }
    }
    else
    {
        cout << "Error during the file opening" << endl;
    }

    connFile.close();
}

// insert all the messages from the file into the vector
void insertConnectionInformationInVector(vector<connectionInformation> &connList)
{
    ifstream filename("fileStorage/connectionInfo.txt");

    if (filename.is_open())
    {
        string line;
        vector<string> ret;

        // for each line of the file extract all the attributes of a message and insert it into the vector
        while (getline(filename, line))
        {

            // Here we must decrypt the line.

            deconcatenateFields(ret, line);

            // assign each attribute to the user
            connectionInformation conn;
            conn.setSocketDescriptor(stoi(ret.at(0))); // convert a string into a u_int32_t for the id field
            conn.setNickname(ret.at(1));
            conn.setLoginTimestamp(ret.at(2));
            conn.setLastActivityTimeStamp(ret.at(3));
            conn.setLogged(stoi(ret.at(4)));

            connList.push_back(conn); // insert inside the vector.

            ret.clear();
        }
    }
    else
    {
        cout << "Error during the file opening" << endl;
    }

    filename.close(); // close file
}

#endif