#include "dl_base.h"

static inline void
dl_rbtree_left_rotate(dl_rbtree_node **root, dl_rbtree_node *sentinel,
    dl_rbtree_node *node);

static inline void
dl_rbtree_right_rotate(dl_rbtree_node **root, dl_rbtree_node *sentinel,
    dl_rbtree_node *node);

void
dl_rbtree_insert_value(dl_rbtree_node *temp, dl_rbtree_node *node,
    dl_rbtree_node *sentinel)
{
    dl_rbtree_node  **p;

    for ( ;; ) {

        p = (node->key < temp->key) ? &temp->left : &temp->right;

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    dl_rbt_red(node);
}

void
dl_rbtree_insert(dl_rbtree *tree, dl_rbtree_node *node)
{
    dl_rbtree_node  **root, *temp, *sentinel;

    /* a binary tree insert */

    root = &tree->root;
    sentinel = tree->sentinel;

    if (*root == sentinel) {
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        dl_rbt_black(node);
        *root = node;

        return;
    }

    tree->insert(*root, node, sentinel);

    /* re-balance tree */

    while (node != *root && dl_rbt_is_red(node->parent)) {

        if (node->parent == node->parent->parent->left) {
            temp = node->parent->parent->right;

            if (dl_rbt_is_red(temp)) {
                dl_rbt_black(node->parent);
                dl_rbt_black(temp);
                dl_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    dl_rbtree_left_rotate(root, sentinel, node);
                }

                dl_rbt_black(node->parent);
                dl_rbt_red(node->parent->parent);
                dl_rbtree_right_rotate(root, sentinel, node->parent->parent);
            }

        } else {
            temp = node->parent->parent->left;

            if (dl_rbt_is_red(temp)) {
                dl_rbt_black(node->parent);
                dl_rbt_black(temp);
                dl_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    dl_rbtree_right_rotate(root, sentinel, node);
                }

                dl_rbt_black(node->parent);
                dl_rbt_red(node->parent->parent);
                dl_rbtree_left_rotate(root, sentinel, node->parent->parent);
            }
        }
    }

    dl_rbt_black(*root);
}

static inline void
dl_rbtree_left_rotate(dl_rbtree_node **root, dl_rbtree_node *sentinel,
    dl_rbtree_node *node)
{
    dl_rbtree_node  *temp;

    temp = node->right;
    node->right = temp->left;

    if (temp->left != sentinel) {
        temp->left->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->left) {
        node->parent->left = temp;

    } else {
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
}


static inline void
dl_rbtree_right_rotate(dl_rbtree_node **root, dl_rbtree_node *sentinel,
    dl_rbtree_node *node)
{
    dl_rbtree_node  *temp;

    temp = node->left;
    node->left = temp->right;

    if (temp->right != sentinel) {
        temp->right->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->right) {
        node->parent->right = temp;

    } else {
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;
}


dl_rbtree_node *
dl_rbtree_next(dl_rbtree *tree, dl_rbtree_node *node)
{
    dl_rbtree_node  *root, *sentinel, *parent;

    sentinel = tree->sentinel;

    if (node->right != sentinel) {
        return dl_rbtree_min(node->right, sentinel);
    }

    root = tree->root;

    for ( ;; ) {
        parent = node->parent;

        if (node == root) {
            return NULL;
        }

        if (node == parent->left) {
            return parent;
        }

        node = parent;
    }
}
