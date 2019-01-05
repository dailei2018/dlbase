#ifndef __DL_LHASH
#define __DL_LHASH

#include "dl_base.h"

#define MAXBITS		26
#define MAXASIZE	(1 << MAXBITS)

#define numints     (int)(sizeof(long)/sizeof(int))

#define DL_NIL_L    1
#define DL_INT_L    2
#define DL_STR_L    4
#define DL_VOID_L   8

#define DL_FREE_L   0x80

/*
enum {
    DL_NIL_L,
    DL_INT_L,
    DL_STR_L,
    DL_VOID_L
};
*/

typedef struct _dl_key_l    dl_key_l;
typedef struct _dl_value_l  dl_value_l;
typedef struct _dl_node_l   dl_node_l;
typedef struct _dl_table    dl_table;


struct _dl_key_l{
    union{
        dl_str  *s;
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
    dl_pool             *arr_p;
    dl_pool             *node_p;
    
    uchar                lsizenode;  /* log2 of size of `node' array */

    int                  sizearray;
    dl_value_l          *array;  /* array part */
  
    dl_node_l           *node;
    dl_node_l           *lastfree;  /* any free position is before this position */
    
    int                 err;
};


dl_table *dl_table_new(dl_pool *pool,int narray, int nhash);
void dl_table_destroy(dl_table *t);
dl_value_l *dl_find_by_index(dl_table *t, long index);

dl_value_l * dl_try_array_set_int_l(dl_table *t, long index, long value);
int dl_array_set_int_l(dl_table *t, dl_value_l *v, long value);

dl_value_l * dl_try_array_set_str_l(dl_table *t, long index, char *str, size_t len);
int dl_array_set_str_l(dl_table *t, dl_value_l *v, char *str, size_t len);

dl_value_l * dl_try_array_set_void_l(dl_table *t, long index, void *value);
int dl_array_set_void_l(dl_table *t, dl_value_l *v, void *value);

/* node */

dl_value_l * dl_try_node_set_int_l(dl_table *t, char *str, size_t len, long value);
int dl_node_set_int_l(dl_table *t, dl_value_l *v, long value);

dl_value_l * dl_try_node_set_str_l(dl_table *t, char *str, size_t len, char *v_str, size_t v_len);
int dl_node_set_str_l(dl_table *t, dl_value_l *v, char *str, size_t len);

dl_value_l * dl_try_node_set_void_l(dl_table *t, char *str, size_t len, void *value);
int dl_node_set_void_l(dl_table *t, dl_value_l *v, void *value);

dl_value_l *dl_find_by_str(dl_table *t, char *str, size_t len);

#endif