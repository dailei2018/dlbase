#include "dl_base.h"

static int dl_phash_set(dl_phash *h, dl_str *key, void *v, int type);
static dl_phash_v *pfill_v(dl_phash_v *hv, void *v);
static dl_pnode *pfill_node(dl_phash *h, dl_str *key, void *v, dl_pnode *n, int type);

uint
dl_phash_key(uchar *data, size_t len)
{
    uint i, key;

    key = 0;
    len = len > 20 ? 20 : len;
    
    for (i = 0; i < len; i++) {
        key = key * 31 + data[i];
    }

    return key;
}

dl_phash *
dl_phash_init(dl_pool *pool, uint slot, int type)
{
    dl_phash *h = dl_palloc(pool, sizeof(dl_phash));
    if(h == NULL) return NULL;

    h->slot_total = slot ? slot : PHASH_INIT;
    h->el_sum = 0;
    h->flag = type;

    h->pool = pool;

    h->node = dl_pcalloc(pool, h->slot_total * sizeof(dl_pnode));
    if(h->node == NULL) return NULL;

    return h;
}

int dl_phash_set_str(dl_phash *h, char *data, int len, dl_str *v){
    dl_str *k = dl_palloc(h->pool, sizeof(dl_str));
    k->data = data;
    k->len = len;
    return dl_phash_set(h, k, v, DL_PH_STR);
}

int dl_phash_set_int(dl_phash *h, char *data, int len, long v){
    dl_str *k = dl_palloc(h->pool, sizeof(dl_str));
    k->data = data;
    k->len = len;
    return dl_phash_set(h, k, &v, DL_PH_INT);
}

int dl_phash_set_void(dl_phash *h, char *data, int len, void *v){
    dl_str *k = dl_palloc(h->pool, sizeof(dl_str));
    k->data = data;
    k->len = len;
    return dl_phash_set(h, k, v, DL_PH_VOID);
}

static int
dl_phash_set(dl_phash *h, dl_str *key, void *v, int type)
{
    int i = 1;
    dl_pnode *node;

    uint h_key = dl_phash_key((uchar *)key->data, key->len);
    int slot_i = h_key % h->slot_total;

    node = &h->node[slot_i];

    // no collision
    if(node->v.tt == DL_PH_NULL){
        if(pfill_node(h, key, v, node, type) == NULL) return DL_ERROR;
        node->k.hash = h_key;
        node->k.next = NULL;
        node->v.next = NULL;

        h->el_sum++;
        return DL_OK;
    }

    dl_pnode *n = node;
    
    while(n){
        if(n->k.hash == h_key &&
           memcmp(n->k.key->data, key->data, key->len) == 0)
        {
            break;
        }else{
            n = n->k.next;
        }
    }
    
    if(n == NULL){
        /*
         *  key not exits
         */
        dl_pnode    *n_next = node->k.next;
         
        n = dl_palloc(h->pool, sizeof(dl_pnode));
        if(n == NULL) return DL_ERROR;
        node->k.next = n;
        n->k.next = n_next;

        if(pfill_node(h, key, v, n, type) == NULL) return DL_ERROR;
        n->k.hash = h_key;
        n->v.next = NULL;

        h->el_sum++;

    }else{
        /*
         * key already exits
         */
         
        if(h->flag & PH_UNIQUE){
            return DL_AGAIN;
        }
         
        if(h->flag & PH_REPLACE){
        if(pfill_node(h, key, v, n, type) == NULL) return DL_ERROR;
            n->k.hash = h_key;
            n->v.next = NULL;
            
            return DL_OK;
        }

        
        /* chain value */
            
        dl_phash_v   *vv, *next_v;
        
        next_v = n->v.next;
        
        vv = dl_palloc(h->pool, sizeof(dl_phash_v));
        n->v.next = vv;
        vv->next = next_v;
        vv->tt = type;

        if(pfill_v(vv, v) == NULL) return DL_ERROR;
        h->el_sum++;
        
    }

    return DL_OK;
}

static dl_phash_v *
pfill_v(dl_phash_v *hv, void *v)
{
    if(hv->tt == DL_PH_STR){
        hv->v.s = (dl_str *)v;
    }else if(hv->tt == DL_PH_INT){
        hv->v.n = *(int *)v;
    }else if(hv->tt == DL_PH_VOID){
        hv->v.v = v;
    }else{
        return NULL;
    }

    return hv;
}

static dl_pnode *
pfill_node(dl_phash *h, dl_str *key, void *v, dl_pnode *n, int type)
{
    n->v.tt = type;
    if(pfill_v(&n->v, v) == NULL) return NULL;

    n->k.key = key;

    return n;
}

dl_phash_v * dl_phash_find(dl_phash *h, char *data, int len)
{
    dl_pnode *node;

    uint h_key = dl_hash_key((uchar *)data, len);
    int slot_i = h_key % h->slot_total;

    node = &h->node[slot_i];

    if(node->v.tt == DL_PH_NULL) return NULL;

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

/*
 * debug
 */

static void dl_dump_phash_v(dl_phash_v *v){
    
    while(v){
        switch(v->tt){
            case DL_H_INT:
                dl_printf("%l ", v->v.n);
                break;
            case DL_H_STR:
                dl_printf("'%V' ", v->v.s);
                break;
            case DL_H_VOID:
                dl_printf("%p ", v->v.v);
                break;
        }
        
        v = v->next;
    }
    
}

void
dl_dump_phash(dl_phash *h)
{
    dl_pnode *node;
    int i;
    for(i = 0; i < h->slot_total; i++){
        node = &h->node[i];

        if(node->v.tt == DL_H_NULL){
            printf("slot:%d, empty\n", i);
        }else{
            printf("slot:%d\n", i);

            while(node){
                dl_printf("  key:'%V' value:", node->k.key);
                
                dl_dump_phash_v(&node->v);
                puts("");
                
                node = node->k.next;
            }

        }

        puts("");
    }
}
