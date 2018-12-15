#ifndef __DL_STRING
#define __DL_STRING

#include "dl_base.h"

typedef struct _dl_str_ch dl_str_ch;
typedef struct _dl_str dl_str;

struct _dl_str{
    char *data;
    int len;

    int flag;       // 1 means data is a null terminate string
};

struct _dl_str_ch{
    dl_str str;
    dl_str_ch *next;
};

char *dl_strnchr(char *str, int len, char c);
dl_str_ch * split_to_ch_c(dl_str *str, char c);


char *dl_pstrdup(dl_pool *pool, dl_str *src);
char *dl_pstrdup_nt(dl_pool *pool, dl_str *src);

/*
 * debug
 */

void dump_ch(dl_str_ch *ch);

#endif
