#ifndef __DL_HASH
#define __DL_HASH

#include "dl_base.h"

#define HASH_INIT 64;

#define H_UNIQUE    1
#define H_LIST      2
#define P_HASH      128

#define PH_UNIQUE   (H_UNIQUE | P_HASH)
#define PH_LIST     (H_LIST | P_HASH)

enum {
    DL_INT = 1,
    DL_STR,
    DL_VOID
};

typedef struct _dl_hash     dl_hash;
typedef struct _dl_hash_k   dl_hash_k;
typedef struct _dl_hash_v   dl_hash_v;
typedef struct _dl_node     dl_node;

struct _dl_hash_v{
    union{
        dl_str *s;
        void *v;
        long n;
    } v;

    int tt;
    dl_hash_v *next;
};

struct _dl_hash_k{
    dl_str       *key;
    uint         hash;
    dl_node     *next;
};

struct _dl_node{
    dl_hash_v v;
    dl_hash_k k;
};

struct _dl_hash {
    dl_pool *pool;
    int slot_total;
    int el_sum;
    int flag;

    dl_node *node;
};

dl_hash *dl_phash_init(dl_pool *pool, uint slot, int type);
int dl_phash_set_str(dl_hash *h, char *data, int len, dl_str *v);
int dl_phash_set_int(dl_hash *h, char *data, int len, long v);
int dl_phash_set_void(dl_hash *h, char *data, int len, void *v);


dl_hash *
dl_hash_init(dl_pool *pool, uint slot, int type);
int dl_hash_set_str(dl_hash *h, char *data, int len, dl_str *v);
int dl_hash_set_int(dl_hash *h, char *data, int len, long v);
int dl_hash_set_void(dl_hash *h, char *data, int len, void *v);
int dl_hash_set_str_rep(dl_hash *h, char *data, int len, dl_str *v);
int dl_hash_set_int_rep(dl_hash *h, char *data, int len, long v);
int dl_hash_set_void_rep(dl_hash *h, char *data, int len, void *v);

void free_hash(dl_hash *h);

dl_hash_v * dl_hash_get(dl_hash *h, char *data, int len);
uint dl_hash_key(uchar *data, size_t len);

/*
 * debug
 */

void dump_hash_key(dl_hash *h);

#endif
