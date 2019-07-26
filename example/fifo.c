#include <stdio.h>
#include <pthread.h>

#include <unistd.h>

#include "dl_base.h"
#include "base.h"

dl_fifo     *fifo;

#define DL_FIFO_NUM 100000

void *thread_func(void *arg)
{
    int         i;
    dl_person   p;
    
    i = 0;
    
    while (1) {
        if (dl_fifo_out(fifo, &p, 1) != 1) {
            //sleep(1);
            continue;
        }
        
        printf("age:%d\n", p.age);
        i ++;
        
        if (i == DL_FIFO_NUM)
            break;
    }
}

int main()
{
    int         rc,i;
    pthread_t   t1;
    void        *res;
    
    fifo = dl_fifo_init(100, sizeof(dl_person), NULL);
    if (fifo == NULL) return -1;
    
    rc = pthread_create(&t1, NULL, thread_func, NULL);
    if (rc != 0)
        return -1;
    
    dl_person   p;
    
    i = 0;
    while (1) {
        
        p.age = i++;

again:
        if (dl_fifo_in(fifo, &p, 1) != 1) {
            //sleep(1);
            goto again;
        }
        
        if (i == DL_FIFO_NUM)
            break;
    }
    
    
    pthread_join(t1, &res);
    dl_fifo_destroy(fifo);
    return 0;
}