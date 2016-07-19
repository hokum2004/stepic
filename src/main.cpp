#include "options.h"
#include "server.h"

#include <iostream>

int main(int argc, char* const argv[])
{
    http_server::Options options = http_server::parseArgs(argc, argv);
    std::cout << "host:port: " << options.ip() << ":" << options.port()
              << "\ndirectory: " << options.directory()
              << "\nis daemon: " << std::boolalpha << options.daemon()
              << std::endl;

    return http_server::run(options);
}

