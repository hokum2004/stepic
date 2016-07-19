#include "unique_fd.h"

#include <iostream>
#include <unistd.h>

namespace aux
{

UniqueFd::UniqueFd(int fd):
    fd(fd)
{
    //std::cout << __PRETTY_FUNCTION__ << ": " << this->fd << std::endl;
}

UniqueFd::UniqueFd(UniqueFd&& that):
    fd(that.fd)
{
    //std::cout << __PRETTY_FUNCTION__ << ": " << this->fd << std::endl;
    that.fd = nullfd;
}

UniqueFd::~UniqueFd()
{
    //std::cout << __PRETTY_FUNCTION__ << ": " << this->fd << std::endl;
    reset(nullfd);
}

void UniqueFd::reset(int fd)
{
    int oldFd = this->fd;
    this->fd = fd;

    if (oldFd != nullfd)
        close(oldFd);
}

UniqueFd::operator bool() const
{
    return fd != nullfd;
}

UniqueFd::operator int() const
{
    return fd;
}

int UniqueFd::get() const
{
    return fd;
}

} /* namespace aux */

