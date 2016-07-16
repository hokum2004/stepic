#include "options.h"

namespace http_server
{

Options::Options():
    ip_("0.0.0.0"),
    port_(1080),
    directory_("."),
    daemon_(true)
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


} /* namespace http_server */
