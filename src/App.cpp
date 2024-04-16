#include "DragonServer.h"

#ifdef _WIN32
#include <stdio.h>
#include <windows.h>
#elif __linux__
#include <signal.h>
#include <unistd.h>
#endif

static DragonServer* pServer = nullptr;

#ifdef _WIN32
BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        std::cout << "dragon server exit" << std::endl;
        if (pServer) {
            delete pServer;
        }
    }
    return TRUE;
}
#elif __linux__
void exit_handler(int s) {
    if (pServer) {
        pServer->saveRequestsToFile();
        delete pServer;
    }
    exit(0);  // 退出程序
}
#endif

int main(int argc, char* argv[]) {
    pServer = new DragonServer();

#ifdef _WIN32
    if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        printf("\nERROR: Could not set control handler");
    }
#elif __linux__
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = exit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
#endif
    pServer->run();
    return 0;
}
