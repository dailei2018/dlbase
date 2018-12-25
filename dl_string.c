#include "dl_base.h"

dl_array * dl_split_by_c(dl_array *arr, dl_str *str, char c)
{
    if(str == NULL || arr == NULL) return NULL;

    char *begin,*end,*cur;
    int len;

    begin = str->data;
    end = str->data + str->len;
    len = str->len;

    dl_str *el;

    for(;;){
        cur = dl_strnchr(begin, len, c);
        if(cur == NULL) cur = end;

        el = dl_array_push(arr);
        if(el == NULL) return NULL;
        
        
        el->len = cur - begin;
        el->data = begin;

        if(cur == end) break;

        begin = cur + 1;
        
        len = end - begin;
    }

    return arr;
}

char *dl_strnchr(char *str, int len, char c){
    char *cur;
    int i;

    cur = str;

    while(len--){
        if(*cur == c) return cur;

        cur++;
    }

    return NULL;
}


char *
dl_pstrdup(dl_pool *pool, char *data, size_t len)
{
    char  *dst;

    dst = dl_pnalloc(pool, len);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, data, len);

    return dst;
}

char *
dl_pstrdup_nt(dl_pool *pool, char *data, size_t len)
{
    char  *dst;

    dst = dl_pnalloc(pool, len + 1);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, data, len);
    dst[len] = '\0';

    return dst;
}


int
dl_atoi(char *line, size_t n)
{
    int  value, cutoff, cutlim;

    if (n == 0) {
        return DL_ERROR;
    }

    cutoff = DL_INT32_MAX / 10;
    cutlim = DL_INT32_MAX % 10;

    for (value = 0; n--; line++) {
        if (*line < '0' || *line > '9') {
            return DL_ERROR;
        }

        if (value >= cutoff && (value > cutoff || *line - '0' > cutlim)) {
            return DL_ERROR;
        }

        value = value * 10 + (*line - '0');
    }

    return value;
}