#include <iostream>
#include "utilityFile.h"
using namespace std;

int main(int argc, char *argv[])
{
    messageBBS m;
    uint32_t a = 32;
    m.setId(a);
    m.setTitle("Aoooo");
    m.setAuthor("ssdsdsds");
    m.setBody("shsdhsdh");

    string s;
    m.concatenateFields(s);
    cout <<s<< endl;   


}