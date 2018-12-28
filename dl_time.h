#ifndef __DL_TIME
#define __DL_TIME

#include "dl_base.h"

time_t dl_get_time_s();
ulong dl_get_time_ms();
char *dl_get_datetime(time_t sec);


#endif