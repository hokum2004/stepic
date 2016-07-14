#ifndef __CURSOR_H__
#define __CURSOR_H__

#include <streambuf>
#include <tuple>

class Cursor
{
public:
    using pos_type = std::streambuf::pos_type;
    Cursor(std::streambuf& file, pos_type begin, pos_type end);

    pos_type posBegin() const;
    pos_type posEnd() const;

    int getValue() const;
    bool isEnd() const;
    bool isEndOfRun() const;
    bool next();
private:
    std::tuple<pos_type, int /*value*/, bool /*end*/> readValue();

    std::streambuf& file;
    const pos_type begin;
    const pos_type end;
    pos_type cur;
    int value;
    int lastValue;
    bool isEnd_;
};

#endif //!__CURSOR_H__
