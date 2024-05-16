#include <iostream>
#include <string>
#include "utilityFile.h"
#include "security.h"
#include "timestampLibrary.h"

using namespace std;

int main(int argc, char *argv[])
{
    /*
    messageBBS m;
    uint32_t a = 32;
    m.setId(a);
    m.setTitle("Aoooo");
    m.setAuthor("ssdsdsds");
    m.setBody("shsdhsdh");

    string s;
    m.concatenateFields(s);
    cout <<s<< endl;   

    //test insertUserInFile
    vector<userBBS> userList;

    //cration of three different users
    userBBS user1("Alice", "password1", "alice@example.com");
    userBBS user2("Bob", "password2", "bob@example.com");
    userBBS user3("Charlie", "password3", "charlie@example.com");

    //insertion of the users
    userList.push_back(user1);
    userList.push_back(user2);
    userList.push_back(user3);

    insertUserInFile(userList);

    //test insertUserInVector
    vector<userBBS> userList1;

    insertUserInVector(userList1);

    for(int i = 0; i < userList1.size(); i++){
        cout<<userList1.at(i).getNickname()+" "+userList1.at(i).getPasswordDigest()+" "+userList1.at(i).getEmail()<<endl;
    }

    //test insertMessageInFile
    vector<messageBBS> messageList;

    //cration of three different messages
    messageBBS message1(192, "Alice", "Message 1", "This is the body of message 1");
    messageBBS message2(287, "Bob", "Message 2", "This is the body of message 2");
    messageBBS message3(337, "Charlie", "Message 3", "This is the body of message 3");

    //insertion of the messages
    messageList.push_back(message1);
    messageList.push_back(message2);
    messageList.push_back(message3);

    insertMessageInFile(messageList);

    //test insertMessageInVector
    vector<messageBBS> messageList1;

    insertMessageInVector(messageList1);

    for(int i = 0; i < messageList1.size(); i++){
        cout<<to_string(messageList1.at(i).getId())+" "+messageList1.at(i).getAuthor()+" "+messageList1.at(i).getTitle()+" "+messageList1.at(i).getBody()<<endl;
    }

    //test checkEmailFormat

    //test hash
    string input = "Hello, world!";
    string hash = computeHash(input);
    cout << "Hash SHA-256 di '" << input << "': " << hash << std::endl;
    */
   
    cout << "Timestamp corrente: " << getCurrentTimestamp() << endl;
    cout << secondDifference("2024-05-16 09:53:10.222" , "2024-05-16 09:53:15.222") << endl;

}