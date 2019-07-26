#ifndef __DL_RBTREE
#define __DL_RBTREE

#include "dl_base.h"

typedef struct _dl_rbtree dl_rbtree;
typedef struct _dl_rbtree_node dl_rbtree_node;

typedef void (*dl_rbtree_insert_pt) (dl_rbtree_node *root,
        dl_rbtree_node *node, dl_rbtree_node *sentinel);

struct _dl_rbtree{
    dl_rbtree_node      *root;
    dl_rbtree_node      *sentinel;
    dl_rbtree_insert_pt  insert;
};

struct _dl_rbtree_node{
    long key;

    dl_rbtree_node *left;
    dl_rbtree_node *right;
    dl_rbtree_node *parent;
    uchar           color;
    uchar           data;
};

#define dl_rbtree_init(tree, n, i)                                           \
    dl_rbt_black(n);                                             \
    (tree)->root = n;                                                         \
    (tree)->sentinel = n;                                                     \
    (tree)->insert = i

#define dl_rbt_red(node)               ((node)->color = 1)
#define dl_rbt_black(node)             ((node)->color = 0)
#define dl_rbt_is_red(node)            ((node)->color)
#define dl_rbt_is_black(node)          (!dl_rbt_is_red(node))
#define dl_rbt_copy_color(n1, n2)      (n1->color = n2->color)


static inline dl_rbtree_node *
dl_rbtree_min(dl_rbtree_node *node, dl_rbtree_node *sentinel)
{

    while (node->left != sentinel) {
        node = node->left;
    }

    return node;
}

dl_rbtree_node *dl_rbtree_next(dl_rbtree *tree, dl_rbtree_node *node);

void dl_rbtree_insert(dl_rbtree *tree, dl_rbtree_node *node);
void dl_rbtree_insert_value(dl_rbtree_node *temp, dl_rbtree_node *node,
    dl_rbtree_node *sentinel);

void
dl_rbtree_delete(dl_rbtree *tree, dl_rbtree_node *node);

dl_rbtree_node *dl_rbtree_grandparent(dl_rbtree_node *node);
dl_rbtree_node *dl_rbtree_uncle(dl_rbtree_node *node);
dl_rbtree_node *dl_rbtree_sibling(dl_rbtree_node *node);

#endif
