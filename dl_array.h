#ifndef __DL_ARRAY
#define __DL_ARRAY

#include "dl_base.h"

struct _dl_array{
    void        *elts;
    int          nelts;
    size_t       size;
    int          nalloc;
    dl_pool     *pool;
};

dl_array *dl_array_create(dl_pool *p, int n, size_t size);
int dl_array_init(dl_array *array, dl_pool *pool, int n, size_t size);
void dl_array_destroy(dl_array *a);
void *dl_array_push(dl_array *a);
void *dl_array_push_n(dl_array *a, int n);

#endif
