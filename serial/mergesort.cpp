#include "mergesort.h"
#include "FileCursor.h"
#include <iostream>

int copyRun(ICursor& cursor, std::ofstream& out)
{
    if (cursor.isEnd())
        return 0;

    do
    {
        out << cursor.getValue() << " ";
        cursor.next();
    }
    while(!cursor.isEndOfRun());

    return 1;
}

int split(ICursor& cursor, const char * out1, const char * out2)
{
    using namespace std;
    ofstream f1;
    f1.open(out1, ios_base::out | ios_base::trunc);
    if (!f1.is_open())
    {
        std::cerr << "split: Can't open file '" << out1 << "'" << std::endl;
        return -1;
    }

    ofstream f2;
    f2.open(out2, ios_base::out | ios_base::trunc);
    if( !f2.is_open())
    {
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

bool merge(ICursor& cur1, ICursor& cur2, const char* out)
{
    using namespace std;
    ofstream os;
    os.open(out, ios_base::out | ios_base::trunc);
    if (!os.is_open())
    {
        std::cerr << "merge: Can't open file '" << out << "'" << std::endl;
        return false;
    }

    do
    {
        cur1.resetEndOfRun();
        cur2.resetEndOfRun();
        do
        {
            if (cur1.getValue() < cur2.getValue()) {
                os << cur1.getValue() << " ";
                cur1.next();
            }
            else {
                os << cur2.getValue() << " ";
                cur2.next();
            }
        }
        while(!cur1.isEndOfRun() && !cur2.isEndOfRun());

        if (!cur1.isEndOfRun())
            copyRun(cur1, os);

        if (!cur2.isEndOfRun())
            copyRun(cur2, os);
    }
    while(!cur1.isEnd() && !cur2.isEnd());

    while(!cur1.isEnd())
        copyRun(cur1, os);

    while(!cur2.isEnd())
        copyRun(cur2, os);

    return true;
}

void mergesort(const char * filename)
{
    const char * const bufs[] = { "buf0.tmp", "buf1.tmp", "buf2.tmp" };

    FileCursor curBase(filename);
    int countOfRun = split(curBase, bufs[0], bufs[1]);

    while (countOfRun > 1) {
        {
            FileCursor c0(bufs[0]);
            FileCursor c1(bufs[1]);
            merge(c0, c1, bufs[2]);
        }

        FileCursor seq(bufs[2]);
        countOfRun = split(seq, bufs[0], bufs[1]);
    }
}

