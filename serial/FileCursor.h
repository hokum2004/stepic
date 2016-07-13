#ifndef __FILECURSOR_H__
#define __FILECURSOR_H__

#include <fstream>

class FileCursor
{
public:
    using pos_type = std::filebuf::pos_type;
    FileCursor(std::filebuf& file, pos_type begin, pos_type end);

    pos_type posBegin() const;
    pos_type posEnd() const;

    int getValue() const;
    bool isEnd() const;
    bool next();
private:
    std::filebuf& file;
    const pos_type begin;
    const pos_type end;
    pos_type cur;
    int value;
};

#endif //!__FILECURSOR_H__
