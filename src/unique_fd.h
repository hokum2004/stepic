#ifndef __UNIQUE_FD_H__
#define __UNIQUE_FD_H__

namespace aux
{

class UniqueFd
{
public:
    static const int nullfd = {-1};

    explicit UniqueFd(int fd = nullfd);
    UniqueFd(UniqueFd&& that);
    ~UniqueFd();

    UniqueFd(const UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;

    void reset(int fd = nullfd);
    operator bool() const;
    operator int() const;
    int get() const;
private:
    int fd;
};

} /* namespace aux */

#endif //!__UNIQUE_FD_H__
