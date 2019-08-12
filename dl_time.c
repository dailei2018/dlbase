#include "dl_base.h"

/*
    waited to be optimized with setitimer and cache the time,
    do not update time within 1 second.
*/

int dl_event_timer_alarm = 1;

static __thread char dl_time_buf[64];
static __thread ulong    msec;
static __thread ulong    sec;


static void dl_update_time()
{
    struct tm *tm1;
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    sec = tv.tv_sec;
    msec = tv.tv_sec*1000 + tv.tv_usec/1000;
    
    tm1 = localtime(&sec);
    strftime(dl_time_buf, 64, "%Y-%m-%d %H:%M:%S", tm1);
    
}

ulong dl_get_time_ms()
{
    struct timeval tv;
    
    if(dl_event_timer_alarm == 1){
        dl_update_time();
        dl_event_timer_alarm = 0;
    }
    
    return msec;
}

ulong dl_get_time_s()
{
    if(dl_event_timer_alarm == 1){
        dl_update_time();
        dl_event_timer_alarm = 0;
    }
    
    return sec;
}

char *dl_get_datetime()
{
    if(dl_event_timer_alarm == 1){
        dl_update_time();
        dl_event_timer_alarm = 0;
    }
    
    return dl_time_buf;
}


/* force update time */
ulong dl_get_time_ms_f()
{
    dl_update_time();
    
    return msec;
}

ulong dl_get_time_s_f()
{
    dl_update_time();
    
    return sec;
}

char *dl_get_datetime_f()
{
    dl_update_time();
    
    return dl_time_buf;
}