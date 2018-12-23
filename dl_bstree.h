#ifndef __DL_BSTREE
#define __DL_BSTREE

#include "dl_base.h"

/*
 * AVT
 */

typedef struct _dl_bstree dl_bstree;
typedef struct _dl_bstree_node dl_bstree_node;

typedef dl_bstree_node * (*dl_bstree_insert_pt) (dl_bstree_node *root, dl_bstree_node *node);

struct _dl_bstree{
    dl_bstree_node *root;
    dl_bstree_insert_pt insert;
};

struct _dl_bstree_node{
    long key;
    ulong height;

    dl_bstree_node *left;
    dl_bstree_node *right;
    dl_bstree_node *parent;
};

#define dl_bstree_init(tree, i)     \
    (tree)->root = NULL;            \
    (tree)->insert = i


static inline dl_bstree_node *dl_bstree_min(dl_bstree_node *node)
{
    if(node == NULL) return node;

    while(node->left){
        node = node->left;
    }

    return node;
}

dl_bstree_node * dl_bstree_insert(dl_bstree *bst, dl_bstree_node *node);

dl_bstree_node *
dl_bstree_insert_value(dl_bstree_node *target_n, dl_bstree_node *node);

dl_bstree_node *dl_bstree_next(dl_bstree *bst, dl_bstree_node *node);
dl_bstree_node * dl_bstree_delete_by_key(dl_bstree *bst, long key);
dl_bstree_node * dl_bstree_delete_by_node(dl_bstree *bst, dl_bstree_node *node);

#endif
