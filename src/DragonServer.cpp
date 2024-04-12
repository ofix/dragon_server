#include "DragonServer.h"
#include "Base64.h"
#include "FileUtils.h"

DragonServer::DragonServer() : m_server{"./dragon.crt", "./dragon.pem"} {}

DragonServer::DragonServer(std::string& data_dir)
    : m_data_dir{data_dir}, m_server{"./dragon.crt", "./dragon.pem"} {}

DragonServer::~DragonServer() {}

bool DragonServer::loadDataDir() {
    std::vector<std::string> files = FileUtils::loadDir(m_data_dir, "json");
    for (size_t i = 0; i < files.size(); i++) {
        std::cout << files[i] << std::endl;
    }
    return true;
}

void DragonServer::installServerErrorHandlers() {
    // m_server.set_logger([](const httplib::Request& request, const httplib::Response& response) {
    //     std::string path = request.path;
    //     gLogger->log("path:%s, method:%s", path.c_str(), request.method.c_str());
    // });
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

void DragonServer::run() {
    loadDataDir();
    installServerErrorHandlers();
    m_server.Get("/", [this](const httplib::Request& request, httplib::Response& response) {
        response.set_content("dragon server is working!", "text/plain");
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
    std::string version{"0.5"};
    copyright(version);
    m_server.listen("localhost", 8888);
}

// 解析URL
Url DragonServer::parseUrl(const std::string& path) {
    Url url = {"", "", -1, ""};
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

void DragonServer::forward(const httplib::Request& request, httplib::Response& response) {
    if (!request.has_param("redirect_url")) {
        response.set_content("parameter redirect_url missing!", "text/plain");
        return;
    }
    const std::string path = request.get_param_value("redirect_url");
    gLogger->log("[bmc] %s, %s\n", request.method.c_str(), path.c_str());
    gLogger->log("[bmc] %s\n", request.body.c_str());
    std::string basic = httpBaiscAuthentication("root", "0penBmc");
    httplib::Headers headers = {{"Authorization", basic}, {"Accept", "*/*"}};
    const std::string method = request.method;
    Url url = parseUrl(path);
    std::string upstream_url = url.protocol + "://" + url.hostname + ":" + std::to_string(url.port);
    httplib::Client client(upstream_url);
    client.enable_server_certificate_verification(false);
    if (method == "GET") {
        if (auto res = client.Get(url.path, headers)) {
            if (res->status == StatusCode::OK_200) {
                processForwardResponse(res, request, response);
            } else {
                auto err = res.error();
                std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
            }
        }
    } else if (method == "POST") {
        if (auto res = client.Post(url.path, headers, request.body, "application/json")) {
            if (res->status == StatusCode::OK_200) {
                processForwardResponse(res, request, response);
            } else {
                auto err = res.error();
                std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
            }
        }
    } else if (method == "PATCH") {
        if (auto res = client.Patch(url.path, headers, request.body, "application/json")) {
            if (res->status == StatusCode::OK_200) {
                processForwardResponse(res, request, response);
            } else {
                auto err = res.error();
                std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
            }
        }
    } else if (method == "PUT") {
        if (auto res = client.Put(url.path, headers, request.body, "application/json")) {
            if (res->status == StatusCode::OK_200) {
                processForwardResponse(res, request, response);
            } else {
                auto err = res.error();
                std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
            }
        }
    } else if (method == "DELETE") {
        if (auto res = client.Delete(url.path, headers, request.body, "application/json")) {
            if (res->status == StatusCode::OK_200) {
                processForwardResponse(res, request, response);
            } else {
                auto err = res.error();
                std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
            }
        }
    }
}

bool DragonServer::parseJson(const std::string& s, Json::Value& v) {
    Json::CharReaderBuilder readerBuilder;
    std::istringstream iss(s);
    std::string errs;
    bool parsingSuccessful = Json::parseFromStream(readerBuilder, iss, &v, &errs);
    if (!parsingSuccessful) {
        std::cerr << "Failed to parse JSON: " << errs << std::endl;
        return false;
    }
    return true;
}

std::string DragonServer::toJson(Json::Value& v, bool pretty) {
    if (pretty) {
        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, v);
    } else {
        Json::FastWriter fastWriter;
        return fastWriter.write(v);
    }
}

void DragonServer::processForwardResponse(httplib::Result& forward_result,
                                          const httplib::Request& origin_request,
                                          httplib::Response& origin_response) {
    // 将返回的结果序列化为JSON
    Json::Value root;
    parseJson(forward_result->body, root);
    Json::Value origin_request_body;
    parseJson(origin_request.body, origin_request_body);
    root["DragonMeta"] = origin_request_body;
    std::string result = toJson(root);
    origin_response.set_content(result, "application/json");
    std::cout << result << std::endl;
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
    std::cout << "*                      server started, please visit: https://localhost:8888      "
                 "                *\n";
    std::cout << "*********************************************************************************"
                 "*****************\n";
}