#include "../../dataStructures/messageStructures/simpleMessage.h"
#include "../../dataStructures/messageStructures/contentMessage.h"
#include "../../dataStructures/messageStructures/RSAEMessage.h"
#include "../securityLib/security.h"

#ifndef MESSAGEPACKINGLIBRARY_H
#define MESSAGEPACKINGLIBRARY_H


string packSimpleMessage(string recvString)
{
    string ret;
    SimpleMessage msg;
    msg.setContent(recvString);
    msg.concatenateFields(ret);
    return ret;
}

string packContentMessage(string recvString, string K)
{
    string ret;
    ContentMessage msg;
    msg.setIV(generateRandomKey(16));
    msg.setC(vectorUnsignedCharToString(encrypt_AES(recvString, K)));
    msg.setHMAC(calculateHMAC(msg.getIV(), msg.getC()));
    msg.concatenateFields(ret);
    return ret;
}

string ContentMessageGetContent(ContentMessage& msg, string K){
    return decrypt_AES(msg.getC() , K);
}

bool verifyContentMessageHMAC(string result , ContentMessage& msg)
{
    msg.deconcatenateAndAssign(result);
    return verifyHMAC(msg.getIV(), msg.getC(), msg.getHMAC());
}


bool verifyContentMessageHMAC(string result)
{
    ContentMessage msg;
    return verifyContentMessageHMAC(result , msg);
}

#endif