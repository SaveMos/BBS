#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "userBBS.h"
#include "messageBBS.h"

using namespace std;

#ifndef UTILITYFILE_H
#define UTILITYFILE_H

//deconcatenate fields of a message or user 
void deconcatenateFields(vector<string> &ret, string &input){

    char delimiter = '-';
        
    size_t pos = input.find(delimiter);   // Find the position of the first delimiter character

    while (pos != std::string::npos){
            
        std::string parte = input.substr(0, pos); // get the first substring before the delimiter 
        ret.push_back(parte); // add the substring to the vector
        input = input.substr(pos + 1); // remove the extracted substring from the original string
        pos = input.find(delimiter);  // find the new position of the next delimiter character
    }
        ret.push_back(input); // add the last substring to the vector
}

//clear the entire content of the file
void clearFileContent(const string& filename) {
    ofstream userFile(filename, ios::trunc);  //open the file in trunc mode
    if (userFile.is_open()) {
        userFile.close();                     //close the empty file
    } else {
        cout << "Error during the file opening" << endl;
    }
}

//insert all the users inside the file of users
void insertUserInFile(vector<userBBS> userList){
   
    string filename("fileStorage/userFile.txt");
    //delete the content of the entire file
    clearFileContent(filename);          

    fstream userFile;
    userFile.open(filename, std::fstream::app);         //open the file in append
    
    if (userFile.is_open()) {
        //ad a row in the file for each user
        for(int i = 0; i < userList.size(); i++){
            userFile << userList.at(i).getNickname() + "-" +  userList.at(i).getPasswordDigest() + "-" + userList.at(i).getEmail() << endl;
        }
       
    } else {
        cout << "Error during the file opening" << endl;
    }

    userFile.close();       //close file
}

//insert all the users from the file into the vector
void insertUserInVector(vector<userBBS>& userList){
   
    ifstream filename("fileStorage/userFile.txt");
    
   if (filename.is_open()) {

        string line;
        vector<string> ret;
       
       //for each line of the file extract all the attributes of a user and insert it into the vector
        while (getline(filename, line)) {

            deconcatenateFields(ret, line);
            //assign each attribute to the user
            userBBS newUser;
            newUser.setNickname(ret.at(0));
            newUser.setPasswordDigest(ret.at(1));
            newUser.setEmail(ret.at(2));
            //insert inside the vector
            userList.push_back(newUser);

            ret.clear();
        }

    } else {
        cout << "Error during the file opening" << endl;
    }

    filename.close();    //close file   
}

//insert all the messages inside the file of messages
void insertMessageInFile(vector<messageBBS> messageList){
   
    string filename("fileStorage/messageFile.txt");
     //delete the content of the entire file
    clearFileContent(filename);          

    fstream messageFile;
    messageFile.open(filename, std::fstream::app);          //open the file in append
    
    if (messageFile.is_open()) {
         //ad a row in the file for each message
        for(int i=0; i<messageList.size(); i++){
            messageFile << to_string(messageList.at(i).getId()) + "-" +  messageList.at(i).getAuthor() + "-" + messageList.at(i).getTitle() + "-" + messageList.at(i).getBody()<< endl;
        }
       
    } else {
        cout << "Error during the file opening" << endl;
    }

    messageFile.close();
}

//insert all the messages from the file into the vector
void insertMessageInVector(vector<messageBBS>& messageList){
   
    ifstream filename("fileStorage/messageFile.txt");
    
   if (filename.is_open()) {

        string line;
        vector<string> ret;
       
        //for each line of the file extract all the attributes of a message and insert it into the vector
        while (getline(filename, line)) {

             deconcatenateFields(ret, line);
            
            //convert a string into a u_int32_t for the id field
            //uint32_t id;
            //stringstream ss(ret.at(0));     // create a stringstream with the string
            //ss >> id;   // convert the value

            //assign each attribute to the user
            messageBBS newMessage;
            newMessage.setId(stoi(ret.at(0)));          //convert a string into a u_int32_t for the id field
            newMessage.setAuthor(ret.at(1));
            newMessage.setTitle(ret.at(2));
            newMessage.setBody(ret.at(3));
            
            //insert inside the vector
            messageList.push_back(newMessage);

            ret.clear();
        }

    } else {
        cout << "Error during the file opening" << endl;
    }

    filename.close();   //close file    
}


std::string readMessagesFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    
    file.close();
    return buffer.str();
}

#endif
