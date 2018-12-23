#ifndef __DL_STRING
#define __DL_STRING

#include "dl_base.h"

typedef struct _dl_str dl_str;

struct _dl_str{
    size_t len;
    char *data;
};

#define dl_string(str)     { sizeof(str) - 1, (char *) str};

char *dl_strnchr(char *str, int len, char c);
dl_array * dl_split_by_c(dl_array *arr, dl_str *str, char c);


char *dl_pstrdup(dl_pool *pool, char *data, size_t len);
char *dl_pstrdup_nt(dl_pool *pool, char *data, size_t len);

/*
 * debug
 */


#endif
