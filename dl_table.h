#ifndef __DL_LHASH
#define __DL_LHASH

#include "dl_base.h"

#define MAXBITS		26
#define MAXASIZE	(1 << MAXBITS)

#define numints     (int)(sizeof(long)/sizeof(int))

enum {
    DL_NIL_L,
    DL_INT_L,
    DL_STR_L,
    DL_VOID_L
};

typedef struct _dl_key_l    dl_key_l;
typedef struct _dl_value_l  dl_value_l;
typedef struct _dl_node_l   dl_node_l;
typedef struct _dl_table    dl_table;


struct _dl_key_l{
    union{
        dl_str  *key;
        long     n;
    } k;
    
    uint        hash;
    int         tt;
    dl_node_l   *next;
};

struct _dl_value_l{
    union{
        dl_str *s;
        void *v;
        long n;
    } v;

    int tt;
};

struct _dl_node_l {
    dl_value_l    i_val;
    dl_key_l      i_key;
};

struct _dl_table {
    dl_pool             *pool;
    uchar                free;
    
    uchar                lsizenode;  /* log2 of size of `node' array */

    int                  sizearray;
    dl_value_l          *array;  /* array part */
  
    dl_node_l           *node;
    dl_node_l           *lastfree;  /* any free position is before this position */
};


dl_table *dl_table_new(dl_pool *pool,int narray, int nhash, uchar free);
dl_value_l *dl_find_by_index(dl_table *t, long index);
int dl_array_set_int_l(dl_table *t, long index, long value);
dl_table *dl_table_new(dl_pool *pool,int narray, int nhash, uchar free);

#endif