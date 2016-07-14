#ifndef __FILECURSOR_H__
#define __FILECURSOR_H__

#include <fstream>
#include <tuple>

class FileCursor
{
public:
    using pos_type = std::filebuf::pos_type;
    FileCursor(std::filebuf& file, pos_type begin, pos_type end);

    pos_type posBegin() const;
    pos_type posEnd() const;

    int getValue() const;
    bool isEnd() const;
    bool isEndOfRun() const;
    bool next();
private:
    std::tuple<pos_type, int /*value*/, bool /*end*/> readValue();

    std::filebuf& file;
    const pos_type begin;
    const pos_type end;
    pos_type cur;
    int value;
    int lastValue;
    bool isEnd_;
};

#endif //!__FILECURSOR_H__
