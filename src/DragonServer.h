#pragma once
#include <string>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../lib/httplib.h"
#include "EasyLogger.h"
#include "Global.h"
#include "lib/json/json.h"
#include "regex"
using namespace httplib;

class DragonServer {
   public:
    DragonServer();
    DragonServer(std::string& data_dir);
    bool loadDataDir();
    virtual ~DragonServer();
    void run();
    void copyright(std::string& version);
    Url parseUrl(const std::string& path);
    void installServerErrorHandlers();
    void forward(const httplib::Request& request,
                 httplib::Response& response);  // 数据转发

   private:
    std::string m_data_dir;       // json模拟数据目录
    httplib::SSLServer m_server;  // HTTPS 服务端
};