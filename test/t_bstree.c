#include "dl_base.h"
#include "t_base.h"

typedef struct {
    char *name;
    int age;
    dl_bstree_node node;
} person;

dl_bstree *bst;
dl_pool *pool;

person *new_p(char *name, int age){
    person *p = dl_palloc(pool, sizeof(person));
    p->name = name;
    p->age = age;
    p->node.key = age;

    return p;
}

int main(){
    pool = dl_create_pool(1024, NULL);

    bst = dl_pcalloc(pool, sizeof(dl_bstree));

    person *p1 = new_p("Lily", 10);
    person *p2 = new_p("Leo", 20);
    person *p3 = new_p("David", 30);

    person *p4 = new_p("David", 25);
    person *p5 = new_p("David", 24);

    person *p6 = new_p("David", 40);

    dl_bstree_insert(bst, &p1->node);
    dl_bstree_insert(bst, &p2->node);
    dl_bstree_insert(bst, &p3->node);
    dl_bstree_insert(bst, &p4->node);
    dl_bstree_insert(bst, &p5->node);
    dl_bstree_insert(bst, &p6->node);

    dl_bstree_dump(bst->root);

    person *p_lily = dl_offset_data(bst->leftmost, person, node);
    printf("name:%s age:%d\n", p_lily->name, p_lily->age);
}


