#ifndef __DL_POOL
#define __DL_POOL

#include "dl_base.h"

#define DL_ALIGNMENT   sizeof(unsigned long)

#define dl_align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define dl_align_ptr(p, a)                                                   \
    (char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

typedef struct _dl_pool_data dl_pool_data;
typedef struct _dl_pool_large dl_pool_large;

struct _dl_pool_large {
    dl_pool_large       *next;
    void                *alloc;
};

struct _dl_pool_data {
    char            *last;
    char            *end;
    dl_pool         *next;
    int             failed;
};

struct _dl_pool {
    dl_pool_data        d;
    size_t              max;
    dl_pool             *current;   // current using pool
    dl_pool_large       *large;
    dl_log              *log;
};

dl_pool *dl_create_pool(size_t size, dl_log *log);
void *dl_palloc(dl_pool *pool, size_t size);
void dl_destroy_pool(dl_pool *pool);
void dl_reset_pool(dl_pool *pool);
void *dl_alloc(size_t size, dl_log *log);
void *dl_pnalloc(dl_pool *pool, size_t size);
void *dl_pcalloc(dl_pool *pool, size_t size);
void *dl_calloc(size_t size, dl_log *log);

void dl_free(void *buf);

#endif
