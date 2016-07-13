#include <fstream>
#include <iostream>

int main(int argc, const char * const argv[])
{
    if (argc < 2) {
        std::cerr << "The input file is not set." << std::endl;
        return 1;
    }
    const char * const filename = argv[1];

    std::ifstream baseFile;
    baseFile.open(filename);
    if (!baseFile.is_open()) {
        std::cerr << "Can't open file '" << filename << "'." << std::endl;
        return 1;
    }

    std::ifstream::pos_type endPos = baseFile.seekg(0, std::ios_base::end).tellg();
    baseFile.seekg(0);


    baseFile.close();
    return 0;
}
