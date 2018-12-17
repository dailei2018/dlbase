#include "dl_base.h"
#include "t_base.h"

typedef struct {
    dl_rbtree_node node;
    long age;
} person;

dl_rbtree *rbt;
dl_rbtree_node *sentinel;
dl_pool *pool;

person *new_p(long age){
    person *p = dl_palloc(pool, sizeof(person));
    p->age = age;
    p->node.key = age;

    return p;
}

int main(){
    pool = dl_create_pool(1024, NULL);

    rbt = dl_palloc(pool, sizeof(dl_rbtree));
    sentinel = dl_palloc(pool, sizeof(dl_rbtree_node));
    dl_rbtree_init(rbt, sentinel, dl_rbtree_insert_value);

    person *p1 = new_p(22);
    person *p2 = new_p(23);
    person *p3 = new_p(24);
    person *p4 = new_p(26);

    person *p5 = new_p(10);

    person *p6 = new_p(25);

    dl_rbtree_insert(rbt, &p1->node);
    dl_rbtree_insert(rbt, &p2->node);
    dl_rbtree_insert(rbt, &p3->node);
    dl_rbtree_insert(rbt, &p4->node);
    dl_rbtree_insert(rbt, &p5->node);

    dl_rbtree_insert(rbt, &p6->node);

    dl_rbtree_node *node = dl_rbtree_min(rbt->root, rbt->sentinel);
    while(node != NULL){
        //printf("%ld color:%s\n", node->key, node->color == 1 ? "red" : "black");

        node = dl_rbtree_next(rbt, node);
    }

}

