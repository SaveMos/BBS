#include <string>
#include <fstream>
using namespace std;

bool checkEmailFormat(string emailRecv){
    return true;
}

void insertUserInFile(string userString){
    
    string filename("tmp.txt");
    fstream userFile;
    userFile.open(filename, std::fstream::app);
    
    if (userFile.is_open()) {
        userFile << userString << endl;
    } else {
        cout << "Error during the file opening" << endl;
    }
}