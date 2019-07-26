#include "dl_base.h"

/*
 * if the number of queue element is odd, return middle element.
 * if even, return the n/2 element
 */

dl_queue *
dl_queue_middle(dl_queue *queue)
{
    dl_queue  *middle, *next;

    middle = dl_queue_head(queue);

    if (middle == dl_queue_last(queue)) {
        return middle;
    }

    next = dl_queue_head(queue);

    /* algorithm
     * every single loop, 'middle' move once, `next` move twice.
     */
    for ( ;; ) {
        middle = dl_queue_next(middle);

        next = dl_queue_next(next);

        if (next == dl_queue_last(queue)) {
            return middle;
        }

        next = dl_queue_next(next);

        if (next == dl_queue_last(queue)) {
            return middle;
        }
    }
}


/* the stable insertion sort */
void
dl_queue_sort(dl_queue *queue, int (*cmp)(const dl_queue *, const dl_queue *))
{
    dl_queue  *q, *prev, *next;

    q = dl_queue_head(queue);

    if (q == dl_queue_last(queue)) {
        return;
    }

    for (q = dl_queue_next(q); q != dl_queue_sentinel(queue); q = next) {

        prev = dl_queue_prev(q);
        next = dl_queue_next(q);

        dl_queue_remove(q);

        do {
            if (cmp(prev, q) <= 0) {
                break;
            }

            prev = dl_queue_prev(prev);

        } while (prev != dl_queue_sentinel(queue));

        dl_queue_insert_after(prev, q);
    }
}
