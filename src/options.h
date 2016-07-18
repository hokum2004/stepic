#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <string>

namespace http_server
{

class Options
{
public:
    Options();
    void setIp(const char * ip);
    void setPort(const short port);
    void setDirectory(const char * directory);
    void setDaemon(bool daemon);
    void setVerbose(bool verbose);

    const char* ip() const;
    short port() const;
    const std::string& directory() const;
    bool daemon() const;
    bool verbose() const;
private:
    std::string ip_;
    short port_;
    std::string directory_;
    bool daemon_;
    bool verbose_;
};

Options parseArgs(int argc, char* const argv[]);

} /* namespace http_server */

#endif //!__OPTIONS_H__
