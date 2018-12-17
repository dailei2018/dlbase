#include "dl_base.h"

#define dl_bst_height(n) (n ? n->height : 0)
#define dl_bst_get_balance(n) ((n) ? (dl_bst_height((n)->left) - dl_bst_height((n)->right)) : 0)

static dl_bstree_node*
dl_bstree_right_rotate(dl_bstree_node *node)
{
    dl_bstree_node *root = node->left;
    node->left = root->right;
    root->right = node;

    node->parent = root;
    root->parent = NULL;

    node->height = 1 + dl_max(dl_bst_height(node->left),
                                  dl_bst_height(node->right));
    root->height = 1 + dl_max(dl_bst_height(root->left),
                                  dl_bst_height(root->right));
    return root;
}

static dl_bstree_node*
dl_bstree_left_rotate(dl_bstree_node *node)
{
    dl_bstree_node *root = node->right;
    node->right = root->left;
    root->left = node;

    node->parent = root;
    root->parent = NULL;

    node->height = 1 + dl_max(dl_bst_height(node->left),
                                  dl_bst_height(node->right));
    root->height = 1 + dl_max(dl_bst_height(root->left),
                                  dl_bst_height(root->right));
    return root;
}

static dl_bstree_node *
dl_bstree_insert_n(dl_bstree_node *target_n, dl_bstree_node *node)
{
    if(target_n == NULL){
        return node;
    }

    if(node->key < target_n->key){
        target_n->left = dl_bstree_insert_n(target_n->left, node);
        target_n->left->parent = target_n;
    }else if(node->key > target_n->key){
        target_n->right = dl_bstree_insert_n(target_n->right, node);
        target_n->right->parent = target_n;
    }else{
        return target_n;
    }

    target_n->height = 1 + dl_max(dl_bst_height(target_n->left),
                                  dl_bst_height(target_n->right));

    int balance = dl_bst_get_balance(target_n);

    // left left case
    if(balance > 1 && node->key < target_n->left->key){
        return dl_bstree_right_rotate(target_n);
    }

    // right right case
    if(balance < -1 && node->key > target_n->right->key){
        return dl_bstree_left_rotate(target_n);
    }

    // left right case
    if(balance > 1 && node->key > target_n->left->key){
        target_n->left = dl_bstree_left_rotate(target_n->left);
        return dl_bstree_right_rotate(target_n);
    }

    // right left case
    if(balance < -1 && node->key < target_n->right->key){
        target_n->right = dl_bstree_right_rotate(target_n->right);
        return dl_bstree_left_rotate(target_n);
    }

    return target_n;
}

void
dl_bstree_insert(dl_bstree *bst, dl_bstree_node *node)
{
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    if(bst->root == NULL){
        bst->root = node;
        bst->leftmost = node;
        bst->rightmost = node;

        return;
    }

    if(node->key < bst->leftmost->key){
        bst->leftmost = node;
    }

    if(node->key > bst->rightmost->key){
        bst->rightmost = node;
    }

    bst->root = dl_bstree_insert_n(bst->root, node);
}

dl_bstree_node *dl_bstree_next(dl_bstree *bst, dl_bstree_node *node){
    if(node == bst->rightmost) return NULL;

    if(node->right != NULL){
        return dl_bstree_min(node->right);
    }

    for(;;){
        dl_bstree_node *parent = node->parent;

        if(node == bst->root){
            return NULL;
        }

        if(parent->left == node){
            return parent;
        }

        node = parent;
    }



}


/*
 * debug
 */

void dl_bstree_dump(dl_bstree_node *node)
{
    if(node != NULL)
    {
        printf("%ld\n", node->key);
        dl_bstree_dump(node->left);
        dl_bstree_dump(node->right);
    }
}



/*
[left left case]
T1, T2, T3 and T4 are subtrees.
         z                                      y
        / \                                   /   \
       y   T4      Right Rotate (z)          x      z
      / \          - - - - - - - - ->      /  \    /  \
     x   T3                               T1  T2  T3  T4
    / \
  T1   T2


[Left Right Case]

     z                               z                           x
    / \                            /   \                        /  \
   y   T4  Left Rotate (y)        x    T4  Right Rotate(z)    y      z
  / \      - - - - - - - - ->    /  \      - - - - - - - ->  / \    / \
T1   x                          y    T3                    T1  T2 T3  T4
    / \                        / \
  T2   T3                    T1   T2

[Right Right Case]

  z                                y
 /  \                            /   \
T1   y     Left Rotate(z)       z      x
    /  \   - - - - - - - ->    / \    / \
   T2   x                     T1  T2 T3  T4
       / \
     T3  T4


[Right Left Case]

   z                            z                            x
  / \                          / \                          /  \
T1   y   Right Rotate (y)    T1   x      Left Rotate(z)   z      y
    / \  - - - - - - - - ->     /  \   - - - - - - - ->  / \    / \
   x   T4                      T2   y                  T1  T2  T3  T4
  / \                              /  \
T2   T3                           T3   T4





*/
