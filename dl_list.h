#ifndef __DL_LIST
#define __DL_LIST

#include "dl_base.h"

typedef struct _dl_list_part dl_list_part;

/*
 * linked list
 * every part is an array, list->nalloc = 1 == normal linked list
 */

struct _dl_list_part {
    void            *elts;
    int              nelts;     // the number of elements in this part
    dl_list_part    *next;
};

struct _dl_list{
    dl_list_part    *last;      // current part in use, the current part == the last part
    dl_list_part     part;
    size_t           size;      // element size
    int              nalloc;    // every part can contain nalloc element
    dl_pool         *pool;
};

dl_list *dl_list_create(dl_pool *pool, int n, size_t size);

void *dl_list_push(dl_list *l);

#endif
