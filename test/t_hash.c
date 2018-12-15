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

void dump_person_v(dl_hash_v *v){
    person *p;

    while(v){
        p = (person *)v->v.v;
        dl_printf("name:%s age:%d\n", p->name, p->age);

        v = v->next;
    }

    dl_printf("\n");
}

int main(){
    pool = dl_create_pool(1024, NULL);

    dl_printf("mem--------------------\n");

    h = dl_hash_init(pool, 4, 0);
    if(h == NULL) err_msg("dl_hash_init:%s", strerror(errno));

    person p1 = {"Lily", 22};
    person p2 = {"Json", 23};
    person p3 = {"Leo", 24};
    person p4 = {"David", 25};
    person p5 = {"Richa", 28};

    person p6 = {"DL", 28};

    dl_str k1 = {"p1", 2, 1};
    dl_str k2 = {"p2", 2, 1};
    dl_str k3 = {"p3", 2, 1};

    dl_hash_set_void(h, &k1, &p1);
    dl_hash_set_void(h, &k1, &p2);
    dl_hash_set_void(h, &k1, &p3);
    dl_hash_set_void(h, &k2, &p4);
    dl_hash_set_void(h, &k2, &p5);
    dl_hash_set_void(h, &k3, &p5);


    //dump_hash_key(h);

    dl_hash_v *v;
    person *p;

    v = dl_hash_get(h, k1.data, k1.len);
    dump_person_v(v);

    dl_hash_set_void_rep(h, &k1, &p6);
    v = dl_hash_get(h, k1.data, k1.len);
    dump_person_v(v);

    free_hash(h);
}
