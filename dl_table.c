#include "dl_base.h"

static dl_value_l *dl_find_l(dl_table *t, dl_key_l *key);
static dl_value_l *dl_set_l(dl_table *t, dl_key_l *key);
static dl_value_l *dl_set_num_l(dl_table *t, long n);
static dl_node_l *hashnum (const dl_table *t, long n);

static dl_node_l *main_pos(dl_table *t, dl_key_l *key);
static dl_value_l *dl_newkey_l(dl_table *t, dl_key_l *key);
static dl_table *dl_table_init(dl_table *t, int narray, int nhash, uchar free);
static void dl_rehash(dl_table *t, dl_key_l *key);

static const dl_node_l _dummynode = {
  {{0}, 0},         /* value */
  {{0}, 0, 0, NULL}    /* key */
};

#define dummynode		(&_dummynode)

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
    if(key->tt == DL_INT_L){
        return dl_find_by_index(t, key->k.n);
    }
    
    return NULL;
}

dl_value_l *dl_find_by_index(dl_table *t, long index)
{
    dl_value_l      *v;
    dl_node_l       *node;
    
    
    if(index < t->sizearray){
        v = &t->array[index];
    }else{
        node = hashnum(t, index);
        
        do{
            if(node->i_key.tt == DL_INT_L && node->i_key.k.n == index){
                v = &node->i_val;
                break;
            }
            
            node = node->i_key.next;
        }while(node);
        
        v = NULL;
    }
    
    return v;
}

static dl_node_l *main_pos(dl_table *t, dl_key_l *key){
    if(key->tt == DL_INT_L){
        return hashnum(t, key->k.n);
    }
}

static dl_value_l *dl_newkey_l(dl_table *t, dl_key_l *key){
    dl_node_l       *mp;
    dl_node_l       *n, *othern;

    mp = main_pos(t, key);
    
    /* not used or not initiated */
    if(mp->i_val.tt == DL_NIL_L || mp == dummynode){
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
    mp->i_key = *key;
    
    return &mp->i_val;
}

int dl_array_set_int_l(dl_table *t, long index, long value){
    dl_value_l      *v;
    dl_node_l       *node;
    long            nk;
    
    dl_key_l        key;
    
    key.tt = DL_INT_L;
    key.k.n = index;
    
    if(index < 0) return -1;
    
    v = dl_find_by_index(t, index);
    
    if(v == NULL){
        v = dl_newkey_l(t, &key);
    }
    
    v->v.n = value;
    v->tt = DL_INT_L;

    return 0;
}

dl_table *dl_table_new(dl_pool *pool,int narray, int nhash, uchar free)
{
    dl_table *t = dl_palloc(pool, sizeof(dl_table));
    t->free = free ? 1 : 0;
    t->pool = pool;
    
    return dl_table_init(t, narray, nhash, t->free);
}

static dl_table *dl_table_init(dl_table *t, int narray, int nhash, uchar free)
{
    dl_pool     *pool;
    int         i;
    
    if(free){
        pool = dl_create_pool(1024, NULL);
        pool->log = t->pool->log;
        t->pool = pool;
    }else{
        pool = t->pool;
    }
    
    t->array = NULL;
    t->sizearray = 0;
    t->lsizenode = 0;
    t->node = (dl_node_l *)dummynode;
    t->lastfree = NULL;
    
    if(narray > 0){
        t->array = dl_palloc(pool, narray*sizeof(dl_value_l));
        t->sizearray = narray;
        
        for(i = 0; i < narray; i++){
            t->array[i].tt = DL_NIL_L;
        }
    }
    
    if(nhash > 0){
        int         size, lsize;
        dl_node_l   *node;
        
        lsize = ceillog2(nhash);
        if(lsize > MAXBITS){
            return NULL;
        }
        
        size = 1 << lsize;
        t->node = dl_palloc(pool, size * sizeof(dl_node_l));
        
        for(i = 0; i < nhash; i++){
            node = &t->node[i];
            node->i_key.next = NULL;
            node->i_key.k.n = DL_NIL_L;
            
            node->i_val.tt = DL_NIL_L;
        }
        
        t->lastfree = &t->node[size];
        t->lsizenode = (uchar)lsize;
    }
    
    return t;
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
      if (t->array[i-1].tt != DL_NIL_L)
        lc++;
    }
    nums[lg] += lc;
    ause += lc;
  }
  return ause;
}

static long arrayindex (dl_key_l *key) {
  if (key->tt == DL_INT_L) {
    return key->k.n;
  }
  return -1;  /* `key' did not match some condition */
}

static int countint (dl_key_l *key, int *nums) {
  long k = arrayindex(key);
  if (0 < k && k <= MAXASIZE) {  /* is `key' an appropriate array index? */
    nums[ceillog2(k)]++;  /* count as such */
    return 1;
  }else
    return 0;
}

static int numusehash (dl_table *t, int *nums, int *pnasize) {
  int totaluse = 0;  /* total number of elements */
  int ause = 0;  /* summation of `nums' */
  int i = 1 << t->lsizenode;
  while (i--) {
    dl_node_l *n = &t->node[i];
    if (n->i_val.tt != DL_NIL_L){
      ause += countint(&n->i_key, nums);
      totaluse++;
    }
  }
  *pnasize += ause;
  return totaluse;
}

static int setarrayvector (dl_table *t, int size) {
    int i;
    
    if(size > 0){
        t->array = dl_palloc(t->pool, size * sizeof(dl_value_l));
        if(t->array == NULL){
            return -1;
        }
        
        for(i = 0; i < size; i++){
            t->array[i].tt = DL_NIL_L;
        }
    }
    
    t->sizearray = size;
    
    return 0;
}


static int setnodevector (dl_table *t, int size) {
  int lsize;
  if (size == 0) {  /* no elements to hash part? */
    t->node = (dl_node_l *)dummynode;  /* use common `dummynode' */
    lsize = 0;
  }
  else {
    int i;
    lsize = ceillog2(size);
    if (lsize > MAXBITS){
        dl_log_error(DL_LOG_ERR, t->pool->log, "table overflow");
        //luaG_runerror(L, "table overflow");
        return -1;
    }
    size = 1 << lsize;
    
    t->node = dl_palloc(t->pool, size * sizeof(dl_node_l));
    
    dl_node_l *node;
    for(i = 0; i < size; i++){
        node = &t->node[i];
        node->i_key.next = NULL;
        node->i_key.tt = DL_NIL_L;
        
        node->i_val.tt = DL_NIL_L;
    }
    
  }
  t->lsizenode = (uchar)lsize;
  t->lastfree = &t->node[size];  /* all positions are free */
  
  return 0;
}

static void resize (dl_table *t, int nasize, int nhsize) {
    int i;
    int oldasize = t->sizearray;
    int oldhsize = t->lsizenode;
    dl_node_l *node = t->node;  /* save old hash ... */
  
    if(t->free){
        dl_log *log = t->pool->log;
        dl_destroy_pool(t->pool);
        t->pool = dl_create_pool(2048, NULL);
        t->pool->log = log;
    }
  
  if (nasize > oldasize)  /* array part must grow? */
    setarrayvector(t, nasize);
  /* create new hash part with appropriate size */
  setnodevector(t, nhsize);  
  
  if (nasize < oldasize) {  /* array part must shrink? */
    t->sizearray = nasize;
    /* re-insert elements from vanishing slice */
    for (i=nasize; i<oldasize; i++) {
      if (t->array[i].tt != DL_NIL_L){
          dl_value_l *v = dl_set_num_l(t, i+1);
          *v = t->array[i];
          //setobjt2t(L, luaH_setnum(L, t, i+1), &t->array[i]);
      }
        
    }
    /* shrink array */
    //luaM_reallocvector(L, t->array, oldasize, nasize, TValue);
  }
  /* re-insert elements from hash part */
  for (i = (1 << oldhsize) - 1; i >= 0; i--) {
    dl_node_l *old = node+i;
    
    if(old->i_val.tt != DL_NIL_L){
        dl_value_l *v = dl_set_l(t, &old->i_key);
        *v = old->i_val;
        //setobjt2t(L, luaH_set(L, t, key2tval(old)), gval(old));
    }
      
  }
  //if (node != dummynode)
    //luaM_freearray(L, node, twoto(oldhsize), Node);  /* free old array */
}

static void dl_rehash(dl_table *t, dl_key_l *ek){
    int nasize, na, i, totaluse;
    int nums[MAXBITS+1];  /* nums[i] = number of keys between 2^(i-1) and 2^i */

    for (i=0; i<=MAXBITS; i++) nums[i] = 0;  /* reset counts */
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