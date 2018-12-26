#include "dl_base.h"

static inline void *dl_palloc_small(dl_pool *pool, size_t size, int align);
static void *dl_palloc_large(dl_pool *pool, size_t size);
static void *dl_palloc_block(dl_pool *pool, size_t size);

dl_pool *
dl_create_pool(size_t size, dl_log *log)
{
    dl_pool  *p;

    p = dl_alloc(size, log);

    if (p == NULL) {
        dl_log_error(DL_LOG_EMERG, log, "malloc failed size:%lu", size);
        return NULL;
    }

    p->d.last = (char *) p + sizeof(dl_pool);
    p->d.end = (char *) p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(dl_pool);
    p->max = (size < DL_MAX_ALLOC_FROM_POOL) ? size : DL_MAX_ALLOC_FROM_POOL;

    p->current = p;
    p->large = NULL;
    p->log = log;
    p->slab_pool = NULL;

    return p;
}

void *
dl_palloc(dl_pool *pool, size_t size)
{
    if (size <= pool->max) {
        return dl_palloc_small(pool, size, 1);
    }

    return dl_palloc_large(pool, size);
}

void *
dl_pnalloc(dl_pool *pool, size_t size)
{
    if (size <= pool->max) {
        return dl_palloc_small(pool, size, 0);
    }

    return dl_palloc_large(pool, size);
}

void *
dl_pcalloc(dl_pool *pool, size_t size)
{
    void *p;

    p = dl_palloc(pool, size);
    if (p) {
        memset(p, 0, size);
    }

    return p;
}

static void *
dl_palloc_large(dl_pool *pool, size_t size)
{
    void            *p;
    int              n;
    dl_pool_large   *large;

    if(pool->slab_pool)
        p = dl_alloc_s(pool->slab_pool, size, pool->log);
    else
        p = dl_alloc(size, pool->log);
    
    if (p == NULL) {
        return NULL;
    }

    n = 0;

    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }

    large = dl_palloc_small(pool, sizeof(dl_pool_large), 1);
    if (large == NULL) {
        dl_free(p);
        return NULL;
    }

    // chain it to pool->large
    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

static inline void *
dl_palloc_small(dl_pool *pool, size_t size, int align)
{
    char      *m;
    dl_pool  *p;

    p = pool->current;

    do {
        m = p->d.last;

        if (align) {
            m = dl_align_ptr(m, DL_ALIGNMENT);
        }

        if ((size_t) (p->d.end - m) >= size) {
            p->d.last = m + size;

            return m;
        }

        p = p->d.next;

    } while (p);

    return dl_palloc_block(pool, size);
}


static void *
dl_palloc_block(dl_pool *pool, size_t size)
{
    char      *m;
    size_t       psize;
    dl_pool  *p, *new;

    psize = (size_t) (pool->d.end - (char *) pool);

    if(pool->slab_pool)
        m = dl_alloc_s(pool->slab_pool, psize, pool->log);
    else
        m = dl_alloc(psize, pool->log);
    
    if (m == NULL) {
        dl_log_error(DL_LOG_EMERG, pool->log, "malloc failed size:%lu", psize);
        return NULL;
    }

    new = (dl_pool *) m;

    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;

    m += sizeof(dl_pool);
    m = dl_align_ptr(m, DL_ALIGNMENT);
    new->d.last = m + size;

    /*
     * current all pools in pool chain failed plus one
     * if we failed 5 times, change current alloc pool to next
     */
    for (p = pool->current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            pool->current = p->d.next;
        }
    }

    p->d.next = new;

    return m;
}

void
dl_destroy_pool(dl_pool *pool)
{
    dl_pool          *p, *n;
    dl_pool_large    *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            if(pool->slab_pool)
                dl_free_s(pool->slab_pool, l->alloc);
            else
                dl_free(l->alloc);
        }
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        if(pool->slab_pool)
            dl_free_s(pool->slab_pool, p);
        else
            dl_free(p);
        
        if (n == NULL) break;
    }
}

void
dl_reset_pool(dl_pool *pool)
{
    dl_pool        *p;
    dl_pool_large  *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            if(pool->slab_pool)
                dl_free_s(pool->slab_pool, l->alloc);
            else
                dl_free(l->alloc);
        }
    }

    for (p = pool; p; p = p->d.next) {
        p->d.last = (char *)p + sizeof(dl_pool);
        p->d.failed = 0;
    }

    pool->current = pool;
    pool->large = NULL;
}

void *
dl_alloc(size_t size, dl_log *log)
{
    void  *p;

    p = malloc(size);
    if (p == NULL) {
        dl_log_error(DL_LOG_EMERG, log, "malloc failed size:%lu", size);
    }

    //printf("alloc:%p\n", p);

    return p;
}

void *
dl_calloc(size_t size, dl_log *log)
{
    void  *p;

    p = dl_alloc(size, log);

    if (p) {
        memset(p, 0, size);
    }

    return p;
}

void
dl_free(void *buf){
    //printf("free:%p\n", buf);

    free(buf);
}




/* slab */
dl_pool *
dl_create_pool_slab(dl_slab_pool *slab_pool, size_t size, dl_log *log)
{
    dl_pool  *p;

    p = dl_alloc_s(slab_pool, size, log);
    if (p == NULL) {
        dl_log_error(DL_LOG_EMERG, log, "malloc failed size:%lu", size);
        return NULL;
    }

    p->d.last = (char *) p + sizeof(dl_pool);
    p->d.end = (char *) p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(dl_pool);
    p->max = (size < DL_MAX_ALLOC_FROM_POOL) ? size : DL_MAX_ALLOC_FROM_POOL;

    p->current = p;
    p->large = NULL;
    p->log = log;
    p->slab_pool = slab_pool;

    return p;
}


void *
dl_alloc_s(dl_slab_pool *slab_pool, size_t size, dl_log *log)
{
    void  *p;

    p = dl_slab_alloc(slab_pool, size);
    if (p == NULL) {
        dl_log_error(DL_LOG_EMERG, log, "malloc failed size:%lu", size);
    }

    //printf("alloc:%p\n", p);

    return p;
}

void
dl_free_s(dl_slab_pool *slab_pool, void *buf){
    //printf("free:%p\n", buf);
    dl_slab_free(slab_pool, buf);
    
}
