#include "dl_base.h"
#include "t_base.h"

dl_pool *pool;

int main(){
    int s;

    pool = dl_create_pool(1024, NULL);
    if(pool == NULL) err_msg("pool create failed");

    dl_shm *shm = dl_shm_create(pool, "/tmp/t_shm", 20480);
    if(shm == NULL) err_msg("shm create failed");

    s = dl_shm_init(shm);
    if(s == -1) err_msg("shm init failed");

    dl_shm_slab_init(shm);

    dl_slab_pool *sp = (dl_slab_pool *)shm->addr;

    char *b1 = dl_slab_alloc(sp, 4096);
    char *b2 = dl_slab_alloc(sp, 5096);

    dl_slab_free(sp, b1);
    dl_slab_free(sp, b2);

}
