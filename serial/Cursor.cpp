#include "Cursor.h"

#include <ios>
#include <cassert>
#include <limits>

namespace
{

using namespace std;

Cursor::pos_type findBegin(std::streambuf& file, Cursor::pos_type desireBegin)
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

Cursor::pos_type findEnd(std::streambuf& file, Cursor::pos_type desireEnd)
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

Cursor::Cursor(std::streambuf& file, pos_type begin_, pos_type end_):
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

Cursor::pos_type Cursor::posBegin() const
{
    return begin;
}

Cursor::pos_type Cursor::posEnd() const
{
    return end;
}

int Cursor::getValue() const
{
    return value;
}

bool Cursor::isEnd() const
{
    return isEnd_;
}

bool Cursor::isEndOfRun() const
{
    return isEnd() || (value < lastValue);
}

void Cursor::resetEndOfRun()
{
    lastValue = value;
}

bool Cursor::next()
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

std::tuple<Cursor::pos_type, int /*value*/, bool /*end*/> Cursor::readValue()
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

