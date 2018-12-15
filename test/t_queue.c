#include "dl_base.h"
#include "t_base.h"

typedef struct _person person;
struct _person {
    dl_queue q;
    char *name;
    int age;
};

void dump_q();
person *new_person(int age, char *name);
int person_cmp(const dl_queue *q1, const dl_queue *q2);

dl_pool *pool;
dl_queue sentinel;


int main(){
    pool = dl_create_pool(1024, NULL);
    dl_queue_init(&sentinel);

    person *p1 = new_person(22, "Lily");
    person *p2 = new_person(25, "Json");
    person *p3 = new_person(12, "Leo");
    person *p4 = new_person(5,  "David");
    person *p5 = new_person(45, "Richa");

    puts("[before sort]");
    dump_q();

    dl_queue_sort(&sentinel, person_cmp);

    puts("[after sort]");
    dump_q();

    return 0;
}

void dump_q(){
    person *p;

    dl_queue *cur = dl_queue_next(&sentinel);

    while(cur != &sentinel){
        /*
         * because dl_queue is the first element of struct,
         * so the following is equivalent
         * dl_queue_data(cur, person, q) == (person *)cur
         */
        p = (person *)cur;
        printf("  name:%s age:%d\n", p->name, p->age);

        cur = dl_queue_next(cur);
    }

    puts("");
}

person *new_person(int age, char *name){
    person *p;

    p = dl_palloc(pool, sizeof(person));
    p->name = dl_palloc(pool, strlen(name)+1);
    strcpy(p->name, name);
    p->age = age;

    // chain it
    dl_queue_insert_head(&sentinel, &p->q);

    return p;
}

int person_cmp(const dl_queue *q1, const dl_queue *q2){
    person *p1, *p2;

    p1 = dl_queue_data(q1, person, q);
    p2 = dl_queue_data(q2, person, q);

    // descending
    return p1->age < p2->age;

    // ascending
    //return p1->age > p2->age;
}
