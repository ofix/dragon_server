#pragma once

#include <cstdarg>
#include <fstream>
#include <iostream>
#include <string>
#include "Global.h"

using namespace std;
#define gLogger EasyLogger::Instance()

class EasyLogger {
   public:
    void log(const std::string& message);
    void log(const char* format, ...);
    EasyLogger& operator<<(const string& message);
    static EasyLogger* Instance();

   private:
    EasyLogger();
    EasyLogger(const EasyLogger&){};
    EasyLogger& operator=(const EasyLogger&) { return *this; };
    static const std::string m_fileName;
    static EasyLogger* m_pThis;
    static ofstream m_logFile;
};