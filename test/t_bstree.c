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

void dump_tree(){
    if(bst->root == NULL){
        dl_printf("nothing..\n");
        return;
    }

    dl_bstree_node *node = dl_bstree_min(bst->root);

    while(node != NULL){

        dl_printf("%-5ld ", node->key);

        if(node->parent != NULL){
            dl_printf("p:%-5ld ", node->parent->key);
        }else{
            dl_printf("root    ");
        }

        if(node->left != NULL){
            dl_printf("left:%ld ", node->left->key);
        }

        if(node->right != NULL){
            dl_printf("right:%ld ", node->right->key);
        }

        dl_printf("\n");

        node = dl_bstree_next(bst, node);
    }
    dl_printf("\n\n");
}

int main(){
    pool = dl_create_pool(1024, NULL);

    bst = dl_pcalloc(pool, sizeof(dl_bstree));

    dl_bstree_init(bst, dl_bstree_insert_value);


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

    dump_tree();

    dl_bstree_delete_by_node(bst, &p6->node);
    dump_tree();

    dl_bstree_delete_by_node(bst, &p3->node);
    dump_tree();

    dl_bstree_delete_by_node(bst, &p1->node);
    dump_tree();

    dl_bstree_delete_by_node(bst, &p5->node);
    dump_tree();

    dl_bstree_delete_by_node(bst, &p4->node);
    dump_tree();

    dl_bstree_delete_by_node(bst, &p2->node);
    dump_tree();
}


