#include "FileCursor.h"

#include <ios>
#include <cassert>
#include <limits>

namespace
{

using namespace std;

FileCursor::pos_type findBegin(std::streambuf& file, FileCursor::pos_type desireBegin)
{
    const auto endPos = file.pubseekoff(0, ios_base::end, ios_base::in);
    auto cur = file.pubseekpos(desireBegin, ios_base::in);

    while(cur != endPos)
    {
        const auto curSymbol = static_cast<char>(file.sgetc());
        if (std::isdigit(curSymbol))
            return cur;

        file.snextc();
        cur = file.pubseekoff(0, ios_base::cur, ios_base::in);
    }

    return cur;
}

FileCursor::pos_type findEnd(std::streambuf& file, FileCursor::pos_type desireEnd)
{
    const auto endPos = file.pubseekoff(0, ios_base::end, ios_base::in);

    if (desireEnd == -1)
        return endPos;

    auto cur = file.pubseekoff(desireEnd, ios_base::beg, ios_base::in);

    while(cur != endPos)
    {
        const auto curSymbol = static_cast<char>(file.sgetc());
        if (std::isspace(curSymbol))
            return cur;

        file.snextc();
        cur = file.pubseekoff(0, ios_base::cur, ios_base::in);
    }

    return cur;
}

} //anonymous namespace

FileCursor::FileCursor(std::streambuf& file, pos_type begin_, pos_type end_):
    file(file),
    begin(findBegin(file,begin_)),
    end(findEnd(file, end_)),
    cur(begin),
    value(-1),
    lastValue(std::numeric_limits<int>::max()),
    isEnd_(false)
{
    assert(begin != end);
    next();
}

FileCursor::pos_type FileCursor::posBegin() const
{
    return begin;
}

FileCursor::pos_type FileCursor::posEnd() const
{
    return end;
}

int FileCursor::getValue() const
{
    return value;
}

bool FileCursor::isEnd() const
{
    return isEnd_;
}

bool FileCursor::isEndOfRun() const
{
    return isEnd() || (value < lastValue);
}

bool FileCursor::next()
{
    lastValue = value;
    bool isRead;
    std::tie(cur, value, isRead) = readValue();

    if (!isRead) {
        isEnd_ = true;
        return false;
    }

    return true;
}

std::tuple<FileCursor::pos_type, int /*value*/, bool /*end*/> FileCursor::readValue()
{
    file.pubseekpos(cur, ios_base::in);
    pos_type newCur = cur;
    int read = 0;
    bool isRead = false;

    while(newCur != end)
    {
        const auto curSymbol = static_cast<char>(file.sbumpc());
        newCur = file.pubseekoff(0, ios_base::cur, ios_base::in);

        if (std::isdigit(curSymbol)) {
            read = read * 10 + (curSymbol - '0');
            isRead = true;
        }
        else
            break;
    }

    return std::make_tuple(newCur, (isRead? read: -1), isRead);
}

