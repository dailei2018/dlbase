#include "dl_base.h"

dl_pool     *pool;

dl_table    *t;

typedef struct{
    char *name;
    int  age;
} person;

void void_handler(void *v){
    person  *vv = v;
    
    dl_free(vv);
}

int main(){
    int             s;
    int             *stat;
    person          *p;
    
    //pool = dl_create_pool(1024, NULL);
    
    t = dl_table_new(NULL, 10, 1, void_handler);
    if(t == NULL) return 1;
    
    person *p1 = dl_alloc(sizeof(person), NULL);
    if(p1 == NULL) return 1;
    p1->name = "LILY";
    p1->age = 22;
    
    person *p2 = dl_alloc(sizeof(person), NULL);
    if(p1 == NULL) return 1;
    p2->name = "LEO";
    p2->age = 21;
    
    dl_node_set_void_l(t, "lily", 4, p1);
    dl_node_set_void_l(t, "leo", 3, p2);
    
    dl_value_l *v;
    
    v = dl_find_by_str(t, "lily", 4);
    if(v == NULL) return 1;

    p = v->v.v;
    printf("age:%d name:%s\n", p->age, p->name);
    
    /* nothing happen */
    dl_node_set_void_l(t, "lily", 4, p1);
    dl_node_set_void_l(t, "leo", 3, p2);
    
    person *p3 = dl_alloc(sizeof(person), NULL);
    if(p3 == NULL) return 1;
    p3->name = "DD";
    p3->age = 33;
    
    /* replace,call void_handler */
    dl_node_set_void_l(t, "lily", 4, p3);
    
    dl_table_destroy(t);
    return 0;
}