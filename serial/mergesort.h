#include "ICursor.h"

#include <fstream>

int copyRun(ICursor& cursor, std::ofstream& out);
int split(ICursor& cursor, const char * out1, const char * out2);
bool merge(ICursor& cur1, ICursor& cur2, const char* out);

void mergesort(const char * filename);

