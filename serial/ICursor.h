#ifndef __ICURSOR_H__
#define __ICURSOR_H__

#include <streambuf>

struct ICursor
{
    using pos_type = std::streambuf::pos_type;

    virtual pos_type posBegin() const = 0;
    virtual pos_type posEnd() const = 0;

    virtual int getValue() const = 0;
    virtual bool isEnd() const = 0;
    virtual bool isEndOfRun() const = 0;
    virtual void resetEndOfRun() = 0;
    virtual bool next() = 0;

    virtual ~ICursor() {}
};

#endif //!__ICURSOR_H__

