#pragma once

#include <fstream>
#include <string>

namespace Dragon {
struct Url {
    std::string protocol;
    std::string hostname;
    int port;
    std::string path;
};
struct Request {
   public:
    void flush(std::ofstream& out);
    std::string method;        // 请求方法
    Url url;                   // 请求URL路径
    std::string parameters;    // 请求参数
    std::string response;      // 响应内容
    int status_code;           // 请求状态码
    std::string request_time;  // 请求时间
    std::string duration;      // 请求耗时
};

}  // namespace Dragon
