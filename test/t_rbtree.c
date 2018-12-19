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

void dump_tree(){
    if(rbt->root == rbt->sentinel){
        dl_printf("nothing..\n");
        return;
    }

    dl_rbtree_node *node = dl_rbtree_min(rbt->root, rbt->sentinel);

    while(node != NULL){
        dl_printf("%ld color:%-5s   ", node->key, node->color == 1 ? "red" : "black");

        if(node->parent != NULL){
            dl_printf("p:%-5ld ", node->parent->key);
        }else{
            dl_printf("root    ");
        }

        if(node->left != sentinel){
            dl_printf("left:%ld ", node->left->key);
        }

        if(node->right != sentinel){
            dl_printf("right:%ld ", node->right->key);
        }

        dl_printf("\n");

        node = dl_rbtree_next(rbt, node);
    }
    dl_printf("\n\n");
}

int main(){
    pool = dl_create_pool(1024, NULL);

    rbt = dl_palloc(pool, sizeof(dl_rbtree));
    sentinel = dl_palloc(pool, sizeof(dl_rbtree_node));
    dl_rbtree_init(rbt, sentinel, dl_rbtree_insert_value);

    person *p1 = new_p(20);
    person *p2 = new_p(30);
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

    dump_tree(0);

    dl_printf("[after delete %ld]\n", p2->node.key);
    dl_rbtree_delete(rbt, &p2->node);
    dump_tree();

    dl_printf("[after delete %ld]\n", p3->node.key);
    dl_rbtree_delete(rbt, &p3->node);
    dump_tree();

    dl_printf("[after delete %ld]\n", p6->node.key);
    dl_rbtree_delete(rbt, &p6->node);
    dump_tree();

    dl_printf("[after delete %ld]\n", p4->node.key);
    dl_rbtree_delete(rbt, &p4->node);
    dump_tree();

    dl_printf("[after delete %ld]\n", p1->node.key);
    dl_rbtree_delete(rbt, &p1->node);
    dump_tree();


    dl_printf("[after delete %ld]\n", p5->node.key);
    dl_rbtree_delete(rbt, &p5->node);
    dump_tree();
}

