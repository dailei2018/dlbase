#ifndef __DL_BSTREE
#define __DL_BSTREE

#include "dl_base.h"

/*
 * AVT
 */

typedef struct _dl_bstree dl_bstree;
typedef struct _dl_bstree_node dl_bstree_node;

struct _dl_bstree{
    dl_bstree_node *root;
    dl_bstree_node *leftmost;
    dl_bstree_node *rightmost;
};

struct _dl_bstree_node{
    long key;
    ulong height;

    dl_bstree_node *left;
    dl_bstree_node *right;
    dl_bstree_node *parent;
};

static inline dl_bstree_node *dl_bstree_min(dl_bstree_node *node)
{
    if(node == NULL) return node;

    while(node->left){
        node = node->left;
    }

    return node;
}

void dl_bstree_insert(dl_bstree *bst, dl_bstree_node *node);

/*
 * debug
 */

void dl_bstree_dump(dl_bstree_node *node);


#endif
