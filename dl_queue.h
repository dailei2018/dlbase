#ifndef __DL_QUEUE
#define __DL_QUEUE

#include "dl_base.h"

typedef struct _dl_queue  dl_queue;

struct _dl_queue {
    dl_queue  *prev;
    dl_queue  *next;
};


#define dl_queue_init(q)           \
    (q)->prev = q;                  \
    (q)->next = q

#define dl_queue_empty(h)          \
    (h == (h)->prev)

#define dl_queue_insert_head(h, x)         \
    (x)->next = (h)->next;                  \
    (x)->next->prev = x;                    \
    (x)->prev = h;                          \
    (h)->next = x

#define dl_queue_insert_after   dl_queue_insert_head

#define dl_queue_insert_tail(h, x)         \
    (x)->prev = (h)->prev;                  \
    (x)->prev->next = x;                    \
    (x)->next = h;                          \
    (h)->prev = x

#define dl_queue_head(h)                   \
    (h)->next

#define dl_queue_last(h)                   \
    (h)->prev

#define dl_queue_sentinel(h)               \
    (h)

#define dl_queue_next(q)                   \
    (q)->next

#define dl_queue_prev(q)                   \
    (q)->prev


#define dl_queue_remove(x)                 \
    (x)->next->prev = (x)->prev;            \
    (x)->prev->next = (x)->next

// split h by q, h queue do not include q, q is the first queue of q&n
#define dl_queue_split(h, q, n)            \
    (n)->prev = (h)->prev;                  \
    (n)->prev->next = n;                    \
    (n)->next = q;                          \
    (h)->prev = (q)->prev;                  \
    (h)->prev->next = h;                    \
    (q)->prev = n;
// add n queue to h, do not include n
#define dl_queue_add(h, n)                 \
    (h)->prev->next = (n)->next;            \
    (n)->next->prev = (h)->prev;            \
    (h)->prev = (n)->prev;                  \
    (h)->prev->next = h;


void
dl_queue_sort(dl_queue *queue, int (*cmp)(const dl_queue *, const dl_queue *));

#endif
