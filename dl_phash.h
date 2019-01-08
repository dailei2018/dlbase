/*
    memory pool
    support one key, multiple value -- H_LIST
    do not support delete elements
    node length can not be changed
*/

#ifndef __DL_PHASH
#define __DL_PHASH

#include "dl_base.h"

#define PHASH_INIT 64;

#define PH_UNIQUE   1
#define PH_LIST     2

enum {
    DL_PH_NULL,
    DL_PH_INT,
    DL_PH_STR,
    DL_PH_VOID
};

typedef struct _dl_phash     dl_phash;
typedef struct _dl_phash_k   dl_phash_k;
typedef struct _dl_phash_v   dl_phash_v;
typedef struct _dl_pnode     dl_pnode;

struct _dl_phash_v{
    union{
        dl_str *s;
        void *v;
        long n;
    } v;

    int tt;
    dl_phash_v *next;
};

struct _dl_phash_k{
    dl_str       *key;
    uint         hash;
    dl_pnode     *next;
};

struct _dl_pnode{
    dl_phash_v v;
    dl_phash_k k;
};

struct _dl_phash {
    dl_pool *pool;
    int slot_total;
    int el_sum;
    int flag;

    dl_pnode *node;
};

dl_phash *dl_phash_init(dl_pool *pool, uint slot, int type);
int dl_phash_set_str(dl_phash *h, char *data, int len, dl_str *v);
int dl_phash_set_int(dl_phash *h, char *data, int len, long v);
int dl_phash_set_void(dl_phash *h, char *data, int len, void *v);

dl_phash_v * dl_phash_find(dl_phash *h, char *data, int len);

void dl_dump_phash(dl_phash *h);

#endif