#ifndef __DL_ARRAY
#define __DL_ARRAY

#include "dl_base.h"

typedef struct _dl_array dl_array;

struct _dl_array{
    void        *elts;
    int          nelts;
    size_t       size;
    int          nalloc;
    dl_pool     *pool;
};

void dl_array_destroy(dl_array *a);
void *dl_array_push(dl_array *a);
void *dl_array_push_n(dl_array *a, int n);

#endif
