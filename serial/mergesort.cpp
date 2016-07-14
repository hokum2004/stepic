#include "mergesort.h"
#include "auxsort.h"

#include "FileCursor.h"
#include <iostream>

using namespace aux_sort;

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

