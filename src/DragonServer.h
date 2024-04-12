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
    std::string httpBaiscAuthentication(const std::string& username, const std::string& password);
    void forward(const httplib::Request& request,
                 httplib::Response& response);  // 数据转发
    void processForwardResponse(httplib::Result& forward_result,
                                const httplib::Request& origin_request,
                                httplib::Response& origin_response);
    bool parseJson(const std::string& s, Json::Value& v);
    std::string toJson(Json::Value& v, bool pretty = true);

   private:
    std::string m_data_dir;       // json模拟数据目录
    httplib::SSLServer m_server;  // HTTPS 服务端
};