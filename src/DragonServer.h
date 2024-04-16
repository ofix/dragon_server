#pragma once
#include <string>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <chrono>
#include <ctime>
#include <ratio>
#include "../lib/httplib.h"
#include "../lib/nlohmann/json.hpp"
#include "../lib/nlohmann/json_fwd.hpp"
#include "EasyLogger.h"
#include "Global.h"
#include "IniFile.h"
#include "Request.h"
#include "regex"

using namespace httplib;
using json = nlohmann::json;
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
                                httplib::Response& origin_response,
                                const std::chrono::system_clock::time_point request_time);
    std::string getFormatTime(const std::chrono::system_clock::time_point tp);
    void addRequest(const std::string& method,
                    const Dragon::Url& url,
                    const std::string& parameters,
                    const std::string& response,
                    const int response_code,
                    const std::chrono::system_clock::time_point request_time,
                    const std::chrono::system_clock::time_point response_time);
    void outputRequestDebugInfo(const httplib::Request& request, httplib::Response& response);
    std::string serializeAllRequests();
    std::string getRequestHost();
    void loadIniConfigFile();
    // 保存所有请求到文件
    bool saveRequestsToFile();
    bool loadRequestsFile();
    bool generateCode();

   private:
    std::string m_data_dir;       // json模拟数据目录
    httplib::SSLServer m_server;  // HTTPS 服务端
    std::vector<Dragon::Request> m_requests;
    std::vector<int> m_flushRequets;  // 用户勾选需要序列化的请求
    std::map<std::string, bool> m_cache;
    ini::IniFile m_ini;  // 可配置选项
    std::string m_accessControlAllowHeaders;
};