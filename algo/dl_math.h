#ifndef __DL_MATH
#define __DL_MATH

#include "dl_base.h"

#define ceillog2(x)	(dl_log2((x)-1) + 1)

long dl_log2(ulong x);

#endif