#ifndef __FILECURSOR_H__
#define __FILECURSOR_H__

#include "ICursor.h"
#include <fstream>
#include <exception>
#include <memory>

class FileCursor: public ICursor
{
public:
    FileCursor(const char * name) throw (std::exception);

    pos_type posBegin() const override;
    pos_type posEnd() const override;

    int getValue() const override;
    bool isEnd() const override;
    bool isEndOfRun() const override;
    void resetEndOfRun() override;
    bool next() override;
private:
    std::filebuf file;
    std::unique_ptr<ICursor> base;
};

#endif //!__FILECURSOR_H__
