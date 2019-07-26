#include "dl_base.h"

static int dl_hash_set(dl_hash *h, dl_str *key, void *v, int type);
static void free_k(dl_hash *h, dl_hash_k *k);
static void free_v(dl_hash *h, dl_node *main, dl_hash_v *v);
static void free_node(dl_hash *h, dl_node *main, dl_node *node);

static dl_hash_v *fill_v(dl_hash *h, dl_hash_v *hv, void *v);
static dl_node *fill_node(dl_hash *h, dl_str *key, void *v, dl_node *n, int type);

dl_hash *
dl_hash_init(dl_log *log, uint slot, dl_h_void_replace_pt handler, int type)
{
    dl_hash *h = dl_alloc(sizeof(dl_hash), log);
    if(h == NULL) return NULL;

    h->slot_total = slot ? slot : HASH_INIT;
    h->el_sum = 0;
    h->flag = type;

    h->log = log;
    h->void_h = handler;

    h->node = dl_calloc(h->slot_total * sizeof(dl_node), h->log);
    if(h->node == NULL) return NULL;

    return h;
}

int dl_hash_set_str(dl_hash *h, char *k, size_t len_k, char *v, size_t len_v)
{
    dl_str key, value;
    key.data = k;
    key.len = len_k;
    
    value.data = v;
    value.len = len_v;
    
    return dl_hash_set(h, &key, &value, DL_H_STR);
}
int dl_hash_set_int(dl_hash *h, char *k, size_t len_k, long v)
{
    dl_str key;
    key.data = k;
    key.len = len_k;
    
    return dl_hash_set(h, &key, &v, DL_H_INT);
}
int dl_hash_set_void(dl_hash *h, char *k, size_t len_k, void *v)
{
    dl_str key;
    key.data = k;
    key.len = len_k;
    return dl_hash_set(h, &key, v, DL_H_VOID);
}

int dl_hash_set_str_rep(dl_hash *h, char *k, size_t len_k, char *v, size_t len_v)
{
    dl_hash_del(h, k, len_k);
    
    dl_str key, value;
    key.data = k;
    key.len = len_k;
    
    value.data = v;
    value.len = len_v;
    return dl_hash_set(h, &key, &value, DL_H_STR);
}
int dl_hash_set_int_rep(dl_hash *h, char *k, size_t len_k, long v)
{
    dl_hash_del(h, k, len_k);
    
    dl_str key;
    key.data = k;
    key.len = len_k;
    
    return dl_hash_set(h, &key, &v, DL_H_INT);
}
int dl_hash_set_void_rep(dl_hash *h, char *k, size_t len_k, void *v)
{
    dl_hash_del(h, k, len_k);
    
    dl_str key;
    key.data = k;
    key.len = len_k;
    return dl_hash_set(h, &key, v, DL_H_VOID);
}

static int dl_hash_set(dl_hash *h, dl_str *key, void *v, int type)
{
    int i = 1;
    dl_node *node;

    uint h_key = dl_hash_key((uchar *)key->data, key->len);
    int slot_i = h_key % h->slot_total;

    node = &h->node[slot_i];

    // no collision
    if(node->v.tt == DL_H_NULL){
        if(fill_node(h, key, v, node, type) == NULL) return DL_ERROR;
        node->k.hash = h_key;
        node->k.next = NULL;
        node->v.next = NULL;

        h->el_sum++;
        return DL_OK;
    }

    /*
     * deal with collision
     */
    dl_node *n = node;
    
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
         dl_node    *n_next = node->k.next;
         
         n = dl_alloc(sizeof(dl_node), h->log);
         if(n == NULL) return DL_ERROR;
         node->k.next = n;
         n->k.next = n_next;

        if(fill_node(h, key, v, n, type) == NULL) return DL_ERROR;
        n->k.hash = h_key;
        n->v.next = NULL;

        h->el_sum++;

    }else{
        /*
         * key already exits
         */

        /* chain value */
        if(h->flag & H_LIST){
            
            dl_hash_v   *vv, *next_v;
            
            next_v = n->v.next;
            
            vv = dl_alloc(sizeof(dl_hash_v), h->log);
            n->v.next = vv;
            vv->next = next_v;
            vv->tt = type;

            if(fill_v(h, vv, v) == NULL) return DL_ERROR;
            h->el_sum++;
        }else{
            
            return DL_AGAIN;
        }
        
    }


    return DL_OK;
}

void dl_hash_del(dl_hash *h, char *key, size_t len)
{
    dl_node     *n, *node, backup_n, *parent;
    uint        h_key;
    int         slot_i;
    
    h_key = dl_hash_key((uchar *)key, len);
    slot_i = h_key % h->slot_total;

    node = &h->node[slot_i];
    
    if(node->v.tt == DL_H_NULL){
        return;
    }
    
    n = node;
    
    while(n){
        if(n->k.hash == h_key &&
           memcmp(n->k.key->data, key, len) == 0)
        {
            break;
        }else{
            n = n->k.next;
        }
    }
    
    if(n == NULL) return;
    
    if(n == node){
        /* main node */
        if(n->k.next){
            /* move next node to the main node */
            backup_n = *n;
            
            *n = *n->k.next;
            *backup_n.k.next = backup_n;
            
            free_node(h, node, backup_n.k.next);
            
        }else{
            free_v(h, node, &n->v);
            free_k(h, &n->k);
            
        }
        
    }else{
        /* not the main node */
        parent = node;
        
        while(parent->k.next != n){
            parent = parent->k.next;
        }
        
        parent->k.next = n->k.next;
        
        free_node(h, node, n);
        
    }
    
    
}

void dl_hash_destroy(dl_hash *h)
{
    dl_node     *node, *tmp_n1, *tmp_n2;
    int i;
    
    for(i = 0; i < h->slot_total; i++){
        node = &h->node[i];
        tmp_n1 = node;
        
        //printf("%d\n", i);
        
        if(node->v.tt == DL_H_NULL){
            continue;
        }else{
            int j = 0;
            while(tmp_n1){
                //printf("main:%p  node:%p %d\n", node, tmp_n1, j++);
                tmp_n2 = tmp_n1->k.next;
                free_node(h, node, tmp_n1);
                tmp_n1 = tmp_n2;
            }

        }
    }
    
    dl_free(h->node);
    dl_free(h);
}

static void free_k(dl_hash *h, dl_hash_k *k)
{
    dl_free(k->key->data);
    dl_free(k->key);
    k->next = NULL;
    k->hash = 0;
}

static void free_v(dl_hash *h, dl_node *main, dl_hash_v *v)
{    
    if(v->tt == DL_H_STR){
        dl_free(v->v.s->data);
        dl_free(v->v.s);
    }else if(v->tt == DL_H_VOID){
        h->void_h(v->v.v);
    }
    
    v->tt = DL_H_NULL;
    v = v->next;
    
    while(v){
        dl_hash_v  *tmp_v;
        
        if(v->tt == DL_H_STR){
            dl_free(v->v.s->data);
            dl_free(v->v.s);
        }else if(v->tt == DL_H_VOID){
            h->void_h(v->v.v);
        }

        tmp_v = v->next;
        dl_free(v);
        v = tmp_v;
    }
}


static void free_node(dl_hash *h, dl_node *main, dl_node *node){
    free_k(h, &node->k);
    free_v(h, main, &node->v);
    
    if(main != node)
        dl_free(node);
}

static dl_hash_v *
fill_v(dl_hash *h, dl_hash_v *hv, void *v)
{
    if(hv->tt == DL_H_STR){
        hv->v.s = dl_alloc(sizeof(dl_str), h->log);
        if(hv->v.s == NULL) return NULL;

        hv->v.s->data = dl_alloc(((dl_str *)v)->len, h->log);
        if(hv->v.s->data == NULL) return NULL;

        memcpy(hv->v.s->data, ((dl_str *)v)->data, ((dl_str *)v)->len);
        hv->v.s->len = ((dl_str *)v)->len;
    }else if(hv->tt == DL_H_INT){
        hv->v.n = *(int *)v;
    }else if(hv->tt == DL_H_VOID){
        hv->v.v = v;
    }else{
        return NULL;
    }

    return hv;
}

static dl_node *
fill_node(dl_hash *h, dl_str *key, void *v, dl_node *n, int type)
{
    n->v.tt = type;
    if(fill_v(h, &n->v, v) == NULL) return NULL;

    n->k.key = dl_alloc(sizeof(dl_str), h->log);
    if(n->k.key == NULL) return NULL;

    n->k.key->len = key->len;
    n->k.key->data = dl_alloc(key->len, h->log);
    memcpy(n->k.key->data, key->data, key->len);
    if(n->k.key->data == NULL) return NULL;

    return n;
}

dl_hash_v * dl_hash_find(dl_hash *h, char *data, int len)
{
    dl_node *node;

    uint h_key = dl_hash_key((uchar *)data, len);
    int slot_i = h_key % h->slot_total;

    node = &h->node[slot_i];

    if(node->v.tt == DL_H_NULL) return NULL;

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
    
    len = len > 20 ? 20 : len;
    
    for (i = 0; i < len; i++) {
        key = key * 31 + data[i];
    }

    return key;
}

/*
 * debug
 */

void dl_dump_hash_v(dl_hash_v *v){
    
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
dl_dump_hash(dl_hash *h)
{
    dl_node *node;
    int i;
    for(i = 0; i < h->slot_total; i++){
        node = &h->node[i];

        if(node->v.tt == DL_H_NULL){
            printf("slot:%d, empty\n", i);
        }else{
            printf("slot:%d\n", i);

            while(node){
                dl_printf("  key:'%V' value:", node->k.key);
                
                dl_dump_hash_v(&node->v);
                puts("");
                
                node = node->k.next;
            }

        }

        puts("");
    }
}
