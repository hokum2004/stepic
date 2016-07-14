#ifndef __CURSOR_H__
#define __CURSOR_H__

#include "ICursor.h"
#include <streambuf>
#include <tuple>

class Cursor: public ICursor
{
public:
    Cursor(std::streambuf& file, pos_type begin, pos_type end);

    pos_type posBegin() const override;
    pos_type posEnd() const override;

    int getValue() const override;
    bool isEnd() const override;
    bool isEndOfRun() const override;
    void resetEndOfRun() override;
    bool next() override;
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
