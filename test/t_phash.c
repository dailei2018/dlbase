#include "dl_base.h"
#include "t_base.h"

typedef struct _person person;
struct _person {
    char *name;
    int age;
};

dl_pool *pool;
dl_hash *h;

void err_msg(const char *format, ...){
    char buf[1024];
    va_list argList;
    va_start(argList, format);

    vsnprintf(buf, 1024, format, argList);

    puts(buf);
    exit(1);
}

int main(){
    pool = dl_create_pool(1024, NULL);

    h = dl_phash_init(pool, 4, H_UNIQUE);
    if(h == NULL) err_msg("dl_phash_init:%s", strerror(errno));

    person p1 = {"Lily", 22};
    person p2 = {"Json", 23};
    person p3 = {"Leo", 24};
    person p4 = {"David", 25};
    person p5 = {"Richa", 28};

    person p6 = {"DL", 28};

    dl_str k1 = {"p1", 2, 1};
    dl_str k2 = {"p2", 2, 1};
    dl_str k3 = {"p3", 2, 1};
    dl_str k4 = {"p4", 2, 1};
    dl_str k5 = {"p5", 2, 1};

    dl_phash_set_void(h, &k1, &p1);
    dl_phash_set_void(h, &k2, &p2);
    dl_phash_set_void(h, &k3, &p3);
    dl_phash_set_void(h, &k4, &p4);
    dl_phash_set_void(h, &k5, &p5);

    //dump_hash_key(h);

    dl_hash_v *v;
    person *p;

    v = dl_hash_get(h, k1.data, k1.len);
    p = (person *)v->v.v;
    dl_printf("key:p1 name:%s age:%d\n", p->name, p->age);

    v = dl_hash_get(h, k5.data, k5.len);
    p = (person *)v->v.v;
    dl_printf("key:p5 name:%s age:%d\n", p->name, p->age);

    /*
     * check unique
     */
    char *key;
    if(dl_phash_set_void(h, &k5, &p6) == DL_BUSY){
        key = dl_pstrdup_nt(pool, &k5);
        dl_printf("\nhash set failed, key:%s already exits\n", key);
    }

    /*
     * change to list
     */
    dl_printf("\nafter changing flag to list");

    h->flag = H_LIST;
    if(dl_phash_set_void(h, &k5, &p6) == DL_OK){
        key = dl_pstrdup_nt(pool, &k5);
        dl_printf("hash set successfully, key:%s\n", key);
    }

    v = dl_hash_get(h, k5.data, k5.len);
    p = (person *)v->v.v;
    dl_printf("key:p5 name:%s age:%d\n", p->name, p->age);

    p = (person *)v->next->v.v;
    dl_printf("key:p5 name:%s age:%d\n", p->name, p->age);
}
