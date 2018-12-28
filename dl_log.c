#include "dl_base.h"

static void dl_log_core(int level, dl_log *log, const char *format, va_list args);

static char *level_str[] = {
    "stderr", "emerg", "alert",
    "crit", "err", "warn", "notice",
    "info", "debug"
};

void dl_log_error(int level, dl_log *log, const char *format, ...){
    if(log == NULL || level > log->log_level) return;
    
    va_list args;
    
    va_start(args, format);
    dl_log_core(level, log, format, args);
    
}

static void dl_log_core(int level, dl_log *log, const char *format, va_list args)
{
    int             len,s;
    char            buffer[DL_MAX_ERROR_STR_LEN], *cur, *last;
    
    cur = buffer;
    last = buffer + DL_MAX_ERROR_STR_LEN - 1;
    
    struct tm *tm1;
    
    time_t sec = time(NULL);
    tm1 = localtime(&sec);
    strftime(cur, 20, "%Y-%m-%d %H:%M:%S", tm1);
    cur += 19;
    
    cur = dl_sprintf(cur, " \- %s: ", level_str[log->log_level]);
    cur = dl_vslprintf(cur, last, format, args);
    *cur++ = '\n';
    
    write(log->file.fd, buffer, cur - buffer);
}

dl_log * dl_log_init(int level, char *fname){
    dl_log *log = malloc(sizeof(dl_log));
    if(log == NULL) return NULL;
    
    log->log_level = level;
    log->file.name.len = strlen(fname);
    log->file.name.data = strdup(fname);
    if(log->file.name.data == NULL) return NULL;
    
    log->file.fd = open(fname, O_RDWR|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);
    if(log->file.fd == -1) return NULL;
    
    return log;
}

void dl_log_free(dl_log *log){
    close(log->file.fd);
    free(log->file.name.data);
    free(log);
}


void exit_log(int err, int level, dl_log *log, const char *fmt, ...)
{
    if(log == NULL) return;
    
    va_list args;
    
    va_start(args, fmt);
    dl_log_core(level, log, fmt, args);
    
    exit(err);
}
