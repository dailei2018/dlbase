#include "dl_base.h"

static dl_value_l *dl_find_l(dl_table *t, dl_key_l *key);
static dl_value_l *dl_set_l(dl_table *t, dl_key_l *key);
static dl_value_l *dl_set_num_l(dl_table *t, long n);
static dl_node_l *hashnum (const dl_table *t, long n);

static dl_node_l *main_pos(dl_table *t, dl_key_l *key);
static dl_value_l *dl_newkey_l(dl_table *t, dl_key_l *key);
static dl_table *dl_table_init(dl_table *t, int narray, int nhash);
static void dl_rehash(dl_table *t, dl_key_l *key);

static int dl_array_init_l(dl_table *t, int size);
static int dl_node_init_l(dl_table *t, int size);

static dl_value_l * dl_array_set(dl_table *t, long index, dl_value_l **vv);
static dl_value_l * dl_node_set(dl_table *t, char *str, size_t len, dl_value_l **vv);


static const dl_node_l _dummynode = {
  {{0}, 0},         /* value */
  {{0}, 0, 0, NULL}    /* key */
};

#define dummynode		(&_dummynode)

void dl_table_destroy(dl_table *t){
    if(t->arr_p != NULL){
        dl_destroy_pool(t->arr_p);
    }
    
    if(t->node_p != NULL){
        dl_destroy_pool(t->node_p);
    }
    
}

dl_table *dl_table_new(dl_pool *pool, int narray, int nhash)
{
    dl_table *t = dl_palloc(pool, sizeof(dl_table));
    t->pool = pool;
    t->arr_p = NULL;
    t->node_p = NULL;
    t->err = 0;
    
    return dl_table_init(t, narray, nhash);
}

static dl_table *dl_table_init(dl_table *t, int narray, int nhash)
{
    dl_pool     *pool;
    int         i,s;

    pool = t->pool;
    
    t->array = NULL;
    t->sizearray = 0;
    t->lsizenode = 0;
    t->node = (dl_node_l *)dummynode;
    t->lastfree = NULL;
    
    if(dl_array_init_l(t, narray) != 0) return NULL;
    
    if(dl_node_init_l(t, nhash) != 0) return NULL;
    
    return t;
}

/* array ------------------------*/

/* int */
dl_value_l * dl_try_array_set_int_l(dl_table *t, long index, long value){
    dl_value_l      *v, *ret_v;
    
    ret_v = dl_array_set(t, index, &v);
    
    if(ret_v == NULL){
        v->v.n = value;
        v->tt = DL_INT_L;
    }
    
    return ret_v;
}

int dl_array_set_int_l(dl_table *t, dl_value_l *v, long value){
    
    v->v.n = value;
    v->tt = DL_INT_L;

    return 0;
}

static dl_value_l * dl_array_set(dl_table *t, long index, dl_value_l **vv)
{
    dl_value_l      *v, *ret_v;
    dl_node_l       *node;
    
    dl_key_l        key;
    
    key.tt = DL_INT_L;
    key.k.n = index;
    
    v = dl_find_by_index(t, index);
    ret_v = v;
    
    if(v == NULL){
        v = dl_newkey_l(t, &key);
        
    }else{
        
        if(v->tt & DL_STR_L){
            dl_free(v->v.s->data);
            dl_free(v->v.s);
            
            ret_v = NULL;
        }else if(v->tt & DL_INT_L){
            ret_v = NULL;
        }
        
    }
    
    *vv = v;
    
    /* if not NULL, means you should deal with memory deallocation manually */
    return ret_v;
}

/* string */
dl_value_l * dl_try_array_set_str_l(dl_table *t, long index, char *str, size_t len){
    dl_value_l      *v, *ret_v;
    
    ret_v = dl_array_set(t, index, &v);
    
    if(ret_v == NULL){
        
        if(dl_array_set_str_l(t, v, str, len) != 0){
            t->err = 1;
        }
        
    }
    
    return ret_v;
}

int dl_array_set_str_l(dl_table *t, dl_value_l *v, char *str, size_t len){
    
    v->v.s = dl_alloc(sizeof(dl_str), t->pool->log);
    if(v->v.s == NULL){
        t->err = 1;
        return -1;
    }
    
    v->v.s->data = dl_alloc(len, t->pool->log);
    if(v->v.s->data == NULL){
        t->err = 1;
        return -1;
    }
    v->v.s->len = len;
    
    memcpy(v->v.s->data, str, len);
    v->tt = DL_STR_L;

    return 0;
}

/* void */
dl_value_l * dl_try_array_set_void_l(dl_table *t, long index, void *value){
    dl_value_l      *v, *ret_v;
    
    ret_v = dl_array_set(t, index, &v);
    
    if(ret_v == NULL){
        
        dl_array_set_void_l(t, v, value);
        
    }
    
    return ret_v;
}

int dl_array_set_void_l(dl_table *t, dl_value_l *v, void *value){
    
    v->v.v = value;
    v->tt = DL_VOID_L;

    return 0;
}


/* node ------------------------------- */
dl_value_l * dl_try_node_set_int_l(dl_table *t, char *str, size_t len, long value){
    dl_value_l      *v, *ret_v;
    
    ret_v = dl_node_set(t, str, len, &v);
    
    if(t->err) return NULL;
    
    if(ret_v == NULL){
        dl_node_set_int_l(t, v, value);
    }
    
    return ret_v;
}

int dl_node_set_int_l(dl_table *t, dl_value_l *v, long value){
    
    v->v.n = value;
    v->tt = DL_INT_L;

    return 0;
}


/* string */
dl_value_l * dl_try_node_set_str_l(dl_table *t, char *str, size_t len, char *v_str, size_t v_len){
    dl_value_l      *v, *ret_v;
    
    ret_v = dl_node_set(t, str, len, &v);
    
    if(ret_v == NULL){
        
        if(dl_node_set_str_l(t, v, v_str, v_len) != 0){
            t->err = 1;
        }
        
    }
    
    return ret_v;
}

int dl_node_set_str_l(dl_table *t, dl_value_l *v, char *str, size_t len){
    
    v->v.s = dl_alloc(sizeof(dl_str), t->pool->log);
    if(v->v.s == NULL){
        t->err = 1;
        return -1;
    }
    
    v->v.s->data = dl_alloc(len, t->pool->log);
    if(v->v.s->data == NULL){
        t->err = 1;
        return -1;
    }
    v->v.s->len = len;
    
    memcpy(v->v.s->data, str, len);
    v->tt = DL_STR_L;

    return 0;
}


/* void */
dl_value_l * dl_try_node_set_void_l(dl_table *t, char *str, size_t len, void *value){
    dl_value_l      *v, *ret_v;
    
    ret_v = dl_node_set(t, str, len, &v);
    
    if(ret_v == NULL){
        dl_node_set_void_l(t, v, value);
    }
    
    return ret_v;
}

int dl_node_set_void_l(dl_table *t, dl_value_l *v, void *value){
    
    v->v.v = value;
    v->tt = DL_VOID_L;

    return 0;
}


static dl_value_l * dl_node_set(dl_table *t, char *str, size_t len, dl_value_l **vv)
{
    dl_value_l      *v, *ret_v;
    dl_node_l       *node;
    
    dl_key_l        key;
    dl_str          ss;
    
    key.tt = DL_STR_L;
    key.k.s = &ss;
    ss.data = str;
    ss.len = len;
    
    v = dl_find_by_str(t, str, len);
    ret_v = v;
    
    if(v == NULL){
        
        key.k.s = dl_alloc(sizeof(dl_str), t->pool->log);
        if(key.k.s == NULL){
            t->err = 1;
            return NULL;
        }
        key.k.s->len = len;
        key.k.s->data = dl_alloc(len, t->pool->log);
        if(key.k.s->data == NULL){
            t->err = 1;
            return NULL;
        }
        
        memcpy(key.k.s->data, str, len);
        
        v = dl_newkey_l(t, &key);
        
    }else{
        
        if(v->tt & DL_STR_L){
            dl_free(v->v.s->data);
            dl_free(v->v.s);
            
            ret_v = NULL;
        }else if(v->tt & DL_INT_L){
            ret_v = NULL;
        }
        
    }
    
    *vv = v;
    
    /* if not NULL, means you should deal with memory deallocation manually */
    return ret_v;
}



/* get free node */
static inline
dl_node_l *getfreepos(dl_table *t)
{
    if(t->lastfree == NULL) return NULL;
    
    while(t->lastfree-- > t->node) {
        if (t->lastfree->i_key.k.n == DL_NIL_L)
            return t->lastfree;
    }
    return NULL;  /* could not find a free place */
}

static dl_node_l *hashnum (const dl_table *t, long n) {
    uint    a[numints];
    int     i;
  
    if(n == 0){
        return &t->node[0];
    }
  
    memcpy(a, &n, sizeof(a));
    for (i = 1; i < numints; i++){
        a[0] += a[i];
    }
    
    i = a[0] % (1 << t->lsizenode);
    
    return &t->node[i];
}

static dl_value_l *dl_set_l(dl_table *t, dl_key_l *key){
    dl_value_l      *v;
    
    v = dl_find_l(t, key);
    
    if(v != NULL){
        return v;
    }
    
    return dl_newkey_l(t, key);
}

static dl_value_l *dl_set_num_l(dl_table *t, long n){
    dl_key_l    key;
    
    key.tt = DL_INT_L;
    key.k.n = n;
    
    return dl_set_l(t, &key);
}

static dl_value_l *dl_find_l(dl_table *t, dl_key_l *key)
{
    if(key->tt & DL_INT_L){
        return dl_find_by_index(t, key->k.n);
    }
    
    return NULL;
}

dl_value_l *dl_find_by_index(dl_table *t, long index)
{
    dl_value_l      *v;
    dl_node_l       *node;
    
    
    if(index < t->sizearray){
        return &t->array[index];
    }else{
        node = hashnum(t, index);
        
        do{
            if( (node->i_key.tt & DL_INT_L) && node->i_key.k.n == index){
                return &node->i_val;
            }
            
            node = node->i_key.next;
        }while(node);
        
    }
    
    return NULL;
}

dl_value_l *dl_find_by_str(dl_table *t, char *str, size_t len)
{
    dl_value_l  *v;
    dl_node_l   *node;
    uint        h;
    
    h = dl_hash_key((uchar *)str, len);
    node = &t->node[h % (1 << t->lsizenode)];
    
    do{
        if(node->i_key.tt == DL_STR_L && memcmp(node->i_key.k.s->data, str, len) == 0 ){
            return &node->i_val;
        }
        
        node = node->i_key.next;
    }while(node);
    
    return NULL;
}


static dl_node_l *main_pos(dl_table *t, dl_key_l *key){
    if(key->tt & DL_INT_L){
        return hashnum(t, key->k.n);
    }
    
    uint h = dl_hash_key((uchar *)key->k.s->data, key->k.s->len);
    
    return &t->node[h % (1 << t->lsizenode)];
    
}

static dl_value_l *dl_newkey_l(dl_table *t, dl_key_l *key){
    dl_node_l       *mp;
    dl_node_l       *n, *othern;

    mp = main_pos(t, key);
    
    /* not used or not initiated */
    if(mp->i_val.tt != DL_NIL_L || mp == dummynode){
        n = getfreepos(t);
        if(n == NULL){
            dl_rehash(t, key);
            
            return dl_set_l(t, key);
        }
        
        othern = main_pos(t, &mp->i_key);
        
        if(mp != othern){
            /* yes, not in the right place, move it to another place.
               if still not in the right place, may will be moved again later.*/
            
            /* find previous node */
            while(othern->i_key.next != mp)
                othern = othern->i_key.next;
            
            othern->i_key.next = n;
            *n = *mp;
            mp->i_key.next = NULL;
            mp->i_val.tt = DL_NIL_L;
            
        }else{
            /* in the right place, just chain */
            n->i_key.next = mp->i_key.next;
            mp->i_key.next = n;
            mp = n;
        }
        
    }
    
    /* now mp is a free node */
    mp->i_key.tt = key->tt;
    
    if(key->tt & DL_INT_L){
        mp->i_key.k.n = key->k.n;
    }else{
        /* string */
        mp->i_key.k.s = key->k.s;
    }
    
    return &mp->i_val;
}

static int computesizes (int nums[], int *narray) {
  int i;
  int twotoi;  /* 2^i */
  int a = 0;  /* number of elements smaller than 2^i */
  int na = 0;  /* number of elements to go to array part */
  int n = 0;  /* optimal size for array part */
  for (i = 0, twotoi = 1; twotoi/2 < *narray; i++, twotoi *= 2) {
    if (nums[i] > 0) {
      a += nums[i];
      if (a > twotoi/2) {  /* more than half elements present? */
        n = twotoi;  /* optimal size (till now) */
        na = a;  /* all elements smaller than n will go to array part */
      }
    }
    if (a == *narray) break;  /* all elements already counted */
  }
  *narray = n;
  //lua_assert(*narray/2 <= na && na <= *narray);
  return na;
}

static int numusearray (dl_table *t, int *nums) {
  int lg;
  int ttlg;  /* 2^lg */
  int ause = 0;  /* summation of `nums' */
  int i = 1;  /* count to traverse all array keys */
  for (lg=0, ttlg=1; lg<=MAXBITS; lg++, ttlg*=2) {  /* for each slice */
    int lc = 0;  /* counter */
    int lim = ttlg;
    if (lim > t->sizearray) {
      lim = t->sizearray;  /* adjust upper limit */
      if (i > lim)
        break;  /* no more elements to count */
    }
    /* count elements in range (2^(lg-1), 2^lg] */
    for (; i <= lim; i++) {
      if (!(t->array[i-1].tt & DL_NIL_L))
        lc++;
    }
    nums[lg] += lc;
    ause += lc;
  }
  return ause;
}

static int countint(dl_key_l *key, int *nums){
    long k;
  
    if(key->tt & DL_INT_L){
        k = key->k.n;
        if (k > 0 && k <= MAXASIZE){
            nums[ceillog2(k)]++;
            return 1;
        }
    }
  
    return 0;
}

static int numusehash (dl_table *t, int *nums, int *pnasize) {
    int totaluse = 0;  /* total number of elements */
    int ause = 0;  /* summation of `nums' */
    int i = 1 << t->lsizenode;
  
    while(i--) {
        dl_node_l *n = &t->node[i];
        if (!(n->i_val.tt & DL_NIL_L)){
            ause += countint(&n->i_key, nums);
            totaluse++;
        }
    }
    
    *pnasize += ause;
    
    return totaluse;
}

static int dl_array_init_l (dl_table *t, int size) {
    int i;
    
    if(size > 0){
        
        t->arr_p = dl_create_pool(1024, t->pool->log);
        if(t->arr_p == NULL) return -1;
        
        t->array = dl_palloc(t->arr_p, size * sizeof(dl_value_l));
        if(t->array == NULL){
            return -1;
        }
        
        /* initiate */
        for(i = 0; i < size; i++){
            t->array[i].tt = DL_NIL_L;
            t->array[i].v.n = 0;
        }
        
    }
    
    t->sizearray = size;
    
    return 0;
}


static int dl_node_init_l (dl_table *t, int size) {
    int lsize, i;
    
    if (size == 0) {  /* no elements to hash part? */
        t->node = (dl_node_l *)dummynode;  /* use common `dummynode' */
        lsize = 0;
        t->lastfree = NULL;
    }else{
        int i;
        lsize = ceillog2(size);
        if (lsize > MAXBITS){
            dl_log_error(DL_LOG_ERR, t->pool->log, "table overflow");
            //luaG_runerror(L, "table overflow");
            return -1;
        }
        size = 1 << lsize;
        
        t->node_p = dl_create_pool(1024, t->pool->log);
        if(t->node_p == NULL) return -1;
        
        t->node = dl_palloc(t->node_p, size * sizeof(dl_node_l));
    
        dl_node_l *node;
        for(i = 0; i < size; i++){
            node = &t->node[i];
            node->i_key.next = NULL;
            node->i_key.tt = DL_NIL_L;
        
            node->i_val.tt = DL_NIL_L;
        }
        
        t->lastfree = &t->node[size];
    }
    
    t->lsizenode = (uchar)lsize;
  
    return 0;
}

static int resize (dl_table *t, int nasize, int nhsize) {
    int i;
    int oldasize, oldhsize;
    
    dl_pool         *old_arr_p;
    dl_value_l      *old_arr;
    
    dl_pool         *old_node_p;
    dl_node_l       *old_node;
    
    /* save old array */
    oldasize = t->sizearray;
    old_arr_p = t->arr_p;
    old_arr = t->array;
    
    /* save old hash */
    oldhsize = t->lsizenode;
    old_node_p = t->node_p;
    old_node = t->node;  /* save old hash ... */
    
    //dl_log_error(DL_LOG_OUT, t->pool->log, "array:%d  hash:%d\n", oldasize, 1 << oldhsize);
    //dl_log_error(DL_LOG_OUT, t->pool->log, "array:%d  hash:%d\n\n", nasize, nhsize);
    
    if (nasize > oldasize){
        dl_array_init_l(t, nasize);
        
        for(i = 0; i < oldasize; i++){
            t->array[i] = old_arr[i];
        }
        
    }
    
    /* initiate new hash */
    dl_node_init_l(t, nhsize);  
  
    if (nasize < oldasize) {
        t->sizearray = nasize;

        for (i = nasize; i < oldasize; i++){
            
            if (old_arr[i].tt != DL_NIL_L){
                dl_value_l *v = dl_set_num_l(t, i);
                *v = old_arr[i];

            }
        
        }

    }
    /* re-insert old elements to new node */
    for (i = (1 << oldhsize) - 1; i >= 0; i--) {
        dl_node_l *old = old_node + i;
    
        if(!(old->i_val.tt & DL_NIL_L)){
            dl_value_l *v = dl_set_l(t, &old->i_key);
            *v = old->i_val;
        }
      
    }
    
    if(old_arr_p) dl_destroy_pool(old_arr_p);
    
    if(old_node_p) dl_destroy_pool(old_node_p);

}

static void dl_rehash(dl_table *t, dl_key_l *ek){
    int na, i;
    
    int nasize;     // integer keys appropriate array index, nasize <= totaluse
    int totaluse;   // total integer keys
    int nums[MAXBITS+1];  /* nums[i] = number of keys between 2^(i-1) and 2^i */

    for (i=0; i<=MAXBITS; i++)
        nums[i] = 0;  /* reset counts */
    
    nasize = numusearray(t, nums);  /* count keys in array part */
    totaluse = nasize;  /* all those keys are integer keys */
    
    totaluse += numusehash(t, nums, &nasize);  /* count keys in hash part */
    
    /* count extra key */
    nasize += countint(ek, nums);
    totaluse++;
    
    /* compute new size for array part */
    na = computesizes(nums, &nasize);
    
    /* resize the table to new computed sizes */
    resize(t, nasize, totaluse - na);
}


