#include "auxsort.h"
#include "Cursor.h"
#include "FileCursor.h"

#include <iostream>
#include <fstream>
#include <tuple>
#include <memory>
#include <vector>
#include <cstdlib>

#include <omp.h>
#include <unistd.h>

class Filenames
{
public:
    Filenames() : part0_(), part1_(), res_() {}

    void set(const std::string& part0, const std::string& part1, const std::string& res)
    {
        Filenames::part0_ = part0;
        Filenames::part1_ = part1;
        Filenames::res_ = res;
    }

    const char* part0() const {return part0_.c_str();}
    const char* part1() const {return part1_.c_str();}
    const char* res() const {return res_.c_str();}
private:
    std::string part0_;
    std::string part1_;
    std::string res_;
};

using pos_type = std::streambuf::pos_type;
using Position = std::pair<pos_type, pos_type>;

void msort(const char * filename, Filenames& bufs, Position& pos)
{
    FileCursor curBase(filename, pos.first, pos.second);
    int countOfRun = aux_sort::split(curBase, bufs.part0(), bufs.part1());

    while (countOfRun > 1)
    {
        {
            FileCursor c0(bufs.part0());
            FileCursor c1(bufs.part1());
            aux_sort::merge(c0, c1, bufs.res());
        }

        FileCursor seq(bufs.res());
        countOfRun = aux_sort::split(seq, bufs.part0(), bufs.part1());
    }
}

int main(int argc, const char * const argv[])
{
    if (argc < 2) {
        std::cerr << "The input file is not set." << std::endl;
        return 1;
    }

    const char * filename = argv[1];

    omp_set_num_threads(4);

    int countThreads = omp_get_max_threads();
    //TODO: should  be power of 2
    if ((countThreads > 1) && ((countThreads % 2) != 0)) {
        --countThreads;
        omp_set_num_threads(countThreads);
    }

    std::cout << "omp_get_max_threads = " << omp_get_max_threads() << std::endl;

    std::unique_ptr< Filenames[] > bufs(new Filenames[countThreads]);
#pragma omp parallel for
    for (int i = 0; i < countThreads; ++i)
    {
        using std::to_string;

        const std::string name = "buf_" + to_string(i) + "-";
        bufs[i].set(name + "0.tmp", name + "1.tmp", name + "res.tmp");
    }

    std::filebuf base;
    if (base.open(filename, std::ios_base::in) == NULL)
    {
        std::cerr << "Can't open file '" << filename << "'." << std::endl;
        return 1;
    }

    std::vector< Position > positions(countThreads);
    pos_type length = base.pubseekoff(0, std::ios_base::end, std::ios_base::in);
    pos_type step = length / countThreads;
    pos_type start = 0;
    for (int i = 0; i < countThreads - 1; ++i)
    {
        Cursor cur(base, start, start + step);
        positions[i] = std::make_pair(cur.posBegin(), cur.posEnd());
        start = cur.posEnd();
    }
    positions[countThreads - 1] = std::make_pair(start, -1);

#pragma omp parallel
    {
        int t = omp_get_thread_num();
        msort(filename, bufs[t], positions[t]);
    }

    if (countThreads > 1)
    {
        while(countThreads > 0)
        {
            countThreads = countThreads / 2;
            omp_set_num_threads(((countThreads == 0) ? 1: countThreads));
#pragma omp parallel
            {
                int t = omp_get_thread_num() * 2;
                std::string cmd = std::string("cp ") + bufs[t].res() + " " + bufs[t].part0();
                system(cmd.c_str());
                cmd = std::string("cp ") + bufs[t+1].res() + " " + bufs[t+1].part0();
                system(cmd.c_str());

                FileCursor part0(bufs[t].part0());
                FileCursor part1(bufs[t+1].part0());
                aux_sort::merge(part0, part1, bufs[t].res());
            }
        }
    }

    return 0;
}
