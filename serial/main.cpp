#include "FileCursor.h"
#include <fstream>
#include <iostream>

int main(int argc, const char * const argv[])
{
    if (argc < 2) {
        std::cerr << "The input file is not set." << std::endl;
        return 1;
    }
    const char * const filename = argv[1];

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

    baseFile.close();
    return 0;
}
