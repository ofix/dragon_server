#include "EasyLogger.h"

const string EasyLogger::m_fileName = "dragon.log";
EasyLogger* EasyLogger::m_pThis = NULL;
ofstream EasyLogger::m_logFile;

EasyLogger::EasyLogger() {}

EasyLogger* EasyLogger::Instance() {
    if (m_pThis == NULL) {
        m_pThis = new EasyLogger();
        m_logFile.open(m_fileName.c_str(), ios::out | ios::app);
    }
    return m_pThis;
}

void EasyLogger::log(const char* format, ...) {
    char* message = NULL;
    int nSize = 0;
    va_list args;
    va_start(args, format);

// 计算缓存大小
#ifdef _WIN32
    nSize = _vscprintf(format, args) + 1;
#else
    nSize = vsnprintf(NULL, 0, format, args) + 1;  // 计算格式化的字符串需要保存的空间大小
#endif
    message = new char[nSize];
    memset(message, 0, nSize);
#ifdef _WIN32
    vsprintf_s(message, nSize, format, args);
#else
    vsprintf(message, format, args);
#endif
    m_logFile << now() << ":\t";
    m_logFile << message << "\n";
    va_end(args);

    delete[] message;
}

void EasyLogger::log(const string& message) {
    m_logFile << now() << ":\t";
    m_logFile << message << "\n";
}

EasyLogger& EasyLogger::operator<<(const string& message) {
    m_logFile << "\n" << now() << ":\t";
    m_logFile << message << "\n";
    return *this;
}
