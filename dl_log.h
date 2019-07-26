#ifndef __DL_LOG
#define __DL_LOG

#include "dl_base.h"

#define DL_LOG_EMERG             1
#define DL_LOG_ALERT             2
#define DL_LOG_CRIT              3
#define DL_LOG_ERR               4
#define DL_LOG_WARN              5
#define DL_LOG_NOTICE            6
#define DL_LOG_INFO              7
#define DL_LOG_DEBUG             8

#define DL_MAX_ERROR_STR_LEN    1024

typedef struct _dl_file dl_file;
struct _dl_file{
    int fd;
    dl_str name;
};

typedef struct _dl_log dl_log;
struct _dl_log {
    int         log_level;
    dl_file     file;
};

void dl_log_error(int level, dl_log *log, const char *format, ...);
dl_log * dl_log_init(int level, char *fname);
void dl_log_free(dl_log *log);

void exit_log(int err, int level, dl_log *log, const char *fmt, ...);

#endif
