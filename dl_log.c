#include "dl_base.h"

static char *level_str[] = {
    "stderr", "emerg", "alert",
    "crit", "err", "warn", "notice",
    "info", "debug"
};

void dl_log_error(int level, dl_log *log, const char *format, ...){
    if(log == NULL || level > log->log_level) return;

    va_list         args;
    int             len,s;
    char            buffer[DL_MAX_ERROR_STR_LEN];
    
    va_start(args, format);
    
    
    /* set time */
    char buf[64];
    char *cur = buf;
    struct tm *tm1;
    
    time_t sec = time(NULL);
    tm1 = localtime(&sec);
    strftime(cur, 20, "%Y-%m-%d %H:%M:%S", tm1);
    cur += 19;
    sprintf(cur, " \- %s: ", level_str[log->log_level]);
    
    len = strlen(buf);
    
    cur = buffer;
    memcpy(cur, buf, len);
    cur += len;
    
    vsnprintf(cur, DL_MAX_ERROR_STR_LEN-len-1, format, args);
    strcat(buffer, "\n");
    
    write(log->file.fd, buffer, strlen(buffer));
    
}

dl_log * dl_log_init(int level, char *fname){
    dl_log *log = malloc(sizeof(log));
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