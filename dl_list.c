#include "dl_base.h"

static inline int
dl_list_init(dl_list *list, dl_pool *pool, int n, size_t size)
{
    list->part.elts = dl_palloc(pool, n * size);
    if (list->part.elts == NULL) {
        return DL_ERROR;
    }

    list->part.nelts = 0;
    list->part.next = NULL;
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;

    return DL_OK;
}

dl_list *
dl_list_create(dl_pool *pool, int n, size_t size)
{
    dl_list  *list;

    list = dl_palloc(pool, sizeof(dl_list));
    if (list == NULL) {
        return NULL;
    }

    if (dl_list_init(list, pool, n, size) != DL_OK) {
        return NULL;
    }

    return list;
}

void *
dl_list_push(dl_list *l)
{
    void            *elt;
    dl_list_part    *last;

    last = l->last;

    if (last->nelts == l->nalloc) {

        /* the last part is full, allocate a new list part */

        last = dl_palloc(l->pool, sizeof(dl_list_part));
        if (last == NULL) {
            return NULL;
        }

        last->elts = dl_palloc(l->pool, l->nalloc * l->size);
        if (last->elts == NULL) {
            return NULL;
        }

        last->nelts = 0;
        last->next = NULL;

        // chain it
        l->last->next = last;

        // point to the new part
        l->last = last;
    }

    elt = (char *) last->elts + l->size * last->nelts;
    last->nelts++;

    return elt;
}
