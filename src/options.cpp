#include "options.h"

#include <iostream>

#include <unistd.h>

namespace
{

void printUsage(const char * name)
{
    std::cerr
        << "Usage: " << name
        << " [-h <ip>] [-p <port>] [-d <directory>] [-n] [-v]"
        << std::endl;
}

} /* anonymous namespace */

namespace http_server
{

Options::Options():
    ip_("0.0.0.0"),
    port_(1080),
    directory_("."),
    daemon_(true),
    verbose_(false)
{}

void Options::setIp(const char* ip)
{
    ip_ = ip;
}

void Options::setPort(short port)
{
    port_ = port;
}

void Options::setDirectory(const char* directory)
{
    directory_ = directory;
}

void Options::setDaemon(bool daemon)
{
    daemon_ = daemon;
}

void Options::setVerbose(bool verbose)
{
    verbose_ = verbose;
}

const char* Options::ip() const
{
    return ip_.c_str();
}

short Options::port() const
{
    return port_;
}

const std::string& Options::directory() const
{
    return directory_;
}

bool Options::daemon() const
{
    return daemon_;
}

bool Options::verbose() const
{
    return verbose_;
}

} /* namespace http_server */


http_server::Options http_server::parseArgs(int argc, char* const argv[])
{
    http_server::Options options;

    const char * const optstring = "h:p:d:nv";
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
            case 'v':
                options.setVerbose(true);
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
