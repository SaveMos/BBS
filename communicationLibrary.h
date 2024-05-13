#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <signal.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>

#include <string.h>
#include <iostream>

#include "messageBBS.h"

#ifndef COMMUNICATIONLIBRARY_H
#define COMMUNICATIONLIBRARY_H


// Funzione per inviare una stringa tramite un socket
bool sendString(int socketDescriptor, string message) {
    vector<uint8_t> buffer(message.begin(), message.end()); // Converto la stringa in una sequenza di byte
    size_t length = buffer.size(); // Calcolo la lunghezza della stringa da inviare.
    // Invio la lunghezza della stringa
    if (send(socketDescriptor, &length, sizeof(length), 0) == -1) {
        cerr << "Errore nell'invio della lunghezza della stringa\n";
        return false;
    }
    // Invio i dati della stringa
    if (send(socketDescriptor, buffer.data(), buffer.size(), 0) == -1) {
        cerr << "Errore nell'invio dei dati della stringa\n";
        return false;
    }
    return true; // L'invio ha avuto successo.
}

// Funzione per ricevere una stringa tramite un socket
std::string receiveString(int socketDescriptor) {
    // Ricevo la lunghezza della stringa
    size_t length;
    if (recv(socketDescriptor, &length, sizeof(length), 0) == -1) {
        std::cerr << "Errore nella ricezione della lunghezza della stringa\n";
        return "";
    }

    // Alloco un buffer per ricevere i dati della stringa
    vector<uint8_t> buffer(length);

    // Ricevo i dati della stringa
    if (recv(socketDescriptor, buffer.data(), length, 0) == -1) {
        std::cerr << "Errore nella ricezione dei dati della stringa\n";
        return "";
    }

    // Converto il buffer in una stringa
    string message(buffer.begin(), buffer.end());
    return message;
}


void sendBBSMessage(int sd, messageBBS& msg)
{
    string msgStr;
    msg.concatenateFields(msgStr);
    sendString(sd , msgStr);
}

void receiveBBSMessage(int sd, messageBBS& message)
{
    string mess = receiveString(sd);
    message.deconcatenateAndAssign(mess);
}

void sendIntegerNumber(int sd, int mess)
{
    uint32_t msg = htonl(mess);
    if (send(sd, (void *)&msg, sizeof(uint32_t), 0) < 0)
    {
        pthread_exit((void *)NULL);
    }
}

int receiveIntegerNumber(int sd)
{
    uint32_t msg = 0;
    if (recv(sd, (void *)&msg, sizeof(uint32_t), 0) < 0)
    {
        return -1;
    }
    else
    {
        return (int)(ntohl(msg));
    }
}
#endif