/*
 * reference
 *  nginx
 *  https://en.wikipedia.org/wiki/Red-black_tree
 */

#include "dl_base.h"

static inline void
dl_rbtree_left_rotate(dl_rbtree_node **root, dl_rbtree_node *node, dl_rbtree_node *sentinel);
static inline void
dl_rbtree_right_rotate(dl_rbtree_node **root, dl_rbtree_node *node, dl_rbtree_node *sentinel);
static void
dl_rbtree_repair(dl_rbtree_node **root, dl_rbtree_node *node, dl_rbtree_node *sentinel);
static void
dl_rbtree_delete_repair(dl_rbtree_node **root, dl_rbtree_node *n, dl_rbtree_node *sentinel, int nest);
static void
dl_rbtree_del_repair_done(dl_rbtree_node *node, dl_rbtree_node *sentinel);

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

    dl_rbtree_repair(root, node, sentinel);
}

static void
dl_rbtree_repair(dl_rbtree_node **root, dl_rbtree_node *node, dl_rbtree_node *sentinel)
{
    dl_rbtree_node *p;
    dl_rbtree_node *g;

    if(node->parent == NULL){
        /*
         * root, paint it to black
         */
        dl_rbt_black(node);

    }else if(dl_rbt_is_black(node->parent)){
        /*
         * if parent color is black, tree is still valid
         */
        return;

    }else if(dl_rbt_is_red(dl_rbtree_uncle(node))){
        /*
         * parent node is red, uncle color is red
         * recolor and try to repair grandparent node.
         */
        dl_rbt_black(node->parent);
        dl_rbt_black(dl_rbtree_uncle(node));

        g = node->parent->parent;;
        dl_rbt_red(g);
        dl_rbtree_repair(root, g, sentinel);

    }else{

        p = node->parent;
        g = node->parent->parent;

        if(node == g->left->right){
            dl_rbtree_left_rotate(root, node, sentinel);
            node = node->left;

        }else if(node == g->right->left){
            dl_rbtree_right_rotate(root, p, sentinel);
            node = node->right;

        }

        p = node->parent;
        g = node->parent->parent;

        if(node == p->left){
            dl_rbtree_right_rotate(root, g, sentinel);
        }else{
            dl_rbtree_left_rotate(root, g, sentinel);
        }

        dl_rbt_black(p);
        dl_rbt_red(g);
    }
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

/*
 * relations
 */

dl_rbtree_node *dl_rbtree_grandparent(dl_rbtree_node *node){
    dl_rbtree_node *p = node->parent;
    if(p == NULL){
        return NULL;    //no parent means no grandparent
    }

    return p->parent;
}

dl_rbtree_node *dl_rbtree_sibling(dl_rbtree_node *node){
    dl_rbtree_node *p = node->parent;
    if(p == NULL){
        return NULL;    //no parent means no sibling
    }

    if(node == p->left)
        return p->right;
    else
        return p->left;
}

dl_rbtree_node *dl_rbtree_uncle(dl_rbtree_node *node){
    dl_rbtree_node *p = node->parent;
    dl_rbtree_node *g = dl_rbtree_grandparent(node);
    if(g == NULL){
        return NULL;    //no grandparent means no uncle
    }

    return dl_rbtree_sibling(p);
}

static inline void
dl_rbtree_left_rotate(dl_rbtree_node **root, dl_rbtree_node *node, dl_rbtree_node *sentinel)
{
    dl_rbtree_node *rn = node->right;
    dl_rbtree_node *p = node->parent;

    node->right = rn->left;
    rn->left = node;

    node->parent = rn;

    if(node->right != sentinel){
        node->right->parent = node;
    }

    if(p != NULL){
        /*
         * not the root node
         */
        if(node == p->left){
            p->left = rn;
        }else{
            p->right = rn;
        }
    }else{
        *root = rn;
    }

    rn->parent = p;
}

static inline void
dl_rbtree_right_rotate(dl_rbtree_node **root, dl_rbtree_node *node, dl_rbtree_node *sentinel)
{
    dl_rbtree_node *ln = node->left;
    dl_rbtree_node *p = node->parent;

    node->left = ln->right;
    ln->right = node;
    node->parent = ln;

    if(node->left != sentinel){
        node->left->parent = node;
    }

    if(p != NULL){
        /*
         * `node` not the root node
         */
        if(node == p->left){
            p->left = ln;
        }else{
            p->right = ln;
        }
    }else{
        *root = ln;
    }

    ln->parent = p;
}

void
dl_rbtree_delete(dl_rbtree *tree, dl_rbtree_node *node)
{
    dl_rbtree_node **root, *sentinel;
    uchar red;

    /* to denote the node to be deleted */
    dl_rbtree_node *d_node;

    /* denote the selected child of d */
    dl_rbtree_node *d_child;

    root = &tree->root;
    sentinel = tree->sentinel;

    if(node->left == sentinel){
        d_child = node->right;
        d_node = node;
    }else if(node->right == sentinel){
        d_child = node->left;
        d_node = node;
    }else{
        d_node = dl_rbtree_min(node->right, sentinel);

        if(d_node->left != sentinel){
            d_child = d_node->left;
        }else{
            d_child = d_node->right;
        }
    }

    red = dl_rbt_is_red(d_node);

    if(d_node == *root){
        /*
         * delete root node and root node has at most one non-leaf child.
         * because if root node has two non-leaf children, d_node will not be *root
         */
        *root = d_child;
        dl_rbt_black(d_child);

        /* DEBUG stuff */
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->key = 0;

        return;
    }

    /*
     * back up d_node
     */
    dl_rbtree_node d_node_bak;
    d_node_bak = *d_node;


    /*
     * remove operation
     */
    /*
     * change d_node parent to d_node backup
     */


    if(d_node == d_node->parent->left){
        d_node->parent->left = &d_node_bak;
    }else{
        d_node->parent->right = &d_node_bak;
    }

    /*
     * replace node with d_node.
     */
    if(d_node != node){

        d_node->left = node->left;
        d_node->right = node->right;
        d_node->parent = node->parent;
        d_node->color = node->color;


        if(d_node->right == &d_node_bak){
            d_node_bak.parent = d_node;
        }

        if(node == *root){
            /*
             * no parent
             */
            *root = d_node;
        }else{
            /*
             * node parent point to new child(d_node)
             */
            if(node == node->parent->left){
                node->parent->left = d_node;
            }else{
                node->parent->right = d_node;
            }

        }

        /*
         * node child point to new parent
         */
        if (d_node->left != sentinel) {
            d_node->left->parent = d_node;
        }

        if (d_node->right != sentinel) {
            d_node->right->parent = d_node;
        }

    }

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;


    /*
     * do repairing
     */

    /*
     *  M-to denote the node to be deleted.
     *  C-to denote the selected child of M.
     *  nil-black sentinel node
     */

    /*
     *      M-red
     *   nil     nil
     *
     * we deleted a red node, just return.
     */
    if(red){
        dl_rbtree_del_repair_done(&d_node_bak, sentinel);
        return;
    }

    /*
     *         M-black
     *      C-red       nil
     *   nil     nil


     *         M-black
     *      nil       C-red
     *             nil     nil
     *
     * just change C-red to black
     */

    if(d_child != sentinel){
        dl_rbt_black(d_child);
        dl_rbtree_del_repair_done(&d_node_bak, sentinel);

        return;
    }

    /*
     *      M-black
     *   nil       nil
     *
     *  we delete a black node with two leaf-nodes(sentinel).
     */

    dl_rbtree_delete_repair(root, &d_node_bak, sentinel, 0);

}

static void
dl_rbtree_del_repair_done(dl_rbtree_node *node, dl_rbtree_node *sentinel){
    dl_rbtree_node *child;

    if(node->left != sentinel){
        child = node->left;
    }else{
        child = node->right;
    }

    if(child != sentinel)
        child->parent = node->parent;

    if(node->parent->left == node){
        node->parent->left = child;
    }else{
        node->parent->right = child;
    }

}

/*
 * we do not really delete a node,
 * our goal is let all paths through d_node(a node will be deleted late) have one more black node
 * than paths that do not pass through d_node.
 *
 * after we achieve the above goal, we remove the d_node and the tree is balanced
 */

static void
dl_rbtree_delete_repair(dl_rbtree_node **root, dl_rbtree_node *n, dl_rbtree_node *sentinel, int nest)
{
    /*
     * case 1
     */
    if(n->parent == NULL){
        *root = n;
        return;
    }

    dl_rbtree_node *s;

    /*
     * case 2
     *         P-black
     *   M-black        S-red
     *             black     black
     *
     *             P-black
     *        S-red       M-black
     *   black     black
     */
    s = dl_rbtree_sibling(n);

    if(dl_rbt_is_red(s)){

        dl_rbt_red(n->parent);
        dl_rbt_black(s);

        if(n == n->parent->left){
            dl_rbtree_left_rotate(root, n->parent, sentinel);
        }else{
            dl_rbtree_right_rotate(root, n->parent, sentinel);
        }

        /* after case 2 not done, go forward */
    }

    /*
     * case 3
     *
     *
               P-black
        M-black     S-black
                  black  black


                    P-black
                S-black   M-black
            black   black
     *
     *
     */

    s = dl_rbtree_sibling(n);

    if(dl_rbt_is_black(n->parent) && dl_rbt_is_black(s) &&
       dl_rbt_is_black(s->left) && dl_rbt_is_black(s->right))
    {
        dl_rbt_red(s);

        /*
         * here P node is balanced with his children,
         * but all paths through P have one fewer black node than paths that do not pass through P
         * so we should rebalance P
         */

        dl_rbtree_delete_repair(root, n->parent, sentinel, 1);
        if(nest == 0)
            dl_rbtree_del_repair_done(n, sentinel);
    }else{

        s = dl_rbtree_sibling(n);

        if(dl_rbt_is_red(n->parent) && dl_rbt_is_black(s) &&
           dl_rbt_is_black(s->left) && dl_rbt_is_black(s->right))
        {
            /* case 4 */
            /*
             * This does not affect the number of black nodes on paths going through S,
             * but it add one to the number of black nodes on paths going through N,
             * we achieve our goal here.
             */

            dl_rbt_red(s);
            dl_rbt_black(n->parent);

            if(nest == 0)
                dl_rbtree_del_repair_done(n, sentinel);

        }else{

            /* case 5 */

            if(dl_rbt_is_black(s)){

                if(n == n->parent->left && dl_rbt_is_black(s->right) &&
                   dl_rbt_is_red(s->left))
                {
                    dl_rbt_red(s);
                    dl_rbt_black(s->left);
                    dl_rbtree_right_rotate(root, s, sentinel);

                }else if(n == n->parent->right && dl_rbt_is_black(s->left) &&
                        dl_rbt_is_red(s->right))
                {
                    dl_rbt_red(s);
                    dl_rbt_black(s->right);
                    dl_rbtree_left_rotate(root, s, sentinel);
                }

            }


            /* case 6 */

            s = dl_rbtree_sibling(n);

            dl_rbt_copy_color(s, n->parent);
            dl_rbt_black(n->parent);

            if(n == n->parent->left){
                dl_rbt_black(s->right);
                dl_rbtree_left_rotate(root, n->parent, sentinel);
            }else{
                dl_rbt_black(s->left);
                dl_rbtree_right_rotate(root, n->parent, sentinel);
            }

            /* end case 6 */

            if(nest == 0)
                dl_rbtree_del_repair_done(n, sentinel);

        }

    }


}
