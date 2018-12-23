#include "dl_base.h"

static inline int dl_array_init(dl_array *array, dl_pool *pool, int n, size_t size);

dl_array *
dl_array_create(dl_pool *p, int n, size_t size)
{
    dl_array *a;

    a = dl_palloc(p, sizeof(dl_array));
    if (a == NULL) {
        return NULL;
    }

    if (dl_array_init(a, p, n, size) != DL_OK) {
        return NULL;
    }

    return a;
}

static inline int
dl_array_init(dl_array *array, dl_pool *pool, int n, size_t size)
{
    array->nelts = 0;
    array->size = size;
    array->nalloc = n;
    array->pool = pool;

    array->elts = dl_palloc(pool, n * size);
    if (array->elts == NULL) {
        return DL_ERROR;
    }

    return DL_OK;
}

//  in two circumstance, recycle memory
void
dl_array_destroy(dl_array *a)
{
    dl_pool  *p;

    p = a->pool;

    if ((char *) a->elts + a->size * a->nalloc == p->d.last) {
        p->d.last -= a->size * a->nalloc;
    }

    if ((char *) a + sizeof(dl_array) == p->d.last) {
        p->d.last = (char *) a;
    }

}

void *
dl_array_push(dl_array *a)
{
    void        *elt, *new;
    size_t       size;
    dl_pool     *p;

    if (a->nelts == a->nalloc) {

        //if the array full

        size = a->size * a->nalloc;
        p = a->pool;

        if ((char *) a->elts + size == p->d.last
            && p->d.last + a->size <= p->d.end){
            /* special case
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            // expand one array element size
            p->d.last += a->size;
            a->nalloc++;

        } else {
            /* allocate a new array and copy old to new address
             * new size is two times of old
             *
             */
            new = dl_palloc(p, 2 * size);
            if (new == NULL) {
                return NULL;
            }

            memcpy(new, a->elts, size);
            a->elts = new;

            a->nalloc *= 2;
        }
    }

    // point to appropriate location
    elt = (char *) a->elts + a->size * a->nelts;

    a->nelts++;

    return elt;
}


void *
dl_array_push_n(dl_array *a, int n)
{
    void        *elt, *new;
    size_t       size;
    int          nalloc;
    dl_pool     *p;

    size = n * a->size;

    if (a->nelts + n > a->nalloc) {

        /* the array is full */

        p = a->pool;

        if ((char *) a->elts + a->size * a->nalloc == p->d.last
            && p->d.last + size <= p->d.end)
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += size;
            a->nalloc += n;

        } else {
            /* allocate a new array */

            nalloc = 2 * ((n >= a->nalloc) ? n : a->nalloc);

            new = dl_palloc(p, nalloc * a->size);
            if (new == NULL) {
                return NULL;
            }

            memcpy(new, a->elts, a->nelts * a->size);
            a->elts = new;
            a->nalloc = nalloc;
        }
    }

    elt = (char *) a->elts + a->size * a->nelts;
    a->nelts += n;

    return elt;
}
