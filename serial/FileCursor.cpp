#include "FileCursor.h"

#include "Cursor.h"

namespace
{

class Exception: public std::exception
{
public:
    Exception(const std::string& message) :
        message(message)
    {}

    const char* what() const noexcept override
    {
        return message.c_str();
    }
private:
    const std::string message;
};

} //anonymous namespace

FileCursor::FileCursor(const char * name) throw (std::exception):
    file(),
    base(nullptr)
{
    if (file.open(name, std::ios_base::in) == NULL)
        throw Exception(std::string("FileCursor: Can't open file '") + name + '"');
    base.reset(new Cursor(file, 0, -1));
}

FileCursor::pos_type FileCursor::posBegin() const
{
    return base->posBegin();
}

FileCursor::pos_type FileCursor::posEnd() const
{
    return base->posEnd();
}

int FileCursor::getValue() const
{
    return base->getValue();
}

bool FileCursor::isEnd() const
{
    return base->isEnd();
}

bool FileCursor::isEndOfRun() const
{
    return base->isEndOfRun();
}

void FileCursor::resetEndOfRun()
{
    base->resetEndOfRun();
}

bool FileCursor::next()
{
    return base->next();
}


