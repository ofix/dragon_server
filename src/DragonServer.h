#pragma once
#include <string>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "lib/httplib.h"

class DragonServer {
   public:
    DragonServer();
    DragonServer(std::string& data_dir);
    bool loadDataDir();
    virtual ~DragonServer();
    void run();

   private:
    std::string m_data_dir;       // json模拟数据目录
    httplib::SSLServer m_server;  // HTTPS
};