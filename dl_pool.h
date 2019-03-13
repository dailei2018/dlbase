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

typedef struct _dl_pool_cleanup dl_pool_cleanup;
typedef void (*dl_pool_cleanup_pt)(void *data);

struct _dl_pool_cleanup {
    dl_pool_cleanup_pt      handler;        //清理函数
    void                    *data;          //传递给handler，清理用
    dl_pool_cleanup         *next;          //链表指针，所有的清理结构体为一个链表
};

struct _dl_pool {
    dl_pool_data        d;
    size_t              max;
    dl_pool             *current;   // current using pool
    dl_pool_large       *large;
    dl_log              *log;
    
    dl_slab_pool        *slab_pool;
    
    dl_pool_cleanup     *cleanup;
};

dl_pool *dl_create_pool(size_t size, dl_log *log);
dl_pool_cleanup *dl_pool_cleanup_add(dl_pool *p, size_t size);
void *dl_palloc(dl_pool *pool, size_t size);
void dl_destroy_pool(dl_pool *pool);
void dl_reset_pool(dl_pool *pool);
void *dl_alloc(size_t size, dl_log *log);
void *dl_pnalloc(dl_pool *pool, size_t size);
void *dl_pcalloc(dl_pool *pool, size_t size);
void *dl_calloc(size_t size, dl_log *log);

void dl_free(void *buf);

/* slab */
dl_pool *
dl_create_pool_slab(dl_slab_pool *slab_pool, size_t size, dl_log *log);
void * dl_alloc_s(dl_slab_pool *slab_pool, size_t size, dl_log *log);
void dl_free_s(dl_slab_pool *slab_pool, void *buf);

#endif
