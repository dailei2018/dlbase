#include <stdio.h>
#include <pthread.h>

#include <unistd.h>

#include "dl_base.h"
#include "base.h"

dl_fifo     *fifo;

#define DL_FIFO_NUM 100000

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static int out_sum = 0;

void *thread_func(void *arg)
{
    int         rc;
    dl_person   p;
    
    while (1) {
        
        pthread_mutex_lock(&mtx);
        
        if (out_sum == DL_FIFO_NUM) {
            pthread_mutex_unlock(&mtx);
            break;
        }

again:
        if (dl_fifo_out(fifo, &p, 1) != 1) {
            //sleep(1);
            goto again;
        }
        out_sum++;
        
        pthread_mutex_unlock(&mtx);
        
        printf("age1:%d\n", p.age);

    }
}

void *thread_func2(void *arg)
{
    int         rc;
    dl_person   p;
    
    while (1) {
        
        pthread_mutex_lock(&mtx);
        
        if (out_sum == DL_FIFO_NUM) {
            pthread_mutex_unlock(&mtx);
            break;
        }

again:
        if (dl_fifo_out(fifo, &p, 1) != 1) {
            //sleep(1);
            goto again;
        }
        out_sum++;
        pthread_mutex_unlock(&mtx);
        
        printf("age2:%d\n", p.age);
    }
}

int main()
{
    int         rc,i;
    pthread_t   t1, t2;
    void        *res;
    
    fifo = dl_fifo_init(100, sizeof(dl_person), NULL);
    if (fifo == NULL) return -1;
    
    rc = pthread_create(&t1, NULL, thread_func, NULL);
    if (rc != 0)
        return -1;
    
    rc = pthread_create(&t2, NULL, thread_func2, NULL);
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
    pthread_join(t2, &res);
    dl_fifo_destroy(fifo);
    return 0;
}