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

    

    person *p1 = new_p("Lily", 74244);
    person *p2 = new_p("Leo", 100367);
    person *p3 = new_p("David", 89170);
    person *p4 = new_p("David", 83916);
    person *p5 = new_p("David", 77088);
    person *p6 = new_p("David", 17535);
    
    person *p7 = new_p("Lily", 96908);
    person *p8 = new_p("Leo", 37415);
    person *p9 = new_p("David", 44397);
    person *p10 = new_p("David", 33469);
    person *p11 = new_p("David", 102798);
    person *p12 = new_p("David", 92781);
    
    person *p13 = new_p("Lily", 16033);
    person *p14 = new_p("Leo", 94852);
    person *p15 = new_p("David", 73988);

    dl_bstree_node *node;

    dl_bstree_insert(bst, &p1->node);
    dl_bstree_insert(bst, &p2->node);
    dl_bstree_insert(bst, &p3->node);
    dl_bstree_insert(bst, &p4->node);
    dl_bstree_insert(bst, &p5->node);
    dl_bstree_insert(bst, &p6->node);
    dl_bstree_insert(bst, &p7->node);
    dl_bstree_insert(bst, &p8->node);
    dl_bstree_insert(bst, &p9->node);
    dl_bstree_insert(bst, &p10->node);
    dl_bstree_insert(bst, &p11->node);
    dl_bstree_insert(bst, &p12->node);
    dl_bstree_insert(bst, &p13->node);
    dl_bstree_insert(bst, &p14->node);
    node = dl_bstree_insert(bst, &p15->node);

    dump_tree();

    
}


