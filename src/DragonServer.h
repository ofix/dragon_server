#pragma once
#include <string>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../lib/httplib.h"
#include "EasyLogger.h"
#include "Global.h"
#include "Request.h"
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
    Dragon::Url parseUrl(const std::string& path);
    void installServerErrorHandlers();
    std::string httpBaiscAuthentication(const std::string& username, const std::string& password);
    void forward(const httplib::Request& request,
                 httplib::Response& response);  // 数据转发
    void processForwardResponse(httplib::Result& forward_result,
                                const Dragon::Url& url,
                                int status_code,
                                const httplib::Request& origin_request,
                                httplib::Response& origin_response);
    bool parseJson(const std::string& s, Json::Value& v);
    std::string toJson(Json::Value& v, bool pretty = true);
    void addRequest(const std::string& method,
                    const Dragon::Url& url,
                    const std::string& parameters,
                    const std::string& response,
                    const int response_code);
    std::string getRequestHost();
    // 保存所有请求到文件
    bool saveRequestsToFile();

   private:
    std::string m_data_dir;       // json模拟数据目录
    httplib::SSLServer m_server;  // HTTPS 服务端
    std::vector<Dragon::Request> m_requests;
    std::vector<int> m_flushRequets;  // 用户勾选需要序列化的请求
    std::map<std::string, bool> m_cache;
};