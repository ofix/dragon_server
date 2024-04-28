#include "DragonServer.h"
#include "Base64.h"
#include "Color.h"
#include "FileUtils.h"

DragonServer::DragonServer() : m_server{"./dragon.crt", "./dragon.key"} {
    loadIniConfigFile();
    loadRequestsFile();
}

DragonServer::DragonServer(std::string& data_dir)
    : m_data_dir{data_dir}, m_server{"./dragon.crt", "./dragon.key"} {
    loadIniConfigFile();
    loadRequestsFile();
}

void DragonServer::loadIniConfigFile() {
    m_ini.load("dragon.ini");
    m_accessControlAllowHeaders =
        m_ini["Access-Control-Allow-Headers"]["Headers"].as<std::string>();
    if (m_accessControlAllowHeaders.length() <= 0) {
        m_accessControlAllowHeaders =
            "Origin, Content-Type, X-Auth-Token, X-Dragon-Extra, X-Dragon-Mock";
    } else {
        if (m_accessControlAllowHeaders.find("X-Dragon-Extra") == std::string::npos) {
            m_accessControlAllowHeaders += ", X-Dragon-Extra";
        }
        if (m_accessControlAllowHeaders.find("X-Dragon-Mock") == std::string::npos) {
            m_accessControlAllowHeaders += ", X-Dragon-Mock";
        }
    }
    m_authUser = m_ini["Authentication"]["username"].as<std::string>();
    if (m_authUser.length() <= 0) {
        m_authUser = "root";
    }
    m_authPwd = m_ini["Authentication"]["password"].as<std::string>();
    if (m_authPwd.length() <= 0) {
        m_authPwd = "0penBmc";
    }
    m_serverPort = m_ini["Server"]["port"].as<uint16_t>();
    if (m_serverPort > 65535 || m_serverPort <= 0) {
        m_serverPort = 8888;
    }
}

DragonServer::~DragonServer() {}

bool DragonServer::loadDataDir() {
    std::vector<std::string> files = FileUtils::loadDir(m_data_dir, "json");
    for (size_t i = 0; i < files.size(); i++) {
        std::cout << files[i] << std::endl;
    }
    return true;
}

void DragonServer::installServerErrorHandlers() {
    m_server.set_error_handler([](const httplib::Request& request, httplib::Response& response) {
        auto fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
        char buf[BUFSIZ];
        snprintf(buf, sizeof(buf), fmt, response.status);
        response.set_content(buf, "text/html");
    });
    m_server.set_exception_handler(
        [](const httplib::Request& req, httplib::Response& res, std::exception_ptr ep) {
            auto fmt = "<h1>Error 500</h1><p>%s</p>";
            char buf[BUFSIZ];
            try {
                std::rethrow_exception(ep);
            } catch (std::exception& e) {
                snprintf(buf, sizeof(buf), fmt, e.what());
            } catch (...) {  // See the following NOTE
                snprintf(buf, sizeof(buf), fmt, "Unknown Exception");
            }
            res.set_content(buf, "text/html");
            res.status = StatusCode::InternalServerError_500;
        });
}

// 序列化所有的请求
std::string DragonServer::serializeAllRequests() {
    json result = json::array();
    for (auto request : m_requests) {
        json item = getRequestJson(request);
        result.push_back(item);
    }
    std::string data = result.dump(4);
    return data;
}

json DragonServer::getRequestJson(Dragon::Request& request) {
    json item = json::object();
    json url = json::object();
    url["hostname"] = request.url.hostname;
    url["port"] = request.url.port;
    url["protocol"] = request.url.protocol;
    url["path"] = request.url.path;
    item["@dragon.url"] = url;
    item["@dragon.method"] = request.method;
    item["@dragon.status_code"] = request.status_code;
    if (request.parameters != "") {
        json parameters = json::parse(request.parameters);
        item["@dragon.parameters"] = parameters;
    } else {
        item["@dragon.parameters"] = "";
    }
    if (request.response != "") {
        json response = json::parse(request.response);
        item["@dragon.response"] = response;
    } else {
        item["@dragon.response"] = "{}";
    }
    item["@dragon.duration"] = request.duration;
    item["@dragon.request_time"] = request.request_time;
    if (request.url_alias != "") {
        item["@dragon.url_alias"] = request.url_alias;
    } else {
        item["@dragon.url_alias"] = request.url.path;
    }
    return item;
}

bool DragonServer::saveRequestsToFile() {
    if (m_requests.size() <= 0) {
        return true;
    }
    std::ofstream out;
    out.open("dragon_data.json", ios::out);
    out.clear();
    std::string data = serializeAllRequests();
    out << data;
    out.flush();
    out.close();
    return true;
}

bool DragonServer::loadRequestsFile() {
    std::string data = FileUtils::loadFile("dragon_data.json");
    m_requests.clear();
    m_cache.clear();
    if (data != "") {
        json arr = json::parse(data);
        for (json::iterator it = arr.begin(); it != arr.end(); ++it) {
            Dragon::Request request;
            Dragon::Url url;
            json _url = (*it)["@dragon.url"];
            url.hostname = _url["hostname"].template get<std::string>();
            url.protocol = _url["protocol"].template get<std::string>();
            url.path = _url["path"].template get<std::string>();
            url.port = _url["port"].template get<int>();
            request.url = url;
            request.status_code = (*it)["@dragon.status_code"].template get<int>();
            request.method = (*it)["@dragon.method"].template get<std::string>();
            json parameters = (*it)["@dragon.parameters"];
            request.parameters = parameters.dump(4);
            json response = (*it)["@dragon.response"];
            request.response = response.dump(4);
            request.request_time = (*it)["@dragon.request_time"].template get<std::string>();
            request.duration = (*it)["@dragon.duration"].template get<int64_t>();
            if ((*it).contains("@dragon.url_alias")) {
                request.url_alias = (*it)["@dragon.url_alias"].template get<std::string>();
            } else {
                request.url_alias = url.path;
            }
            m_requests.push_back(request);
            size_t nIndex = m_requests.size() - 1;
            m_cache.insert(
                {request.method + url.hostname + url.path, static_cast<int16_t>(nIndex)});
        }
    }
}

void DragonServer::run() {
    loadDataDir();
    installServerErrorHandlers();
    m_server.Get("/", [this](const httplib::Request& request, httplib::Response& response) {
        response.set_content("dragon server is working!", "text/plain");
    });
    // 设置接口url别名
    m_server.Post("/url_alias",
                  [this](const httplib::Request& request, httplib::Response& response) {
                      json body = json::parse(request.body);
                      const std::string full_url = body["url"].template get<std::string>();
                      Dragon::Url url = parseUrl(full_url);
                      const std::string url_alias = body["url_alias"].template get<std::string>();
                      const std::string method = body["method"].template get<std::string>();
                      auto it = m_cache.find(method + url.hostname + url.path);
                      if (it != std::end(m_cache)) {
                          auto _request = m_requests[it->second];
                          _request.url_alias = url_alias;  // 修改url_alias
                      }
                      setHttpCorsHeaders(request, response);
                      response.set_content("{code:200,status:\"ok\"}", "application/json");
                  });
    // 批量设置接口url别名
    m_server.Post(
        "/batch_url_alias", [this](const httplib::Request& request, httplib::Response& response) {
            json arr = json::parse(request.body);
            for (json::iterator it = arr.begin(); it != arr.end(); ++it) {
                const std::string full_url = (*it)["url"].template get<std::string>();
                Dragon::Url url = parseUrl(full_url);
                const std::string url_alias = (*it)["url_alias"].template get<std::string>();
                const std::string method = (*it)["method"].template get<std::string>();
                auto ptr = m_cache.find(method + url.hostname + url.path);
                if (ptr != std::end(m_cache)) {
                    auto _request = m_requests[ptr->second];
                    _request.url_alias = url_alias;
                }
            }

            setHttpCorsHeaders(request, response);
            response.set_content("{code:200,status:\"ok\"}", "application/json");
        });
    // 返回所有监听的请求
    m_server.Get("/history", [this](const httplib::Request& request, httplib::Response& response) {
        std::string data = serializeAllRequests();
        response.set_content(data, "application/json");
    });
    m_server.Get("/request", [this](const httplib::Request& request, httplib::Response& response) {
        if (!request.has_param("url")) {
            response.set_content("{\"error\":\"parameter url missing!\"}", "application/json");
            std::string error = "/request parmeter url missing!";
            std::cerr << RED(error) << std::endl;
            return;
        }
        auto requst_time = std::chrono::high_resolution_clock::now();
        const std::string full_url = request.get_param_value("url");
        Dragon::Url url = parseUrl(full_url);
        const std::string method = request.get_param_value("method");
        auto it = m_cache.find(method + url.hostname + url.path);
        if (it != std::end(m_cache)) {
            auto request = m_requests[it->second];
            json result = getRequestJson(request);
            response.set_content(result.dump(4), "application/json");
        } else {
            httplib::Request new_request = request;  // 转换请求参数
            new_request.params.emplace("redirect_url", full_url);
            new_request.method = method;
            forward(new_request, response);
        }
    });
    m_server.Get("/proxy", [this](const httplib::Request& request, httplib::Response& response) {
        forward(request, response);
    });
    m_server.Post("/proxy", [this](const httplib::Request& request, httplib::Response& response) {
        forward(request, response);
    });
    m_server.Put("/proxy", [this](const httplib::Request& request, httplib::Response& response) {
        forward(request, response);
    });
    m_server.Patch("/proxy", [this](const httplib::Request& request, httplib::Response& response) {
        forward(request, response);
    });
    m_server.Delete("/proxy", [this](const httplib::Request& request, httplib::Response& response) {
        forward(request, response);
    });
    m_server.Options("/proxy", [this](const httplib::Request& request,
                                      httplib::Response& response) { forward(request, response); });
    std::string version{"0.5"};
    copyright(version);
    m_server.listen("localhost", m_serverPort);
}

// 解析URL
Dragon::Url DragonServer::parseUrl(const std::string& path) {
    Dragon::Url url = {"", "", -1, ""};
    size_t pos_host;
    size_t pos_port;
    size_t len = path.size();
    for (size_t i = 0; i < len; i++) {
        if (path.at(i) == ':') {
            if (path.at(i + 1) == '/' && path.at(i + 2) == '/') {
                url.protocol = path.substr(0, i);
                i += 3;
                pos_host = i;
            } else {
                pos_port = i + 1;
                url.hostname = path.substr(pos_host, pos_port - pos_host);
                while (path.at(i + 1) >= '0' && path.at(i + 1) <= '9') {
                    i++;
                }
                std::string port = path.substr(pos_port, i - pos_port + 1);
                url.port = std::stoi(port);
            }
        } else if (path.at(i) == '/') {  // http://www.baidu.com/
            if (url.hostname == "") {
                url.hostname = path.substr(pos_host, i - pos_host);
            }
            url.path = path.substr(i, len - i);
            break;
        }
    }
    // http://www.baidu.com
    if (url.hostname == "") {
        url.hostname = path.substr(pos_host, len - pos_host);
    }
    // http://www.baidu.com
    if (url.path == "") {
        url.path = "/";
    }
    // https://www.baidu.com/xxx
    if (url.port == -1) {
        if (url.protocol.compare("https") == 0) {
            url.port = 443;
        } else if (url.protocol.compare("http") == 0) {
            url.port = 80;
        }
    }
    return url;
}

std::string DragonServer::httpBaiscAuthentication(const std::string& username,
                                                  const std::string& password) {
    std::string auth = username + ':' + password;
    std::string basic = base64_encode(auth);
    return "Basic " + basic;
}

void DragonServer::outputResponseError(httplib::Result& result) {
    auto err = result.error();
    std::cout << "Status code:" << result->status << ",HTTP error: " << httplib::to_string(err)
              << std::endl;
}

bool DragonServer::isMockRequest(const httplib::Request& request) {
    return request.has_header("X-Dragon-Mock");
}

void DragonServer::setHttpCorsHeaders(const httplib::Request& request,
                                      httplib::Response& response) {
    // 允许跨域访问
    response.set_header("Access-Control-Allow-Origin", request.get_header_value("origin"));
    response.set_header("Access-Control-Allow-Methods", "GET, POST, PATCH, PUT, DELETE, OPTIONS");
    response.set_header("Access-Control-Allow-Credentials", "true");
    response.set_header("Access-Control-Allow-Headers", m_accessControlAllowHeaders);
}

void DragonServer::forward(const httplib::Request& request, httplib::Response& response) {
    // 检查参数是否正确
    if (!request.has_param("redirect_url")) {
        response.set_content("parameter redirect_url missing!", "text/plain");
        std::string error = "redirect_url not found!";
        std::cerr << RED(error) << std::endl;
        return;
    }
    auto requst_time = std::chrono::high_resolution_clock::now();
    const std::string path = request.get_param_value("redirect_url");
    std::string basic = httpBaiscAuthentication(m_authUser, m_authPwd);
    httplib::Headers headers = {{"Authorization", basic}, {"Accept", "*/*"}};
    Dragon::Url url = parseUrl(path);

    // 检查HTTP表头是否包含X-Dragon-Mock
    bool is_mock_request = isMockRequest(request);
    const char* log_format = is_mock_request ? "[bmc][cache] %s, %s\n" : "[bmc] %s, %s\n";
    gLogger->log(log_format, request.method.c_str(), path.c_str());
    if (is_mock_request) {
        auto it = m_cache.find(request.method + url.hostname + url.path);
        setHttpCorsHeaders(request, response);  // 允许跨域
        if (it != std::end(m_cache)) {
            auto key = m_requests[it->second];
            json result = getRequestJson(key);
            json dragon_response = result["@dragon.response"];
            response.set_content(dragon_response.dump(4), "application/json");
            // 将响应结果打印到控制台
            outputRequestDebugInfo(request, response);
        }
        return;
    }
    // 转发真实的请求
    std::string upstream_url = url.protocol + "://" + url.hostname + ":" + std::to_string(url.port);
    httplib::Client client(upstream_url);
    std::string content_type = request.get_header_value("Content-Type");
    client.enable_server_certificate_verification(false);
    if (request.method == "GET") {
        if (auto res = client.Get(url.path, headers)) {
            if (res->status == StatusCode::OK_200) {
                processForwardResponse(res, url, res->status, request, response, requst_time);
            } else {
                outputResponseError(res);
            }
        }
    } else if (request.method == "POST") {
        if (auto res = client.Post(url.path, headers, request.body, content_type)) {
            if (res->status == StatusCode::OK_200 || res->status == StatusCode::Created_201) {
                processForwardResponse(res, url, res->status, request, response, requst_time);
            } else {
                outputResponseError(res);
            }
        }
    } else if (request.method == "PATCH") {
        if (auto res = client.Patch(url.path, headers, request.body, content_type)) {
            if (res->status == StatusCode::OK_200) {
                processForwardResponse(res, url, res->status, request, response, requst_time);
            } else {
                outputResponseError(res);
            }
        }
    } else if (request.method == "PUT") {
        if (auto res = client.Put(url.path, headers, request.body, content_type)) {
            if (res->status == StatusCode::OK_200) {
                processForwardResponse(res, url, res->status, request, response, requst_time);
            } else {
                outputResponseError(res);
            }
        }
    } else if (request.method == "DELETE") {
        if (auto res = client.Delete(url.path, headers, request.body, content_type)) {
            if (res->status == StatusCode::OK_200) {
                processForwardResponse(res, url, res->status, request, response, requst_time);
            } else {
                outputResponseError(res);
            }
        }
    } else if (request.method == "OPTIONS") {
        // 允许跨域访问
        setHttpCorsHeaders(request, response);
        // response.set_header("Access-Control-Allow-Origin", request.get_header_value("origin"));
        // response.set_header("Access-Control-Allow-Methods",
        //                     "GET, POST, PATCH, PUT, DELETE, OPTIONS");
        // response.set_header("Access-Control-Allow-Credentials", "true");
        // response.set_header("Access-Control-Allow-Headers", m_accessControlAllowHeaders);
        // 解决CORS跨域，浏览器每次发送真实请求前，会额外方式一次OPTION请求,返回HTTP CODE
        // 200,数据任意字符即可
        response.set_content("{}", "application/json");
    }
}

/**
 * @todo 处理转发请求响应
 * @param forward_result 转发请求响应结果
 * @param url 原始请求
 * @param status_code 转发请求HTTPS响应状态码
 * @param origin_request 原始的转发请求
 * @param origin_response 原始的转发响应
 */
void DragonServer::processForwardResponse(
    httplib::Result& forward_result,
    const Dragon::Url& url,
    int status_code,
    const httplib::Request& origin_request,
    httplib::Response& origin_response,
    const std::chrono::system_clock::time_point request_time) {
    auto response_time = std::chrono::high_resolution_clock::now();
    // 将返回的结果序列化为JSON
    try {
        json root = json::parse(forward_result->body);
        auto duration =
            std::chrono::duration_cast<chrono::milliseconds>(response_time - request_time).count();
        std::string pretty_request_time = getFormatTime(request_time);
        // 允许跨域访问
        setHttpCorsHeaders(origin_request, origin_response);
        origin_response.set_header("X-Dragon-Extra", origin_request.body);  // 返回用户传递的参数
        // 返回响应
        origin_response.set_content(forward_result->body, "application/json");
        origin_response.status = forward_result->status;
        // 添加请求到缓存"
        addRequest(origin_request.method, url, origin_request.body, forward_result->body,
                   status_code, pretty_request_time, duration);
        // 将响应结果打印到控制台
        outputRequestDebugInfo(origin_request, origin_response);
    } catch (json::parse_error& error) {
        std::cerr << error.what() << std::endl;
    }
}

// 打印调试信息
void DragonServer::outputRequestDebugInfo(const httplib::Request& request,
                                          httplib::Response& response,
                                          bool is_cache) {
    const std::string path = request.get_param_value("redirect_url");
    std::string clr_url = "";
    std::string request_url =
        now() + (is_cache ? " [bmc][cache] " : " [bmc] ") + request.method + ", " + path;
    if (request.method == "GET") {
        clr_url = GREEN(request_url);
    } else if (request.method == "POST") {
        clr_url = YELLOW(request_url);
    } else if (request.method == "PATCH") {
        clr_url = BLUE(request_url);
    } else if (request.method == "PUT") {
        clr_url = MAGENTA(request_url);
    } else if (request.method == "DELETE") {
        clr_url = RED(request_url);
    } else {
        clr_url = request_url;
    }
    std::cout << clr_url << std::endl;

    if (request.body.length() > 0) {
        const char* log_prefix = is_cache ? "[bmc][cache] payload: %s\n" : "[bmc] payload: %s\n";
        gLogger->log(log_prefix, request.body.c_str());
        std::string payload =
            now() + (is_cache ? " [bmc][cache]" : " [bmc]") + " payload: " + request.body;
        std::cout << YELLOW(payload) << std::endl;
    }

    std::cout << response.body << std::endl;
}

std::string DragonServer::getFormatTime(const std::chrono::system_clock::time_point tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    // std::tm tm = *std::gmtime(&time);  // GMT (UTC)
    std::tm tm = *std::localtime(&time);  // Locale time-zone, usually UTC by default.
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void DragonServer::addRequest(const std::string& method,
                              const Dragon::Url& url,
                              const std::string& parameters,
                              const std::string& response,
                              const int response_code,
                              std::string pretty_request_time,
                              int64_t duration) {
    Dragon::Request request = {};
    request.method = method;
    request.url = url;
    request.parameters = parameters;
    request.status_code = response_code;
    request.response = response;
    request.request_time = pretty_request_time;
    request.duration = duration;  // 以ms为单位的请求响应耗时
    if (m_cache.find(method + url.hostname + url.path) == std::end(m_cache)) {
        m_requests.push_back(request);
        std::size_t nIndex = m_requests.size() - 1;
        m_cache.insert({method + url.hostname + url.path, static_cast<int16_t>(nIndex)});
    }
}

std::string DragonServer::getRequestHost() {
    if (m_requests.size() > 0) {
        return m_requests[0].url.hostname;
    }
    return "";
}

bool DragonServer::generateCode() {
    std::ofstream out;
    out.open("dragon_code_" + getRequestHost() + ".json", ios::out | ios::app);
    out.clear();                      // 清空已有数据
    if (m_flushRequets.size() > 0) {  // 刷新用户设置的请求
        for (size_t i = 0; i < m_requests.size(); i++) {
            auto request = m_requests[m_flushRequets[i]];
            request.flush(out);
        }
    } else {  // 刷新所有请求
        for (size_t i = 0; i < m_requests.size(); i++) {
            auto request = m_requests[i];
            request.flush(out);
        }
    }
    out.flush();
    out.close();
    return true;
}

void DragonServer::copyright(std::string& version) {
    std::cout << "*********************************************************************************"
                 "*****************\n";
    std::cout << "*                                                                                "
                 "                *\n";
    std::cout << "*     ________                                       ________                    "
                 "                *\n";
    std::cout << "*     ___  __ \\____________ _______ _____________    __  ___/______________   "
                 "______________     *\n";
    std::cout << "*     __  / / /_  ___/  __ `/_  __ `/  __ \\_  __ \\   _____ \\_  _ \\_  ___/_ | "
                 "/ /  _ \\_  ___/     *\n";
    std::cout << "*     _  /_/ /_  /   / /_/ /_  /_/ // /_/ /  / / /   ____/ //  __/  /   __ |/ // "
                 " __/  /         *\n";
    std::cout << "*     /_____/ /_/    \\__,_/ _\\__, / \\____//_/ /_/    /____/ \\___//_/    "
                 "_____/ \\___//_/          *\n";
    std::cout << "*                           /____/                                               "
                 "                *\n";
    std::cout << "*                                                                                "
                 "                *\n";
    std::cout << "*                          author: songhuabiao@greatwall.com.cn                  "
                 "                *\n";
    std::cout << "*                                    version:" + version +
                     "                                                 *\n";
    std::cout << "*                      server started, please visit: https://localhost:" +
                     std::to_string(m_serverPort) + "                      *\n";
    std::cout << "*********************************************************************************"
                 "*****************\n";
}