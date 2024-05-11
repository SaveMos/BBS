#include <string>
#include <vector>
#include <iostream>
#include <limits>
#include <regex>

using namespace std;

#ifndef UTILITY_H
#define UTILITY_H

bool checkEmailFormat(string emailRecv){
    //regular expression for the email format
    regex emailRegex(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b)");

    //check the received string with the regex
    return regex_match(emailRecv, emailRegex);
}

vector<string> divideString(string input , char delimiter = '-') {
    std::vector<std::string> result;
    std::string part;
    
    // Trova la posizione del carattere '-'
    size_t dashPos = input.find(delimiter);

    if (dashPos != std::string::npos) { // Se il carattere '-' è stato trovato
        // Prende la sottostringa prima del carattere '-' e la aggiunge al vettore
        result.push_back(input.substr(0, dashPos));

        // Prende la sottostringa dopo il carattere '-' e la aggiunge al vettore
        result.push_back(input.substr(dashPos + 1));
    } else { // Se non c'è il carattere '-', restituisce la stringa originale come unico elemento del vettore
        result.push_back(input);
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
    string body = "";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Pulisce il buffer di input
    std::getline(std::cin, body); // Legge una linea di input, inclusi gli spazi bianchi
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