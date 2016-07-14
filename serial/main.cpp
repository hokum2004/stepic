#include "mergesort.h"
#include <iostream>


int main(int argc, const char * const argv[])
{
    if (argc < 2) {
        std::cerr << "The input file is not set." << std::endl;
        return 1;
    }

    mergesort(argv[1]);

    return 0;
}
