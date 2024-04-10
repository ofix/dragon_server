#include "DragonServer.h"
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
void DragonServer::run() {
    loadDataDir();
    m_server.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
    });
    std::cout << "started dragon server at 10000" << std::endl;
    m_server.listen("localhost", 10000);
}