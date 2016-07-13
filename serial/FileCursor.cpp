#include "FileCursor.h"

#include <ios>
#include <cassert>

namespace
{

using namespace std;

FileCursor::pos_type findBegin(std::filebuf& file, FileCursor::pos_type desireBegin)
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

FileCursor::pos_type findEnd(std::filebuf& file, FileCursor::pos_type desireEnd)
{
    const auto endPos = file.pubseekoff(0, ios_base::end, ios_base::in);

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

FileCursor::FileCursor(std::filebuf& file, pos_type begin_, pos_type end_):
    file(file),
    begin(findBegin(file,begin_)),
    end(findEnd(file, end_)),
    cur(begin),
    value(-1)
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
    return cur == end;
}

bool FileCursor::next()
{
    file.pubseekpos(cur, ios_base::in);
    int read = 0;
    bool isRead = false;
    while(!isEnd())
    {
        const auto curSymbol = static_cast<char>(file.sbumpc());
        cur = file.pubseekoff(0, ios_base::cur, ios_base::in);

        if (std::isdigit(curSymbol)) {
            read = read * 10 + (curSymbol - '0');
            isRead = true;
        }
        else
            break;
    }
    if (!isRead)
        return false;

    value = read;
    return true;
}
