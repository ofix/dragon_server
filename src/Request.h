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
    std::string method;
    Url url;
    std::string parameters;
    std::string response;
    int code;
};

}  // namespace Dragon
