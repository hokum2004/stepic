#ifndef __AUXSORT_H__
#define __AUXSORT_H__

#include "ICursor.h"

#include <fstream>

namespace aux_sort
{

int copyRun(ICursor& cursor, std::ofstream& out);
int split(ICursor& cursor, const char * out1, const char * out2);
bool merge(ICursor& cur1, ICursor& cur2, const char* out);

} /*namespace aux_sort*/

#endif //!__AUXSORT_H__

