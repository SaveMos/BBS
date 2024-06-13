#include <string>
#include <vector>
#include <iostream>
#include <limits>
#include <regex>

using namespace std;

#ifndef UTILITY_H
#define UTILITY_H

bool checkEmailFormat(string emailRecv){
    regex emailRegex(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b)"); //regular expression for the email format
    return regex_match(emailRecv, emailRegex);  //check the received string with the regex
}

std::vector<std::string> divideString(const std::string& str, char delimiter = '-') {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}

void substituteWhiteSpaces(string& input, bool to_from){
    const unsigned int size = input.length();
    for(unsigned int i = 0 ; i < size ; i++){
        if(input.at(i) == ' ' && to_from == false){
            input.at(i) = '_';
        } else if(input.at(i) == '_' && to_from == true){
            input.at(i) = ' ';
        }
    }
}

string insertLineFromKeyboard(){
    std::string body;
    std::getline(std::cin, body);
    return body;
}

unsigned int countOccurrencies(string input, char c){
    const unsigned int size = input.length();
    unsigned int count = 0;
    for(unsigned int i = 0 ; i < size ; i++){
        if(input.at(i) == c){
            count++;
        }
    }
    return count;
}


#endif