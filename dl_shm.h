#ifndef __DL_SHM
#define __DL_SHM

#include "dl_base.h"


typedef struct {
    u_char      *addr;
    size_t       size;

    char        *fname;
    int          fd;
    dl_log      *log;

} dl_shm;


dl_shm *dl_shm_create(dl_pool *p, char *fname, size_t size);
int dl_shm_init(dl_shm *shm);


/*
 * slab
 */

#define DL_SLAB_PAGE_MASK   3
#define DL_SLAB_PAGE        0
#define DL_SLAB_BIG         1
#define DL_SLAB_EXACT       2
#define DL_SLAB_SMALL       3


#define DL_SLAB_PAGE_FREE   0
#define DL_SLAB_PAGE_BUSY   0xffffffffffffffff
#define DL_SLAB_PAGE_START  0x8000000000000000

#define DL_SLAB_SHIFT_MASK  0x000000000000000f
#define DL_SLAB_MAP_MASK    0xffffffff00000000
#define DL_SLAB_MAP_SHIFT   32

#define DL_SLAB_BUSY        0xffffffffffffffff


#define dl_slab_slots(pool)                                                  \
    (dl_slab_page *) ((u_char *) (pool) + sizeof(dl_slab_pool))

#define dl_slab_page_type(page)   ((page)->prev & DL_SLAB_PAGE_MASK)

#define dl_slab_page_prev(page)                                              \
    (dl_slab_page *) ((page)->prev & ~DL_SLAB_PAGE_MASK)

#define dl_slab_page_addr(pool, page)                                        \
    ((((page) - (pool)->pages) << dl_pagesize_shift)                         \
     + (uintptr_t) (pool)->start)

#define dl_slab_junk(p, size)


typedef struct _dl_slab_page  dl_slab_page;

struct _dl_slab_page {
    /*
     * 未分配的第一个空闲页 - 连续空闲页数(包括自己)
     * 已分配连续页中的第一个空闲页 - 连续页数 | 页状态
     * 已分配连续页中的非第一个空闲页 - 页状态
     */
    uintptr_t         slab;     // 记录连续的空闲页数
    dl_slab_page  *next;

    /*
     * 已分配的页 - 后两位为页类型
     */
    uintptr_t         prev;
};


typedef struct {
    uint        total;
    uint        used;

    uint        reqs;   // the number of requests
    uint        fails;
} dl_slab_stat;


typedef struct {
    volatile ulong      lock;

    size_t              min_size;
    size_t              min_shift;

    dl_slab_page    *pages;
    dl_slab_page    *last;
    dl_slab_page     free;

    dl_slab_stat    *stats;
    uint             pfree;

    char           *start;
    char           *end;

    char           *log_ctx;
    char            zero;

    unsigned          log_nomem:1;

    void             *data;
    void             *addr;
} dl_slab_pool;


void dl_shm_slab_init(dl_shm *shm);
void *
dl_slab_alloc(dl_slab_pool *pool, size_t size);
void *
dl_slab_alloc_locked(dl_slab_pool *pool, size_t size);
void
dl_slab_free_locked(dl_slab_pool *pool, void *p);
void
dl_slab_free(dl_slab_pool *pool, void *p);






#endif
