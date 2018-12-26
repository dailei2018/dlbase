#include "dl_base.h"

static int dl_hash_set(dl_hash *h, dl_str *key, void *v, int type, int replace);
static void free_v(dl_hash_v *v);
static void free_node(dl_node *node);
static dl_hash_v *fill_v(dl_hash *h, dl_str *key, void *v, dl_hash_v *hv);
static dl_node *fill_node(dl_hash *h, dl_str *key, void *v, dl_node *n, int type);

dl_hash *
dl_hash_init(dl_pool *pool, uint slot, int type)
{
    dl_hash *h = dl_alloc(sizeof(dl_hash), pool->log);
    if(h == NULL) return NULL;

    h->slot_total = slot ? slot : HASH_INIT;
    h->el_sum = 0;
    h->flag = type;

    h->pool = pool;

    h->node = dl_calloc(h->slot_total * sizeof(dl_node), pool->log);
    if(h->node == NULL) return NULL;

    return h;
}

int dl_hash_set_str(dl_hash *h, char *data, int len, dl_str *v){
    dl_str str;
    str.data = data;
    str.len = len;
    return dl_hash_set(h, &str, v, DL_STR, 0);
}
int dl_hash_set_int(dl_hash *h, char *data, int len, long v){
    dl_str str;
    str.data = data;
    str.len = len;
    return dl_hash_set(h, &str, &v, DL_INT, 0);
}
int dl_hash_set_void(dl_hash *h, char *data, int len, void *v){
    dl_str str;
    str.data = data;
    str.len = len;
    return dl_hash_set(h, &str, v, DL_VOID, 0);
}
int dl_hash_set_str_rep(dl_hash *h, char *data, int len, dl_str *v){
    dl_str str;
    str.data = data;
    str.len = len;
    return dl_hash_set(h, &str, v, DL_STR, 1);
}
int dl_hash_set_int_rep(dl_hash *h, char *data, int len, long v){
    dl_str str;
    str.data = data;
    str.len = len;
    return dl_hash_set(h, &str, &v, DL_INT, 1);
}
int dl_hash_set_void_rep(dl_hash *h, char *data, int len, void *v){
    dl_str str;
    str.data = data;
    str.len = len;
    return dl_hash_set(h, &str, v, DL_VOID, 1);
}

static int dl_hash_set(dl_hash *h, dl_str *key, void *v, int type, int replace){
    int i = 1;
    dl_node *node;

    if(h->flag & P_HASH) return DL_ERROR;

    uint h_key = dl_hash_key((uchar *)key->data, key->len);
    int slot_i = h_key % h->slot_total;

    node = &h->node[slot_i];

    // no collision
    if(node->v.tt == 0){
        if(fill_node(h, key, v, node, type) == NULL) return DL_ERROR;
        node->k.hash = h_key;

        h->el_sum++;
        return DL_OK;
    }

    /*
     * deal with collision
     */
    dl_node **n;

    n = &node;
    while(*n){
        if((*n)->k.hash == h_key &&
           memcmp((*n)->k.key->data, key->data, key->len) == 0)
        {
            break;
        }else{
            n = &(*n)->k.next;
        }
    }

    if(*n == NULL){
        /*
         *  key not exits
         */
        *n = dl_alloc(sizeof(dl_node), h->pool->log);
        if(*n == NULL) return DL_ERROR;

        if(fill_node(h, key, v, *n, type) == NULL) return DL_ERROR;
        (*n)->k.hash = h_key;

        h->el_sum++;

    }else{
        /*
         * key already exits
         * shall we replace it or chain it
         */

        // replace
        if((h->flag & H_UNIQUE) || replace == 1){
            free_node(node);
            node->v.tt = 0;

            if(fill_node(h, key, v, node, type) == NULL) return DL_ERROR;
            node->k.hash = h_key;

            return DL_OK;
        }

        dl_hash_v **vv = &(*n)->v.next;

        while(*vv){
            vv = &(*vv)->next;
        }

        *vv = dl_alloc(sizeof(dl_hash_v), h->pool->log);
        (*vv)->tt = type;

        if(fill_v(h, key, v, *vv) == NULL) return DL_ERROR;
        h->el_sum++;

    }

    return DL_OK;
}

void free_hash(dl_hash *h){
    int i;

    if(h->flag & P_HASH) return;

    for(i = 0; i < h->slot_total; i++){
        free_node(&h->node[i]);
    }

    dl_free(h->node);
    dl_free(h);
}

static void free_v(dl_hash_v *v){
    dl_hash_v *tmp_v = v;

    while(v){
        if(v->tt == DL_STR){
            dl_free(v->v.s->data);
            dl_free(v->v.s);
        }

        tmp_v = v->next;
        dl_free(v);
        v = tmp_v;
    }
}

static void free_node(dl_node *node){
    dl_node *first_n = node;

    if(first_n->v.tt == 0) return;

    node = node->k.next;
    while(node){
        dl_free(node->k.key->data);
        dl_free(node->k.key);

        free_v(&node->v);

        node = node->k.next;
    }

    dl_hash_v *v = first_n->v.next;
    free_v(v);

    if(first_n->v.tt == DL_STR){
        dl_free(first_n->v.v.s->data);
        dl_free(first_n->v.v.s);
    }

    dl_free(first_n->k.key->data);
}

static dl_hash_v *
fill_v(dl_hash *h, dl_str *key, void *v, dl_hash_v *hv)
{
    if(hv->tt == DL_STR){
        hv->v.s = dl_alloc(sizeof(dl_str), h->pool->log);
        if(hv->v.s == NULL) return NULL;

        hv->v.s->data = dl_alloc(((dl_str *)v)->len, h->pool->log);
        if(hv->v.s->data == NULL) return NULL;

        memcpy(hv->v.s->data, ((dl_str *)v)->data, ((dl_str *)v)->len);
        hv->v.s->len = ((dl_str *)v)->len;
    }else if(hv->tt == DL_INT){
        hv->v.n = *(int *)v;
    }else if(hv->tt == DL_VOID){
        hv->v.v = v;
    }else{
        return NULL;
    }

    hv->next = NULL;

    return hv;
}

static dl_node *
fill_node(dl_hash *h, dl_str *key, void *v, dl_node *n, int type)
{
    n->v.tt = type;
    if(fill_v(h, key, v, &n->v) == NULL) return NULL;

    n->k.key = dl_alloc(sizeof(dl_str), h->pool->log);
    if(n->k.key == NULL) return NULL;

    n->k.key->len = key->len;
    n->k.key->data = dl_alloc(key->len, h->pool->log);
    memcpy(n->k.key->data, key->data, key->len);
    if(n->k.key->data == NULL) return NULL;

    n->k.next = NULL;

    return n;
}

dl_hash_v * dl_hash_get(dl_hash *h, char *data, int len){
    dl_node *node;

    uint h_key = dl_hash_key((uchar *)data, len);
    int slot_i = h_key % h->slot_total;

    node = &h->node[slot_i];

    if(node->v.tt == 0) return NULL;

    while(node){
        if(node->k.hash == h_key &&
           memcmp(node->k.key->data, data, len) == 0)
        {
            return &node->v;
        }else{
            node = node->k.next;
        }
    }

    return NULL;

}

uint
dl_hash_key(uchar *data, size_t len)
{
    uint i, key;

    key = 0;

    for (i = 0; i < len; i++) {
        key = key * 31 + data[i];
    }

    return key;
}

/*
 * debug
 */

void
dump_hash_key(dl_hash *h)
{
    char buf[64];
    dl_node *node;
    dl_hash_k *k;
    int i;
    for(i = 0; i < h->slot_total; i++){
        node = &h->node[i];

        if(node->v.tt == 0){
            printf("slot:%d, empty\n", i);
        }else{
            printf("slot:%d\n", i);

            while(node){
                memcpy(buf, node->k.key->data, node->k.key->len);
                buf[node->k.key->len] = '\0';
                printf("  key:%s hash:%u\n", buf, node->k.hash);
                node = node->k.next;
            }

        }

        puts("");
    }
}
