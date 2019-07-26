#include "dl_base.h"

#define dl_bst_height(n) (n ? n->height : 0)
#define dl_bst_get_balance(n) ((n) ? (dl_bst_height((n)->left) - dl_bst_height((n)->right)) : 0)

static dl_bstree_node*dl_bstree_right_rotate(dl_bstree_node *node);
static dl_bstree_node*dl_bstree_left_rotate(dl_bstree_node *node);
static dl_bstree_node * dl_bstree_delete(dl_bstree_node *node, long key);

dl_bstree_node * dl_bstree_delete_by_node(dl_bstree *bst, dl_bstree_node *node)
{
    if(bst->root == NULL || node == NULL) return NULL;

    bst->root = dl_bstree_delete(bst->root, node->key);

    return bst->root;
}

dl_bstree_node * dl_bstree_delete_by_key(dl_bstree *bst, long key)
{
    if(bst->root == NULL) return NULL;

    bst->root = dl_bstree_delete(bst->root, key);

    return bst->root;
}

dl_bstree_node * dl_bstree_find_by_key(dl_bstree *bst, long key)
{
    dl_bstree_node *cur = bst->root;
    
    while(1){
        if(cur == NULL) break;
        if(cur->key == key) return cur;
        
        if(cur->key > key){
            cur = cur->left;
        }else{
            cur = cur->right;
        }
    }

    return NULL;
}

static
dl_bstree_node * dl_bstree_delete(dl_bstree_node *node, long key)
{
    if(node == NULL) return NULL;

    if(key < node->key){
        node->left = dl_bstree_delete(node->left, key);

    }else if(key > node->key){
        node->right = dl_bstree_delete(node->right, key);

    }else{

        dl_bstree_node *child, temp;

        /*
         * node with one child or no child
         */
        if(node->left == NULL || node->right == NULL){
            child = node->left ? node->left : node->right;

            if(child == NULL){
                /*
                 * no child
                 */
                if(node->parent){
                    if(node == node->parent->left){
                        node->parent->left = NULL;
                    }else{
                        node->parent->right = NULL;
                    }
                }

                return NULL;

            }else{
                /*
                 * one child
                 */
                if(node->parent == NULL){
                    child->parent = NULL;
                }else{
                    child->parent = node->parent;

                    if(node == node->parent->right){
                        node->parent->right = child;
                    }else{
                        node->parent->left = child;
                    }

                }

                return child;
            }

        }else{
            /*
             * node with two children
             */
            child = dl_bstree_min(node->right);

            temp = *child;

            if(node->parent == NULL){
                child->parent = NULL;

            }else{
                child->parent = node->parent;

                if(node == node->parent->right){
                    node->parent->right = child;
                }else{
                    node->parent->left = child;
                }

            }

            child->left = node->left;
            node->left->parent = child;

            if(node->right == child){

                child->right = node;
                node->parent = child;

                node->left = NULL;
                node->right = temp.right;
                if(temp.right != NULL) temp.right->parent = node;

            }else{

                child->right = node->right;
                node->right->parent = child;

                node->left = NULL;

                node->parent = temp.parent;
                temp.parent->left = node;

                node->right = temp.right;
                if(temp.right != NULL) temp.right->parent = node;

            }

            child->right = dl_bstree_delete(child->right, node->key);
            node = child;

        }

    }

    /*
     * rebalance
     */

    node->height = 1 + dl_max(dl_bst_height(node->left),
                                  dl_bst_height(node->right));

    int balance = dl_bst_get_balance(node);

    // left left case
    if(balance > 1 && dl_bst_get_balance(node->left) >= 0){
        return dl_bstree_right_rotate(node);
    }

    // right right case
    if(balance < -1 && dl_bst_get_balance(node->right) <=0){
        return dl_bstree_left_rotate(node);
    }

    // left right case
    if(balance > 1 && dl_bst_get_balance(node->left) < 0){
        node->left = dl_bstree_left_rotate(node->left);
        return dl_bstree_right_rotate(node);
    }

    // right left case
    if(balance < -1 && dl_bst_get_balance(node->right) > 0){
        node->right = dl_bstree_right_rotate(node->right);
        return dl_bstree_left_rotate(node);
    }

    return node;

}

static dl_bstree_node*
dl_bstree_right_rotate(dl_bstree_node *node)
{
    dl_bstree_node *root = node->left;
    node->left = root->right;

    if(root->right != NULL)
        root->right->parent = node;

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
    if(root->left != NULL)
        root->left->parent = node;

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
dl_bstree_insert_repair(dl_bstree_node *target_n, dl_bstree_node *node){

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

dl_bstree_node *
dl_bstree_insert_value(dl_bstree_node *target_n, dl_bstree_node *node)
{
    if(node->key < target_n->key){

        if(target_n->left == NULL){
            target_n->left = node;
        }else{
            target_n->left = dl_bstree_insert_value(target_n->left, node);
            if(target_n->left == NULL) return NULL;
        }
        target_n->left->parent = target_n;

    }else if(node->key > target_n->key){

        if(target_n->right == NULL){
            target_n->right = node;
        }else{
            target_n->right = dl_bstree_insert_value(target_n->right, node);
            if(target_n->right == NULL) return NULL;
        }
        target_n->right->parent = target_n;

    }else{
        return NULL;
    }

    return dl_bstree_insert_repair(target_n, node);
}

dl_bstree_node *
dl_bstree_insert(dl_bstree *bst, dl_bstree_node *node)
{
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    if(bst->root == NULL){
        bst->root = node;
        return node;
    }

    node = bst->insert(bst->root, node);
    if(node == NULL){
        /*
         * duplicated
         */

        return NULL;
    }else{
        bst->root = node;
        return node;
    }
}

dl_bstree_node *dl_bstree_next(dl_bstree *bst, dl_bstree_node *node)
{
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
