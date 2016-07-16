#include "options.h"

#include <iostream>
#include <cstdlib>

#include <unistd.h>

void printUsage(const char * name)
{
    std::cerr
        << "Usage: " << name
        << " [-h <ip>] [-p <port>] [-d <directory>] [-n]"
        << std::endl;
}

http_server::Options parseArgs(int argc, char* const argv[])
{
    http_server::Options options;

    const char * const optstring = "h:p:d:n";
    int opt = -1;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch(opt) {
            case 'h':
                options.setIp(optarg);
                break;
            case 'p':
                options.setPort(std::atoi(optarg));
                break;
            case 'd':
                options.setDirectory(optarg);
                break;
            case 'n':
                options.setDaemon(false);
                break;
            case '?':
                printUsage(argv[0]);
            default:
                exit(EXIT_FAILURE);
                break;
        }
    }
    return options;
}

int main(int argc, char* const argv[])
{
    http_server::Options options = parseArgs(argc, argv);
    std::cout << "listening: " << options.ip() << ":" << options.port()
              << "\ndirectory: " << options.directory()
              << "\nis daemon: " << std::boolalpha << options.daemon()
              << std::endl;
    return 0;
}
