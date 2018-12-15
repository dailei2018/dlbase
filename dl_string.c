#include "dl_base.h"

dl_str_ch * split_to_ch_c(dl_str *str, char c){
    char *begin,*end,*cur;
    int len;

    begin = str->data;
    end = str->data + str->len;
    len = str->len;

    dl_str_ch *head_ch = malloc(sizeof(dl_str_ch));
    dl_str_ch *ch = NULL;

    while(1){
        cur = dl_strnchr(begin, len, c);

        if(cur == NULL){
            cur = end;
        }

        if(ch == NULL){
            ch = head_ch;
        }else{
            ch->next = malloc(sizeof(dl_str_ch));
            ch = ch->next;
            ch->next = NULL;
        }

        ch->str.data = begin;
        ch->str.len = cur - begin;

        if(cur == end) break;

        begin = cur + 1;
        len -= ch->str.len+1;
    }

    return head_ch;

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

/*
 * debug
 */

void dump_ch(dl_str_ch *ch){
    char buf[1024];
    while(ch != NULL){
        if(ch->str.len == 0){
            memcpy(buf, "NULL", 5);
        }else{
            memcpy(buf, ch->str.data, ch->str.len);
            buf[ch->str.len] = '\0';
        }

        printf("%s\n", buf);
        ch = ch->next;
    }
}

char *
dl_pstrdup(dl_pool *pool, dl_str *src)
{
    char  *dst;

    dst = dl_pnalloc(pool, src->len);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src->data, src->len);

    return dst;
}

char *
dl_pstrdup_nt(dl_pool *pool, dl_str *src)
{
    char  *dst;

    dst = dl_pnalloc(pool, src->len + 1);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src->data, src->len);
    dst[src->len] = '\0';

    return dst;
}
