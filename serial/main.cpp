#include "FileCursor.h"
#include <fstream>
#include <iostream>
#include <string>

int copyRun(FileCursor& cursor, std::filebuf& out)
{
    if (cursor.isEnd())
        return 0;

    do
    {
        std::string str = std::to_string(cursor.getValue()) + " ";
        out.sputn(str.c_str(), str.length());
        cursor.next();
    }
    while(!cursor.isEndOfRun());
/*
    if (!cursor.isEnd())
    {
        std::string str = std::to_string(cursor.getValue()) + " ";
        out.sputn(str.c_str(), str.length());
        cursor.next();
    }
*/
    return 1;
}

int split(FileCursor& cursor, const char * out1, const char * out2)
{
    using namespace std;
    filebuf f1;
    if (f1.open(out1, ios_base::out | ios_base::trunc) == NULL) {
        std::cerr << "split: Can't open file '" << out1 << "'" << std::endl;
        return -1;
    }

    filebuf f2;
    if (f2.open(out2, ios_base::out | ios_base::trunc) == NULL) {
        std::cerr << "split: Can't open file '" << out2 << "'" << std::endl;
        return -1;
    }

    int countOfRun = 0;

    while (!cursor.isEnd())
    {
        countOfRun += copyRun(cursor, f1);
        countOfRun += copyRun(cursor, f2);
    }

    return countOfRun;
}

int main(int argc, const char * const argv[])
{
    if (argc < 2) {
        std::cerr << "The input file is not set." << std::endl;
        return 1;
    }
    const char * const filename = argv[1];

    const char * const bufs[] = { "buf0.tmp", "buf1.tmp", "buf2.tmp" };

    std::filebuf baseFile;
    if (baseFile.open(filename, std::ios_base::in) == NULL) {
        std::cerr << "Can't open file '" << filename << "'." << std::endl;
        return 1;
    }

    std::filebuf::pos_type endPos = baseFile.pubseekoff(
        0,
        std::ios_base::end,
        std::ios_base::in);

    baseFile.pubseekoff(
        0,
        std::ios_base::beg,
        std::ios_base::in);


    FileCursor cur1(baseFile, 0, -1);

    int countOfRun = split(cur1, bufs[0], bufs[1]);
    std::cout << "countOfRun = " << countOfRun << std::endl;
#if 0
    std::filebuf out1;
    out1.open(bufs[0], std::ios_base::out | std::ios_base::trunc);
    copyRun(cur1, out1);

    std::filebuf out2;
    out2.open(bufs[1], std::ios_base::out | std::ios_base::trunc);
    copyRun(cur1, out2);
#endif
    std::cout << std::endl;

    return 0;
}
