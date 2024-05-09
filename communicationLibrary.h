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

// Funzione per inviare una stringa tramite un socket
bool sendString(int socketDescriptor, std::string& message) {
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
    size_t l_dim_msg = 0, dim_msg = 0, ret = 0;
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
/*

int ricevi_intero_con_uscita(int sd)
{
    uint32_t msg = 0;
    int ret = recv(sd, (void *)&msg, sizeof(uint32_t), 0);
    if (ret < 0)
    {
        pthread_exit((void *)NULL);
    }
    else
    {
       return (int)(ntohs(msg));
    }
}


bool data_valida(int ora, int minuto, int giorno, int mese, int anno_f)
{
    // controlla che la data inserita come inpout sia valida e che sia successiva alla data odierna
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    const int giorno_current = timeinfo->tm_mday;
    const int mese_current = timeinfo->tm_mon + 1;
    const int anno_current = timeinfo->tm_year + 1900;
    const int ora_current = timeinfo->tm_hour;
    const int minuto_current = timeinfo->tm_min;

    const int anno = anno_f + 2000;
    if (minuto > 59 || minuto < 0)
    {
        return 0;
    }

    if (giorno <= 0 || mese <= 0)
    {
        return 0;
    }

    if (anno < anno_current)
    {
        return 0;
    }

    if (anno > 3000)
    {
        return 0;
    }

    if (anno == anno_current && mese_current > mese)
    {
        return 0;
    }

    if (anno == anno_current && mese_current == mese && giorno_current > giorno)
    {
        return 0;
    }

    if (anno == anno_current && mese_current == mese && giorno_current == giorno && ora_current > ora)
    {
        return 0;
    }

    if (anno == anno_current && mese_current == mese && giorno_current == giorno && ora_current == ora && minuto_current > minuto)
    {
        return 0;
    }

    if (mese == 2 && giorno >= 29) // Validità nel mese di febbraio
    {
        if (giorno > 29)
        {
            return 0;
        }

        if (giorno == 29 && ((anno % 400 != 0) && ((anno % 4 != 0) || (anno % 100 == 0))))
        {
            return 0;
        }
    }

    if (mese == 4 || mese == 6 || mese == 9 || mese == 11)
    {
        if (giorno > 30)
        {
            return 0;
        }
    }
    else
    {
        if (giorno > 31)
        {
            return 0;
        }
    }

    return 1;
}

*/