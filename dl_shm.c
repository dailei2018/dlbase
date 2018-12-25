#include "dl_base.h"

static dl_slab_page *
dl_slab_alloc_pages(dl_slab_pool *pool, uint pages);
static void
dl_slab_free_pages(dl_slab_pool *pool, dl_slab_page *page, uint pages);
static void dl_slab_init(dl_slab_pool *pool);

dl_shm *dl_shm_create(dl_pool *p, char *fname, size_t size){
    dl_shm *shm;

    shm = dl_palloc(p, sizeof(dl_shm));
    if(shm == NULL) return NULL;

    shm->fname = dl_pstrdup_nt(p, fname, strlen(fname));
    if(shm->fname == NULL) return NULL;

    shm->size = size;
    shm->log = p->log;
    shm->addr = NULL;

    return shm;
}

int dl_shm_init(dl_shm *shm){
    int s;

    shm->fd = open(shm->fname, O_RDWR | O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if(shm->fd == -1) return -1;

    struct stat sb;
    s = fstat(shm->fd, &sb);
    if(s == -1) return -1;

    if(sb.st_size == 0){
        s = ftruncate(shm->fd, shm->size);
        if(s == -1){
            return -1;
        }
    }

    shm->addr = mmap(NULL, shm->size, PROT_READ | PROT_WRITE, MAP_SHARED, shm->fd, 0);

    if(shm->addr == MAP_FAILED){
        return -1;
    }

    return 0;
}

dl_slab_pool * dl_shm_slab_init(dl_shm *shm){
    dl_slab_pool *sp = (dl_slab_pool *)shm->addr;

    sp->end = shm->addr + shm->size;
    sp->min_shift = 3;
    sp->addr = shm->addr;

    dl_slab_init(sp);
    
    return sp;
}

/*
 * slab
 */

static uint  dl_slab_max_size;
static uint  dl_slab_exact_size;
static uint  dl_slab_exact_shift;

static uint  dl_pagesize;
static uint  dl_pagesize_shift;
static uint  dl_cacheline_size;

static void
dl_slab_init(dl_slab_pool *pool)
{
    char          *p;
    size_t           size;
    int              m;
    uintptr_t             i, n, pages;
    dl_slab_page    *slots, *page;

    dl_pagesize = getpagesize();
    for (n = dl_pagesize; n >>= 1; dl_pagesize_shift++){

    }
    dl_cacheline_size = DL_CUP_CACHE_LINE;

    /* STUB */
    if (dl_slab_max_size == 0) {
        //2048
        dl_slab_max_size = dl_pagesize / 2;
        //64 = 2^6
        dl_slab_exact_size = dl_pagesize / (8 * sizeof(uintptr_t));
        //dl_slab_exact_shift=6
        for (n = dl_slab_exact_size; n >>= 1; dl_slab_exact_shift++) {
            /* void */
        }
    }

    //1 << 3 bits = 2^3 = 8
    pool->min_size = (size_t) 1 << pool->min_shift;

    slots = dl_slab_slots(pool);

    p = (char *) slots;
    size = pool->end - p;

    dl_slab_junk(p, size);

    //9 = 12-3
    n = dl_pagesize_shift - pool->min_shift;

    //即8,16,32,64,128,256,512,1024,2048 9个分级
    for (i = 0; i < n; i++) {
        slots[i].slab = 0;
        slots[i].next = &slots[i];
        slots[i].prev = 0;
    }

    //跳过9个slot分级数组区域
    p += n * sizeof(dl_slab_page);

    pool->stats = (dl_slab_stat *) p;
    memset(pool->stats, 0, n * sizeof(dl_slab_stat));

    //跳过9个dl_slab_stat分级数组区域
    p += n * sizeof(dl_slab_stat);

    //计算出剩余的内存大小
    size -= n * (sizeof(dl_slab_page) + sizeof(dl_slab_stat));

    //由于每一个page均对应一个dl_slab_page的管理结构，因此下面是计算剩余空间还可分配出多少页
    pages = (uint) (size / (dl_pagesize + sizeof(dl_slab_page)));

    pool->pages = (dl_slab_page *) p;
    memset(pool->pages, 0, pages * sizeof(dl_slab_page));

    page = pool->pages;

    /* only "next" is used in list head */
    pool->free.slab = 0;
    pool->free.next = page;
    pool->free.prev = 0;

    //记录页数
    page->slab = pages;
    //下一页既指向空闲页
    page->next = &pool->free;
    page->prev = (uintptr_t) &pool->free;

    //4K对齐
    pool->start = dl_align_ptr(p + pages * sizeof(dl_slab_page),
                                dl_pagesize);

    //这个地方是进行对齐后的page调整，这个地方是我前面疑问的部分解决位置。
    m = pages - (pool->end - pool->start) / dl_pagesize;
    if (m > 0) {
        pages -= m;
        page->slab = pages;
    }

    //页数组最后
    pool->last = pool->pages + pages;
    //空闲页数
    pool->pfree = pages;

    pool->log_nomem = 1;
    pool->log_ctx = &pool->zero;
    pool->zero = '\0';
}

void *
dl_slab_alloc(dl_slab_pool *pool, size_t size)
{
    void  *p;

    p = dl_slab_alloc_locked(pool, size);

    return p;
}

void *
dl_slab_alloc_locked(dl_slab_pool *pool, size_t size)
{
    size_t            s;
    uintptr_t         p, n, m, mask, *bitmap;
    uintptr_t         i, slot, shift, map;
    dl_slab_page  *page, *prev, *slots;

    if (size > dl_slab_max_size) {

        page = dl_slab_alloc_pages(pool, (size >> dl_pagesize_shift)
                                          + ((size % dl_pagesize) ? 1 : 0));
        if (page) {
            p = dl_slab_page_addr(pool, page);

        } else {
            p = 0;
        }

        goto done;
    }

    if (size > pool->min_size) {
        shift = 1;
        for (s = size - 1; s >>= 1; shift++) { /* void */ }
        slot = shift - pool->min_shift;

    } else {
        shift = pool->min_shift;
        slot = 0;
    }

    pool->stats[slot].reqs++;

    slots = dl_slab_slots(pool);
    page = slots[slot].next;

    if (page->next != page) {

        if (shift < dl_slab_exact_shift) {
            bitmap = (uintptr_t *) dl_slab_page_addr(pool, page);

            map = (dl_pagesize >> shift) / (sizeof(uintptr_t) * 8);

            for (n = 0; n < map; n++) {

                if (bitmap[n] != DL_SLAB_BUSY) {

                    for (m = 1, i = 0; m; m <<= 1, i++) {
                        if (bitmap[n] & m) {
                            continue;
                        }

                        bitmap[n] |= m;

                        i = (n * sizeof(uintptr_t) * 8 + i) << shift;

                        p = (uintptr_t) bitmap + i;

                        pool->stats[slot].used++;

                        if (bitmap[n] == DL_SLAB_BUSY) {
                            for (n = n + 1; n < map; n++) {
                                if (bitmap[n] != DL_SLAB_BUSY) {
                                    goto done;
                                }
                            }

                            prev = dl_slab_page_prev(page);
                            prev->next = page->next;
                            page->next->prev = page->prev;

                            page->next = NULL;
                            page->prev = DL_SLAB_SMALL;
                        }

                        goto done;
                    }
                }
            }

        } else if (shift == dl_slab_exact_shift) {

            for (m = 1, i = 0; m; m <<= 1, i++) {
                if (page->slab & m) {
                    continue;
                }

                page->slab |= m;

                if (page->slab == DL_SLAB_BUSY) {
                    prev = dl_slab_page_prev(page);
                    prev->next = page->next;
                    page->next->prev = page->prev;

                    page->next = NULL;
                    page->prev = DL_SLAB_EXACT;
                }

                p = dl_slab_page_addr(pool, page) + (i << shift);

                pool->stats[slot].used++;

                goto done;
            }

        } else { /* shift > dl_slab_exact_shift */

            mask = ((uintptr_t) 1 << (dl_pagesize >> shift)) - 1;
            mask <<= DL_SLAB_MAP_SHIFT;

            for (m = (uintptr_t) 1 << DL_SLAB_MAP_SHIFT, i = 0;
                 m & mask;
                 m <<= 1, i++)
            {
                if (page->slab & m) {
                    continue;
                }

                page->slab |= m;

                if ((page->slab & DL_SLAB_MAP_MASK) == mask) {
                    prev = dl_slab_page_prev(page);
                    prev->next = page->next;
                    page->next->prev = page->prev;

                    page->next = NULL;
                    page->prev = DL_SLAB_BIG;
                }

                p = dl_slab_page_addr(pool, page) + (i << shift);

                pool->stats[slot].used++;

                goto done;
            }
        }

        //"dl_slab_alloc(): page is busy"
        puts("dl_slab_alloc(): page is busy");
    }

    page = dl_slab_alloc_pages(pool, 1);

    if (page) {
        if (shift < dl_slab_exact_shift) {
            /*
             * here, let the requested size are 8 bytes, 4096/8 = 512,
             * one page can contain 512 blocks.
             * so we need 512 bits to record memory block(8 bytes) usage.
             * 64-bit machine one pointer size is 8 bytes,64 bits.
             * 512/64 = 8, so we need 8 pointers size
             */
            bitmap = (uintptr_t *) dl_slab_page_addr(pool, page);

            n = (dl_pagesize >> shift) / ((1 << shift) * 8);

            if (n == 0) {
                n = 1;
            }

            /* "n" elements for bitmap, plus one requested */
            bitmap[0] = ((uintptr_t) 2 << n) - 1;

            map = (dl_pagesize >> shift) / (sizeof(uintptr_t) * 8);

            for (i = 1; i < map; i++) {
                bitmap[i] = 0;
            }

            page->slab = shift;
            page->next = &slots[slot];
            page->prev = (uintptr_t) &slots[slot] | DL_SLAB_SMALL;

            slots[slot].next = page;

            pool->stats[slot].total += (dl_pagesize >> shift) - n;

            p = dl_slab_page_addr(pool, page) + (n << shift);

            pool->stats[slot].used++;

            goto done;

        } else if (shift == dl_slab_exact_shift) {

            page->slab = 1;
            page->next = &slots[slot];
            page->prev = (uintptr_t) &slots[slot] | DL_SLAB_EXACT;

            slots[slot].next = page;

            pool->stats[slot].total += sizeof(uintptr_t) * 8;

            p = dl_slab_page_addr(pool, page);

            pool->stats[slot].used++;

            goto done;

        } else { /* shift > dl_slab_exact_shift */

            page->slab = ((uintptr_t) 1 << DL_SLAB_MAP_SHIFT) | shift;
            page->next = &slots[slot];
            page->prev = (uintptr_t) &slots[slot] | DL_SLAB_BIG;

            slots[slot].next = page;

            pool->stats[slot].total += dl_pagesize >> shift;

            p = dl_slab_page_addr(pool, page);

            pool->stats[slot].used++;

            goto done;
        }
    }

    p = 0;

    pool->stats[slot].fails++;

done:

    return (void *) p;
}


static dl_slab_page *
dl_slab_alloc_pages(dl_slab_pool *pool, uint pages)
{
    dl_slab_page  *page, *p;

    for (page = pool->free.next; page != &pool->free; page = page->next) {

        // enough free contiguous pages
        if (page->slab >= pages) {

            // free contiguous pages larger than requested pages
            if (page->slab > pages) {

                /*
                 * the last free page(prev) point to the first free page
                 */
                page[page->slab - 1].prev = (uintptr_t) &page[pages];

                /*
                 * first free page
                 */
                page[pages].slab = page->slab - pages;
                page[pages].next = page->next;
                page[pages].prev = page->prev;

                /*
                 * isolate the current request pages
                 */
                p = (dl_slab_page *) page->prev;
                p->next = &page[pages];
                page->next->prev = (uintptr_t) &page[pages];

            } else {
                /*
                 * free contiguous pages equals requested pages
                 */
                p = (dl_slab_page *) page->prev;
                p->next = page->next;
                page->next->prev = page->prev;

            }

            /*
             * set page properties
             */
            page->slab = pages | DL_SLAB_PAGE_START;
            page->next = NULL;
            page->prev = DL_SLAB_PAGE;

            pool->pfree -= pages;

            /* only request one page, just return */
            if (--pages == 0) {
                return page;
            }

            /* request one more pages, set all these pages except the first one. */
            for (p = page + 1; pages; pages--) {
                p->slab = DL_SLAB_PAGE_BUSY;
                p->next = NULL;
                p->prev = DL_SLAB_PAGE;
                p++;
            }

            return page;
        }
    }

    if (pool->log_nomem) {
        puts("dl_slab_alloc() failed: no memory");
        //"dl_slab_alloc() failed: no memory"
        //dl_log_error(DL_LOG_EMERG, pool->log, "dl_slab_alloc() failed: no memory");
    }

    return NULL;
}

void
dl_slab_free(dl_slab_pool *pool, void *p)
{

    dl_slab_free_locked(pool, p);

}

void
dl_slab_free_locked(dl_slab_pool *pool, void *p)
{
    size_t            size;
    uintptr_t         slab, m, *bitmap;
    uintptr_t           i, n, type, slot, shift, map;
    dl_slab_page  *slots, *page;

    if ((char *) p < pool->start || (char *) p > pool->end) {
        //dl_slab_error(pool, DL_LOG_ALERT, "dl_slab_free(): outside of pool");
        goto fail;
    }

    n = ((char *) p - pool->start) >> dl_pagesize_shift;
    page = &pool->pages[n];
    slab = page->slab;
    type = dl_slab_page_type(page);

    switch (type) {

    case DL_SLAB_SMALL:

        shift = slab & DL_SLAB_SHIFT_MASK;
        size = (size_t) 1 << shift;

        if ((uintptr_t) p & (size - 1)) {
            goto wrong_chunk;
        }

        n = ((uintptr_t) p & (dl_pagesize - 1)) >> shift;
        m = (uintptr_t) 1 << (n % (sizeof(uintptr_t) * 8));
        n /= sizeof(uintptr_t) * 8;
        bitmap = (uintptr_t *)
                             ((uintptr_t) p & ~((uintptr_t) dl_pagesize - 1));

        if (bitmap[n] & m) {
            slot = shift - pool->min_shift;

            if (page->next == NULL) {
                slots = dl_slab_slots(pool);

                page->next = slots[slot].next;
                slots[slot].next = page;

                page->prev = (uintptr_t) &slots[slot] | DL_SLAB_SMALL;
                page->next->prev = (uintptr_t) page | DL_SLAB_SMALL;
            }

            bitmap[n] &= ~m;

            n = (dl_pagesize >> shift) / ((1 << shift) * 8);

            if (n == 0) {
                n = 1;
            }

            if (bitmap[0] & ~(((uintptr_t) 1 << n) - 1)) {
                goto done;
            }

            map = (dl_pagesize >> shift) / (sizeof(uintptr_t) * 8);

            for (i = 1; i < map; i++) {
                if (bitmap[i]) {
                    goto done;
                }
            }

            dl_slab_free_pages(pool, page, 1);

            pool->stats[slot].total -= (dl_pagesize >> shift) - n;

            goto done;
        }

        goto chunk_already_free;

    case DL_SLAB_EXACT:

        m = (uintptr_t) 1 <<
                (((uintptr_t) p & (dl_pagesize - 1)) >> dl_slab_exact_shift);
        size = dl_slab_exact_size;

        if ((uintptr_t) p & (size - 1)) {
            goto wrong_chunk;
        }

        if (slab & m) {
            slot = dl_slab_exact_shift - pool->min_shift;

            /*first free 64 bytes from a full page  */
            if (slab == DL_SLAB_BUSY) {
                slots = dl_slab_slots(pool);

                page->next = slots[slot].next;
                slots[slot].next = page;

                page->prev = (uintptr_t) &slots[slot] | DL_SLAB_EXACT;
                page->next->prev = (uintptr_t) page | DL_SLAB_EXACT;
            }

            /* change one bit 1 to 0 */
            page->slab &= ~m;

            /* page still in use */
            if (page->slab) {
                goto done;
            }

            /* all block freed, so we should free the whole page */
            dl_slab_free_pages(pool, page, 1);

            pool->stats[slot].total -= sizeof(uintptr_t) * 8;

            goto done;
        }

        goto chunk_already_free;

    case DL_SLAB_BIG:

        shift = slab & DL_SLAB_SHIFT_MASK;
        size = (size_t) 1 << shift;

        if ((uintptr_t) p & (size - 1)) {
            goto wrong_chunk;
        }

        m = (uintptr_t) 1 << ((((uintptr_t) p & (dl_pagesize - 1)) >> shift)
                              + DL_SLAB_MAP_SHIFT);

        if (slab & m) {
            slot = shift - pool->min_shift;

            if (page->next == NULL) {
                slots = dl_slab_slots(pool);

                page->next = slots[slot].next;
                slots[slot].next = page;

                page->prev = (uintptr_t) &slots[slot] | DL_SLAB_BIG;
                page->next->prev = (uintptr_t) page | DL_SLAB_BIG;
            }

            page->slab &= ~m;

            if (page->slab & DL_SLAB_MAP_MASK) {
                goto done;
            }

            dl_slab_free_pages(pool, page, 1);

            pool->stats[slot].total -= dl_pagesize >> shift;

            goto done;
        }

        goto chunk_already_free;

    case DL_SLAB_PAGE:

        /* 4K alignment */
        if ((uintptr_t) p & (dl_pagesize - 1)) {
            goto wrong_chunk;
        }

        /* the first page */
        if (!(slab & DL_SLAB_PAGE_START)) {
            //dl_slab_error(pool, DL_LOG_ALERT,"dl_slab_free(): page is already free");
            goto fail;
        }

        /* in use but not the first page */
        if (slab == DL_SLAB_PAGE_BUSY) {
            //dl_slab_error(pool, DL_LOG_ALERT, "dl_slab_free(): pointer to wrong page");
            goto fail;
        }

        n = ((char *) p - pool->start) >> dl_pagesize_shift;

        /* get the number of pages */
        size = slab & ~DL_SLAB_PAGE_START;

        dl_slab_free_pages(pool, &pool->pages[n], size);

        dl_slab_junk(p, size << dl_pagesize_shift);

        return;
    }

    /* not reached */

    return;

done:

    pool->stats[slot].used--;

    dl_slab_junk(p, size);

    return;

wrong_chunk:
    puts("dl_slab_free(): pointer to wrong chunk");
    //dl_slab_error(pool, DL_LOG_ALERT,"dl_slab_free(): pointer to wrong chunk");

    goto fail;

chunk_already_free:
    puts("dl_slab_free(): chunk is already free");
    //dl_slab_error(pool, DL_LOG_ALERT,"dl_slab_free(): chunk is already free");

fail:

    return;
}


static void
dl_slab_free_pages(dl_slab_pool *pool, dl_slab_page *page, uint pages)
{
    dl_slab_page  *prev, *join;

    pool->pfree += pages;

    page->slab = pages--;   //记录连续的空闲页数

    if (pages) {
        memset(&page[1], 0, pages * sizeof(dl_slab_page));
    }

    if (page->next) {
        prev = dl_slab_page_prev(page);
        prev->next = page->next;
        page->next->prev = page->prev;
    }

    join = page + page->slab;

    if (join < pool->last) {

        if (dl_slab_page_type(join) == DL_SLAB_PAGE) {

            if (join->next != NULL) {
                pages += join->slab;
                page->slab += join->slab;

                prev = dl_slab_page_prev(join);
                prev->next = join->next;
                join->next->prev = join->prev;

                join->slab = DL_SLAB_PAGE_FREE;
                join->next = NULL;
                join->prev = DL_SLAB_PAGE;
            }
        }
    }

    if (page > pool->pages) {
        join = page - 1;

        if (dl_slab_page_type(join) == DL_SLAB_PAGE) {

            if (join->slab == DL_SLAB_PAGE_FREE) {
                join = dl_slab_page_prev(join);
            }

            if (join->next != NULL) {
                pages += join->slab;
                join->slab += page->slab;

                prev = dl_slab_page_prev(join);
                prev->next = join->next;
                join->next->prev = join->prev;

                page->slab = DL_SLAB_PAGE_FREE;
                page->next = NULL;
                page->prev = DL_SLAB_PAGE;

                page = join;
            }
        }
    }

    if (pages) {
        page[pages].prev = (uintptr_t) page;
    }

    // chain it to the head of free
    page->prev = (uintptr_t) &pool->free;
    page->next = pool->free.next;

    page->next->prev = (uintptr_t) page;

    pool->free.next = page;
}
