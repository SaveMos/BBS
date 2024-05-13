#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;

#ifndef TIMESTAMPLIBRARY_H
#define TIMESTAMPLIBRARY_H

std::string getCurrentTimestamp() {
    // Ottieni il tempo corrente in millisecondi
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    // Ottieni il tempo da epoch in millisecondi
    auto since_epoch = ms.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
    auto fractional_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch) % 1000;

    // Ottieni il tempo da epoch in secondi
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // Formatta il timestamp in una stringa
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&currentTime), "%Y:%m:%d:%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << fractional_seconds.count();

    return oss.str();
}

bool checkTimestampFormat(string timestamp){
    return true;
}

#endif