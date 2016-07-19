#include "server.h"
#include "unique_fd.h"

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <sstream>
#include <fstream>
#include <thread>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>


namespace
{

const int fail = -1;
const int inet_aton_fail = 0;

static bool terminated = false;

void termination(int signum)
{
    terminated = true;
}

bool setSignalsHandlers()
{
    struct sigaction sa;
    sa.sa_handler = termination;
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaddset(&sa.sa_mask, SIGINT);
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, nullptr) == fail) {
        perror("sigaction");
        return false;
    }
    if (sigaction(SIGINT, &sa, nullptr) == fail) {
        perror("sigaction");
        return false;
    }
    return true;
}

bool init(const http_server::Options& options)
{

    if (options.daemon()) {
        if (daemon(0, 1) == fail) {
            perror("daemon");
            return false;
        }
    }

    if (options.verbose()) {
        std::cout << "The server started as "
                  << (options.daemon()? "daemon": "application")
                  << std::endl;
        std::cout << "pid = " << getpid() << std::endl;
    }

    return setSignalsHandlers();
}

aux::UniqueFd makeListeningSocket()
{
    //aux::UniqueFd sd {socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)};
    aux::UniqueFd sockfd {socket(AF_INET, SOCK_STREAM, 0)};
    if (!sockfd) {
        perror("socket");
        return aux::UniqueFd();
    }

    const int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET,
                   SO_REUSEADDR, &yes, sizeof(yes)) == fail) {
        perror("setsockopt");
        return aux::UniqueFd();
    }

    return sockfd;
}

bool startListen(const aux::UniqueFd& sockfd, const http_server::Options& options)
{
    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(options.port());
    if (inet_aton(options.ip(), &addr_in.sin_addr) == inet_aton_fail) {
        std::cerr << "inet_aton: " << "The address '"
                  << options.ip() << "' is wrong" << std::endl;
        return false;
    }

    if (options.verbose()) {
        std::cout << "Listening: "
                  << inet_ntoa(addr_in.sin_addr) << ":" << ntohs(addr_in.sin_port)
                  << std::endl;
    }

    if (bind(sockfd,
             reinterpret_cast<struct sockaddr*>(&addr_in),
             sizeof(addr_in)) == fail) {
        perror("bind");
        return false;
    }

    if (listen(sockfd, SOMAXCONN) == fail) {
        perror("listen");
        return false;
    }

    return true;
}

void outputBinaryData(const char * data, size_t size)
{
    std::cout << std::setfill('0');
    for(size_t i = 0; i < size; ++i)
        std::cout << std::setw(2) << std::hex
                  << static_cast<int>(data[i]) << " ";
    std::cout << std::setfill(' ');
    std::cout << std::dec;
    std::cout << std::endl;
    std::cout << std::endl;
}

int sendData(int sockfd, const char * data, size_t size, bool verbose, std::ofstream& logout)
{
    logout << std::string(data, size);

    if (verbose)
    {
        std::cout << "Send: ";
        outputBinaryData(data, size);
    }

    int res = send(sockfd, data, size, 0);
    if (res == -1)
        perror("send");

    return res;
}

int sendBadRequest(int sockfd, bool verbose, std::ofstream& logout)
{
    static const char badRequest[] = "HTTP/1.0 400 Bad Request\r\n"
                                     "Content-Type: text/html\r\n"
                                     "Content-Length: 43\r\n"
                                     "\r\n"
                                     "<html><body>400 Bad Request</body></html>\r\n";

    return sendData(sockfd, badRequest, sizeof(badRequest) - 1, verbose, logout);
}

int sendNotFound(int sockfd, bool verbose, std::ofstream& logout)
{
    static const char notFound[] = "HTTP/1.0 404 Not Found\r\n"
                                   "Content-Type: text/html\r\n"
                                   "Content-Length: 41\r\n"
                                   "\r\n"
                                   "<html><body>404 Not Found</body></html>\r\n";

    return sendData(sockfd, notFound, sizeof(notFound) - 1, verbose, logout);
}

int sendAnswer(int sockfd, const std::string& data, bool verbose, std::ofstream& logout)
{
    std::ostringstream os;
    os << "HTTP/1.0 200 OK\r\n" 
       << "Content-Type: text/html\r\n"
       << "Content-Length: " << data.length() << "\r\n"
       << "\r\n"
       << data;

    return sendData(sockfd, os.str().c_str(), os.str().length(), verbose, logout);
}

void clientWorker(aux::UniqueFd&& sockfd,
                  const sockaddr_in peerAddr,
                  const http_server::Options& options)
{
    aux::UniqueFd clientfd(std::move(sockfd));

    std::string logName;
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        std::ostringstream os;
        os << "/tmp/" << tv.tv_sec << "_" << tv.tv_usec << ".log";
        logName = os.str();
        os.str("");
    }

    std::ofstream logout(logName);

    if (options.verbose()) {
        std::cout << "Connection with: "
                  << inet_ntoa(peerAddr.sin_addr)
                  << ":" << ntohs(peerAddr.sin_port)
                  << std::endl;
    }
    logout << "Connection with: "
           << inet_ntoa(peerAddr.sin_addr)
           << ":" << ntohs(peerAddr.sin_port)
           << "\n" << std::endl;

    char buf[1024];
    ssize_t received = recv(clientfd, &buf, sizeof(buf), 0);
    switch (received)
    {
        case -1:
            perror("recv");
            break;
        case 0:
            break;
        default:
            if (options.verbose()) {
                std::cout << "Received: ";
                outputBinaryData(buf, received);
            }

            logout << std::string(buf, received);
            logout.flush();

            const char* getLine = std::strtok(buf, "\r\n");

            if ((getLine == NULL) || (strncmp(getLine, "GET ", 4) != 0))
            {
                sendBadRequest(clientfd, options.verbose(), logout);
                return;
            }

            const char* protocolStr = std::strrchr(getLine, 'H');
            if ((protocolStr == NULL) || (std::strncmp(protocolStr, "HTTP/", 5) != 0))
            {
                sendBadRequest(clientfd, options.verbose(), logout);
                return;
            }

            std::string filename(getLine + 4, protocolStr - getLine - 5);

            std::filebuf file;
            if (file.open(options.directory() + filename, std::ios_base::in) == nullptr)
            {
                sendNotFound(clientfd, options.verbose(), logout);
                return;
            }

            using std::ios_base;
            auto posEnd = file.pubseekoff(0, ios_base::end);
            file.pubseekpos(0);

            //sendAnswer(clientfd, filename, options.verbose());

            if (options.verbose())
                std::cout << "Sending header..." << std::endl;

            std::ostringstream os;
            int fileSize = posEnd;
            os << "HTTP/1.0 200 OK\r\n" 
               << "Content-Type: text/html\r\n"
               << "Content-Length: " << (fileSize - 1) << "\r\n"
               << "\r\n";

            int read = 0;
            do
            {
                read = file.sgetn(buf, sizeof(buf));
                os << std::string(buf, read);
            } while(read == sizeof(buf));

            if (sendData(clientfd, os.str().c_str(), os.str().length(), options.verbose(), logout) == -1)
                return;
/*
            if (options.verbose())
                std::cout << "Sending content..." << std::endl;
            int read = 0;
            do
            {
                read = file.sgetn(buf, sizeof(buf));
                if (sendData(clientfd, buf, read, options.verbose()) == -1)
                    return;
            } while(read == sizeof(buf));
*/
            break;
    }
}

} /* anonymous namespace */

int http_server::run(const http_server::Options& options)
{
    if (!init(options))
        return EXIT_FAILURE;

    aux::UniqueFd sockfd { makeListeningSocket() };
    if (!sockfd)
        return EXIT_FAILURE;

    if (!startListen(sockfd, options))
        return EXIT_FAILURE;

    std::ofstream logout("/tmp/mainfinal.log");
    logout << "start" << std::endl;

    while(!terminated) {
        struct sockaddr_in peerAddr;
        socklen_t peerAddrSize = sizeof(peerAddr);
        aux::UniqueFd clientfd {
            accept(sockfd,
                   reinterpret_cast<sockaddr*>(&peerAddr),
                   &peerAddrSize)
            };

        if (clientfd) {
            //clientWorker(std::move(clientfd), peerAddr, options);
            logout << "Connection with: "
                   << inet_ntoa(peerAddr.sin_addr)
                   << ":" << ntohs(peerAddr.sin_port)
                   << "\n" << std::endl;
            std::thread t(clientWorker, std::move(clientfd), peerAddr, options);
            t.detach();
        } else {
            perror("accept");
        }
    }

    if (options.verbose())
        std::cout << "The server stoped" << std::endl;

    logout << "stop\n";
    logout.close();

    return 0;
}

