#include "server.h"
#include "unique_fd.h"

#include <iostream>
#include <cstdlib>

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

    while(!terminated) {
        struct sockaddr_in peerAddr;
        socklen_t peerAddrSize = sizeof(peerAddr);
        aux::UniqueFd clientfd {
            accept(sockfd,
                   reinterpret_cast<sockaddr*>(&peerAddr),
                   &peerAddrSize)
            };

        if (clientfd) {
            if (options.verbose()) {
                std::cout << "Connection with: "
                          << inet_ntoa(peerAddr.sin_addr)
                          << ":" << ntohs(peerAddr.sin_port)
                          << std::endl;
            }
        } else {
            perror("accept");
        }
    }

    if (options.verbose())
        std::cout << "The server stoped" << std::endl;

    return 0;
}

