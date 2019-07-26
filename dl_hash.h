/*
    support one key, multiple value -- H_LIST
    support replacement.
    support delete key.
    node length can not be changed
*/

#ifndef __DL_HASH
#define __DL_HASH

#include "dl_base.h"

#define HASH_INIT 64;

#define H_UNIQUE    1
#define H_LIST      2

typedef void (*dl_h_void_replace_pt)(void *v);

enum {
    DL_H_NULL,
    DL_H_INT,
    DL_H_STR,
    DL_H_VOID
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
    dl_log      *log;
    int         slot_total;
    int         el_sum;
    int         flag;
    
    dl_h_void_replace_pt    void_h;     //will be called before custom data replaced or deleted
    
    dl_node     *node;
};


dl_hash *
dl_hash_init(dl_log *log, uint slot, dl_h_void_replace_pt handler, int type);
int dl_hash_set_str(dl_hash *h, char *k, size_t len_k, char *v, size_t len_v);
int dl_hash_set_int(dl_hash *h, char *k, size_t len_k, long v);
int dl_hash_set_void(dl_hash *h, char *k, size_t len_k, void *v);

int dl_hash_set_str_rep(dl_hash *h, char *k, size_t len_k, char *v, size_t len_v);
int dl_hash_set_int_rep(dl_hash *h, char *k, size_t len_k, long v);
int dl_hash_set_void_rep(dl_hash *h, char *k, size_t len_k, void *v);


dl_hash_v * dl_hash_find(dl_hash *h, char *data, int len);
uint dl_hash_key(uchar *data, size_t len);

void dl_hash_del(dl_hash *h, char *key, size_t len);
void dl_hash_destroy(dl_hash *h);

void dl_dump_hash(dl_hash *h);

/*
 * debug
 */

void dump_hash_key(dl_hash *h);

#endif
