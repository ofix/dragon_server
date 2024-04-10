#include "DragonServer.h"

int main(int argc, char* argv[]) {
    DragonServer server;
    server.run();
    std::cout << "dragon server exit" << std::endl;
    return 1;
}
