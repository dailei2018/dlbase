#include "dl_base.h"

static int dl_phash_set(dl_hash *h, dl_str *key, void *v, int type);
static dl_hash_v *pfill_v(dl_hash *h, dl_str *key, void *v, dl_hash_v *hv);
static dl_node *pfill_node(dl_hash *h, dl_str *key, void *v, dl_node *n, int type);


dl_hash *
dl_phash_init(dl_pool *pool, uint slot, int type)
{
    dl_hash *h = dl_palloc(pool, sizeof(dl_hash));
    if(h == NULL) return NULL;

    h->slot_total = slot ? slot : HASH_INIT;
    h->el_sum = 0;
    h->flag = type | P_HASH;

    h->pool = pool;

    h->node = dl_pcalloc(pool, h->slot_total * sizeof(dl_node));
    if(h->node == NULL) return NULL;

    return h;
}

int dl_phash_set_str(dl_hash *h, dl_str *k, dl_str *v){
    return dl_phash_set(h, k, v, DL_STR);
}

int dl_phash_set_int(dl_hash *h, dl_str *k, long v){
    return dl_phash_set(h, k, &v, DL_INT);
}

int dl_phash_set_void(dl_hash *h, dl_str *k, void *v){
    return dl_phash_set(h, k, v, DL_VOID);
}

static int
dl_phash_set(dl_hash *h, dl_str *key, void *v, int type)
{
    int i = 1;
    dl_node *node;

    if(!(h->flag & P_HASH)) return DL_ERROR;

    uint h_key = dl_hash_key((uchar *)key->data, key->len);
    int slot_i = h_key % h->slot_total;

    node = &h->node[slot_i];

    // no collision
    if(node->v.tt == 0){
        if(pfill_node(h, key, v, node, type) == NULL) return DL_ERROR;
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
           memcmp((*n)->k.key.data, key->data, key->len) == 0)
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
        *n = dl_palloc(h->pool, sizeof(dl_node));
        if(pfill_node(h, key, v, *n, type) == NULL) return DL_ERROR;
        (*n)->k.hash = h_key;

        h->el_sum++;

    }else{
        /*
         * key already exits
         * shall we replace it or chain it
         */

        if(h->flag & H_UNIQUE){
            return DL_BUSY;
        }

        dl_hash_v **vv = &(*n)->v.next;

        while(*vv){
            vv = &(*vv)->next;
        }

        *vv = dl_palloc(h->pool, sizeof(dl_hash_v));
        (*vv)->tt = type;

        if(pfill_v(h, key, v, *vv) == NULL) return DL_ERROR;
        h->el_sum++;

    }

    return DL_OK;
}

static dl_hash_v *
pfill_v(dl_hash *h, dl_str *key, void *v, dl_hash_v *hv)
{
    if(hv->tt == DL_STR){
        hv->v.s = dl_palloc(h->pool, sizeof(dl_str));
        if(hv->v.s == NULL) return NULL;

        hv->v.s->data = dl_pstrdup(h->pool, (dl_str *)v);
        if(hv->v.s->data == NULL) return NULL;

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
pfill_node(dl_hash *h, dl_str *key, void *v, dl_node *n, int type)
{
    n->v.tt = type;
    if(pfill_v(h, key, v, &n->v) == NULL) return NULL;

    n->k.key.len = key->len;
    n->k.key.data = dl_pstrdup(h->pool, key);
    if(n->k.key.data == NULL) return NULL;

    n->k.next = NULL;

    return n;
}
