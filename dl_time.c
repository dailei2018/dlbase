#include "dl_base.h"

/*
    waited to be optimized with setitimer and cache the time,
    do not update time within 1 second.
*/

static char dl_time_buf[64];

time_t dl_get_time_s()
{
    return time(NULL);
}

ulong dl_get_time_ms()
{
    struct timeval tv;
    ulong msec;

    gettimeofday(&tv, NULL);
    msec = tv.tv_sec*1000 + tv.tv_usec/1000;
    
    return msec;
}

char *dl_get_datetime(time_t sec)
{
    struct tm *tm1;

    sec = sec ? sec : time(NULL);
    
    tm1 = localtime(&sec);
    strftime(dl_time_buf, 64, "%Y-%m-%d %H:%M:%S", tm1);
    
    return dl_time_buf;
}