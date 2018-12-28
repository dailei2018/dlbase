#ifndef __DL_STRING
#define __DL_STRING

#include "dl_base.h"

#define dl_cpymem(dst, src, n)   (((char *) memcpy(dst, src, n)) + (n))

typedef struct _dl_str dl_str;

struct _dl_str{
    size_t len;
    char *data;
};

#define dl_string(str)     { sizeof(str) - 1, (char *) str};

void exit_msg(int err, const char *fmt, ...);
void dl_printf(const char *fmt, ...);
char * dl_sprintf(char *buf, const char *fmt, ...);
char * dl_snprintf(char *buf, size_t max, const char *fmt, ...);
char * dl_slprintf(char *buf, char *last, const char *fmt, ...);
char * dl_vslprintf(char *buf, char *last, const char *fmt, va_list args);

char *dl_strnchr(char *str, int len, char c);
dl_array * dl_split_by_c(dl_array *arr, dl_str *str, char c);

char *dl_pstrdup(dl_pool *pool, char *data, size_t len);
char *dl_pstrdup_nt(dl_pool *pool, char *data, size_t len);
char *dl_strdup_nt(char *data, size_t len);
int dl_atoi(char *line, size_t n);
/*
 * debug
 */


#endif
