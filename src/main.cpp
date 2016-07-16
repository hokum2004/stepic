#include "options.h"

#include <iostream>
#include <cstdlib>

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static bool terminated = false;
void termination(int signum)
{
    terminated = true;
}

void setSignalsHandlers()
{
    struct sigaction sa;
    sa.sa_handler = termination;
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaddset(&sa.sa_mask, SIGINT);
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, nullptr) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* const argv[])
{
    http_server::Options options = http_server::parseArgs(argc, argv);
    std::cout << "listening: " << options.ip() << ":" << options.port()
              << "\ndirectory: " << options.directory()
              << "\nis daemon: " << std::boolalpha << options.daemon()
              << std::endl;

    if (options.daemon()) {
        if (daemon(0, 0) == -1) {
            perror("daemon");
            exit(EXIT_FAILURE);
        }
        std::cout << "The server started as daemon" << std::endl;
    } else {
        std::cout << "The server started as normal application" << std::endl;
    }

    setSignalsHandlers();

    std::cout << "pid = " << getpid() << std::endl;

    while(!terminated)
        pause();

    std::cout << "The server stoped" << std::endl;

    return 0;
}

