#include "dlc_base.h"
#include "dlc_des_internal.h"

static void des_key_schedule (const char * rawkey, uint32_t * subkey);

/*
 * des_key_schedule():	  Calculate 16 subkeys pairs (even/odd) for
 *			  16 encryption rounds.
 *			  To calculate subkeys for decryption the caller
 *			  have to reorder the generated subkeys.
 *
 *    rawkey:	    8 Bytes of key data
 *    subkey:	    Array of at least 32 u32s. Will be filled
 *		    with calculated subkeys.
 *
 */
static void
des_key_schedule (const char * rawkey, uint32_t * subkey)
{
  uint32_t  left, right, work;
  int       round;

  READ_64BIT_DATA (rawkey, left, right)

  DO_PERMUTATION (right, work, left, 4, 0x0f0f0f0f)
  DO_PERMUTATION (right, work, left, 0, 0x10101010)

  left = ((leftkey_swap[(left >> 0) & 0xf] << 3)
          | (leftkey_swap[(left >> 8) & 0xf] << 2)
          | (leftkey_swap[(left >> 16) & 0xf] << 1)
          | (leftkey_swap[(left >> 24) & 0xf])
          | (leftkey_swap[(left >> 5) & 0xf] << 7)
          | (leftkey_swap[(left >> 13) & 0xf] << 6)
          | (leftkey_swap[(left >> 21) & 0xf] << 5)
          | (leftkey_swap[(left >> 29) & 0xf] << 4));

  left &= 0x0fffffff;

  right = ((rightkey_swap[(right >> 1) & 0xf] << 3)
           | (rightkey_swap[(right >> 9) & 0xf] << 2)
           | (rightkey_swap[(right >> 17) & 0xf] << 1)
           | (rightkey_swap[(right >> 25) & 0xf])
           | (rightkey_swap[(right >> 4) & 0xf] << 7)
           | (rightkey_swap[(right >> 12) & 0xf] << 6)
           | (rightkey_swap[(right >> 20) & 0xf] << 5)
           | (rightkey_swap[(right >> 28) & 0xf] << 4));

  right &= 0x0fffffff;

  for (round = 0; round < 16; ++round)
    {
      left = ((left << encrypt_rotate_tab[round])
              | (left >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;
      right = ((right << encrypt_rotate_tab[round])
               | (right >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;

      *subkey++ = (((left << 4) & 0x24000000)
                   | ((left << 28) & 0x10000000)
                   | ((left << 14) & 0x08000000)
                   | ((left << 18) & 0x02080000)
                   | ((left << 6) & 0x01000000)
                   | ((left << 9) & 0x00200000)
                   | ((left >> 1) & 0x00100000)
                   | ((left << 10) & 0x00040000)
                   | ((left << 2) & 0x00020000)
                   | ((left >> 10) & 0x00010000)
                   | ((right >> 13) & 0x00002000)
                   | ((right >> 4) & 0x00001000)
                   | ((right << 6) & 0x00000800)
                   | ((right >> 1) & 0x00000400)
                   | ((right >> 14) & 0x00000200)
                   | (right & 0x00000100)
                   | ((right >> 5) & 0x00000020)
                   | ((right >> 10) & 0x00000010)
                   | ((right >> 3) & 0x00000008)
                   | ((right >> 18) & 0x00000004)
                   | ((right >> 26) & 0x00000002)
                   | ((right >> 24) & 0x00000001));

      *subkey++ = (((left << 15) & 0x20000000)
                   | ((left << 17) & 0x10000000)
                   | ((left << 10) & 0x08000000)
                   | ((left << 22) & 0x04000000)
                   | ((left >> 2) & 0x02000000)
                   | ((left << 1) & 0x01000000)
                   | ((left << 16) & 0x00200000)
                   | ((left << 11) & 0x00100000)
                   | ((left << 3) & 0x00080000)
                   | ((left >> 6) & 0x00040000)
                   | ((left << 15) & 0x00020000)
                   | ((left >> 4) & 0x00010000)
                   | ((right >> 2) & 0x00002000)
                   | ((right << 8) & 0x00001000)
                   | ((right >> 14) & 0x00000808)
                   | ((right >> 9) & 0x00000400)
                   | ((right) & 0x00000200)
                   | ((right << 7) & 0x00000100)
                   | ((right >> 7) & 0x00000020)
                   | ((right >> 3) & 0x00000011)
                   | ((right << 2) & 0x00000004)
                   | ((right >> 21) & 0x00000002));
    }
}



int dlc_des_setkey (des_ctx *ctx, const char * key)
{
    int i;

    des_key_schedule(key, ctx->encrypt_subkeys);

    for(i=0; i<32; i+=2){
        ctx->decrypt_subkeys[i]	= ctx->encrypt_subkeys[30-i];
        ctx->decrypt_subkeys[i+1] = ctx->encrypt_subkeys[31-i];
    }

    return 0;
}

/*
 * Electronic Codebook Mode DES encryption/decryption of data according
 * to 'mode'.
 */
int
dlc_des_crypt(des_ctx *ctx, const char * from, char * to, int mode)
{
  uint32_t  left, right, work;
  uint32_t  *keys;

  keys = mode ? ctx->decrypt_subkeys : ctx->encrypt_subkeys;

  READ_64BIT_DATA (from, left, right)
  INITIAL_PERMUTATION (left, work, right)

  DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) DES_ROUND (left, right, work, keys)

  FINAL_PERMUTATION (right, work, left)
  WRITE_64BIT_DATA (to, right, left)

  return 0;
}


dl_str * _dlc_des_ecb_encrypt(void *ctx, char * src, int slen, char * to, int triple)
{
    dlc_des_crypt_func      enc_func;
    int         r,plen,total,nblock,i;
    char        *cur, padding[8], *pad_cur;
    dl_str      *ss;
    
    if(!triple)
        enc_func = (dlc_des_crypt_func)dlc_des_crypt;
    else
        enc_func = (dlc_des_crypt_func)dlc_3des_crypt;
    

    r = slen % DLC_DES_BLOCK_SIZE;
    if(r == 0) plen = 0;
    else plen = DLC_DES_BLOCK_SIZE - r;
    
    total = (slen / DLC_DES_BLOCK_SIZE + 1) * DLC_DES_BLOCK_SIZE;
    
    nblock = slen / DLC_DES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        enc_func(ctx, src, cur, 0);
        src += DLC_DES_BLOCK_SIZE;
        cur  += DLC_DES_BLOCK_SIZE;
    }
    
    pad_cur = padding;
    if(plen == 0){
        plen = 8;
    }else{
        pad_cur = dl_cpymem(pad_cur, src, 8 - plen);
    }
    
    /* pkcs#7 padding*/
    memset(pad_cur, (uchar)plen, plen);
    enc_func(ctx, padding, cur, 0);
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    return ss;
}


dl_str * _dlc_des_ecb_decrypt(void *ctx, char * src, int slen, char * to, int triple)
{
    dlc_des_crypt_func      enc_func;
    int         r,plen,total,nblock,i;
    char        *cur, padding[8];
    dl_str      *ss;
    
    if(!triple)
        enc_func = (dlc_des_crypt_func)dlc_des_crypt;
    else
        enc_func = (dlc_des_crypt_func)dlc_3des_crypt;
    
    total = slen;
        
    nblock = total / 8;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        enc_func(ctx, src, cur, 1);
        src += DLC_DES_BLOCK_SIZE;
        cur  += DLC_DES_BLOCK_SIZE;
    }
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    /* pkcs#7 unpadding */
    uchar b = to[total - 1];
    ss->len -= b;
    
    return ss;
}


void dlc_des_set_vector(void *ctx, char *vi)
{
    memcpy(((des_ctx *)ctx)->vi, vi, 8);
}

dl_str * _dlc_des_cbc_encrypt(void *ctx, char * src, int slen, char * to, int triple)
{
    dlc_des_crypt_func      enc_func;
    int         r,plen,total,nblock,i;
    char        *cur, *vi, padding[8], *pad_cur;
    dl_str      *ss;
    des_ctx     *d_ctx;
    
    uint64_t    *u1, *u2, u3;
    
    d_ctx = (des_ctx *)ctx;
    vi = d_ctx->vi;
    
    if(!triple)
        enc_func = (dlc_des_crypt_func)dlc_des_crypt;
    else
        enc_func = (dlc_des_crypt_func)dlc_3des_crypt;
    
    r = slen % DLC_DES_BLOCK_SIZE;
    if(r == 0) plen = 0;
    else plen = DLC_DES_BLOCK_SIZE - r;
    
    total = (slen / DLC_DES_BLOCK_SIZE + 1) * DLC_DES_BLOCK_SIZE;
    nblock = slen / DLC_DES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        u1 = (uint64_t *)vi;
        u2 = (uint64_t *)src;
        
        u3 = *u1 ^ *u2;
        
        enc_func(ctx, (char *)(&u3), cur, 0);
        
        vi = cur;
        
        src += DLC_DES_BLOCK_SIZE;
        cur  += DLC_DES_BLOCK_SIZE;
    }
    
    pad_cur = padding;
    if(plen == 0){
        plen = 8;
    }else{
        pad_cur = dl_cpymem(pad_cur, src, 8 - plen);
    }
    
    /* pkcs#7 padding*/
    memset(pad_cur, (uchar)plen, plen);
    u1 = (uint64_t *)vi;
    u2 = (uint64_t *)padding;
    u3 = *u1 ^ *u2;
    enc_func(ctx, (char *)(&u3), cur, 0);
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    
    return ss;
}


dl_str * _dlc_des_cbc_decrypt(void *ctx, char * src, int slen, char * to, int triple)
{
    dlc_des_crypt_func      enc_func;
    int         r,plen,total,nblock,i;
    char        *cur, *vi;
    dl_str      *ss;
    des_ctx     *d_ctx;
    
    uint64_t    *u1, *u2, *u3;
    
    d_ctx = (des_ctx *)ctx;
    vi = d_ctx->vi;
    
    if(!triple)
        enc_func = (dlc_des_crypt_func)dlc_des_crypt;
    else
        enc_func = (dlc_des_crypt_func)dlc_3des_crypt;
    
    total = slen;
    nblock = total / 8;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        u1 = (uint64_t *)vi;
        
        enc_func(ctx, src, cur, 1);
        
        u2 = (uint64_t *)cur;
        u3 = u2;
        
        *u3 = *u1 ^ *u2;
        
        vi = src;
        
        src += DLC_DES_BLOCK_SIZE;
        cur  += DLC_DES_BLOCK_SIZE;
    }
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    /* pkcs#7 unpadding */
    uchar b = to[total - 1];
    ss->len -= b;
    
    return ss;
}



/* cfb */
dl_str * _dlc_des_cfb_encrypt(void *ctx, char * src, int slen, char * to, int triple)
{
    dlc_des_crypt_func      enc_func;
    int         r,plen,total,nblock,i;
    char        *cur, *vi, padding[8], *pad_cur;
    dl_str      *ss;
    des_ctx     *d_ctx;
    
    uint64_t    *u1, *u2, u3;
    
    d_ctx = (des_ctx *)ctx;
    vi = d_ctx->vi;
    
    if(!triple)
        enc_func = (dlc_des_crypt_func)dlc_des_crypt;
    else
        enc_func = (dlc_des_crypt_func)dlc_3des_crypt;
    
    r = slen % DLC_DES_BLOCK_SIZE;
    if(r == 0) plen = 0;
    else plen = DLC_DES_BLOCK_SIZE - r;
    
    total = (slen / DLC_DES_BLOCK_SIZE + 1) * DLC_DES_BLOCK_SIZE;
    nblock = slen / DLC_DES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        enc_func(ctx, vi, (char *)&u3, 0);
        
        u1 = (uint64_t *)src;
        u2 = (uint64_t *)cur;
        
        *u2 = u3 ^ *u1;
        
        vi = cur;
        
        src += DLC_DES_BLOCK_SIZE;
        cur  += DLC_DES_BLOCK_SIZE;
    }
    
    pad_cur = padding;
    if(plen == 0){
        plen = 8;
    }else{
        pad_cur = dl_cpymem(pad_cur, src, 8 - plen);
    }
    
    /* pkcs#7 padding*/
    memset(pad_cur, (uchar)plen, plen);
    enc_func(ctx, vi, (char *)&u3, 0);
        
    u1 = (uint64_t *)padding;
    u2 = (uint64_t *)cur;
    
    *u2 = u3 ^ *u1;
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    
    return ss;
}


dl_str * _dlc_des_cfb_decrypt(void *ctx, char * src, int slen, char * to, int triple)
{
    dlc_des_crypt_func      enc_func;
    int         r,plen,total,nblock,i;
    char        *cur, *vi;
    dl_str      *ss;
    des_ctx     *d_ctx;
    
    uint64_t    *u1, *u2, u3;
    
    d_ctx = (des_ctx *)ctx;
    vi = d_ctx->vi;
    
    if(!triple)
        enc_func = (dlc_des_crypt_func)dlc_des_crypt;
    else
        enc_func = (dlc_des_crypt_func)dlc_3des_crypt;
    
    total = slen;
    nblock = total / 8;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        enc_func(ctx, vi, (char *)&u3, 0);
        
        u1 = (uint64_t *)src;
        u2 = (uint64_t *)cur;
        
        *u2 = u3 ^ *u1;
        
        vi = src;
        
        src += DLC_DES_BLOCK_SIZE;
        cur  += DLC_DES_BLOCK_SIZE;
    }
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    /* pkcs#7 unpadding */
    uchar b = to[total - 1];
    ss->len -= b;
    
    return ss;
}



/* ofb */
dl_str * _dlc_des_ofb_encrypt(void *ctx, char * src, int slen, char * to, int triple)
{
    dlc_des_crypt_func      enc_func;
    int         r,plen,total,nblock,i;
    char        *cur, *vi, vi_mid[8], padding[8], *pad_cur;
    dl_str      *ss;
    des_ctx     *d_ctx;
    
    uint64_t    *u1, *u2, u3;
    
    d_ctx = (des_ctx *)ctx;
    vi = d_ctx->vi;
    
    if(!triple)
        enc_func = (dlc_des_crypt_func)dlc_des_crypt;
    else
        enc_func = (dlc_des_crypt_func)dlc_3des_crypt;
    
    r = slen % DLC_DES_BLOCK_SIZE;
    if(r == 0) plen = 0;
    else plen = DLC_DES_BLOCK_SIZE - r;
    
    total = (slen / DLC_DES_BLOCK_SIZE + 1) * DLC_DES_BLOCK_SIZE;
    nblock = slen / DLC_DES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        enc_func(ctx, vi, (char *)&u3, 0);
        
        u1 = (uint64_t *)src;
        u2 = (uint64_t *)cur;
        
        *u2 = u3 ^ *u1;
        
        memcpy(vi_mid, &u3, 8);
        vi = vi_mid;
        
        src += DLC_DES_BLOCK_SIZE;
        cur  += DLC_DES_BLOCK_SIZE;
    }
    
    pad_cur = padding;
    if(plen == 0){
        plen = 8;
    }else{
        pad_cur = dl_cpymem(pad_cur, src, 8 - plen);
    }
    
    /* pkcs#7 padding*/
    memset(pad_cur, (uchar)plen, plen);
    enc_func(ctx, vi, (char *)&u3, 0);
        
    u1 = (uint64_t *)padding;
    u2 = (uint64_t *)cur;
    
    *u2 = u3 ^ *u1;
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    
    return ss;
}


dl_str * _dlc_des_ofb_decrypt(void *ctx, char * src, int slen, char * to, int triple)
{
    dlc_des_crypt_func      enc_func;
    int         r,plen,total,nblock,i;
    char        *cur, *vi, vi_mid[8];
    dl_str      *ss;
    des_ctx     *d_ctx;
    
    uint64_t    *u1, *u2, u3;
    
    d_ctx = (des_ctx *)ctx;
    vi = d_ctx->vi;
    
    if(!triple)
        enc_func = (dlc_des_crypt_func)dlc_des_crypt;
    else
        enc_func = (dlc_des_crypt_func)dlc_3des_crypt;
    
    total = slen;
    nblock = total / 8;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        enc_func(ctx, vi, (char *)&u3, 0);
        
        u1 = (uint64_t *)src;
        u2 = (uint64_t *)cur;
        
        *u2 = u3 ^ *u1;
        
        memcpy(vi_mid, &u3, 8);
        vi = vi_mid;
        
        src += DLC_DES_BLOCK_SIZE;
        cur  += DLC_DES_BLOCK_SIZE;
    }
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    /* pkcs#7 unpadding */
    uchar b = to[total - 1];
    ss->len -= b;
    
    return ss;
}


/* crt */
dl_str * _dlc_des_crt_encrypt(void *ctx, char * src, int slen, char * to, int triple)
{
    dlc_des_crypt_func      enc_func;
    int         r,plen,total,nblock,i;
    char        *cur, vi_mid[8], padding[8], *pad_cur;
    dl_str      *ss;
    des_ctx     *d_ctx;
    
    uint64_t    *u1, *u2, u3, crt;
    
    d_ctx = (des_ctx *)ctx;
    crt = *(uint64_t *)d_ctx->vi;
    
    if(!triple)
        enc_func = (dlc_des_crypt_func)dlc_des_crypt;
    else
        enc_func = (dlc_des_crypt_func)dlc_3des_crypt;
    
    r = slen % DLC_DES_BLOCK_SIZE;
    if(r == 0) plen = 0;
    else plen = DLC_DES_BLOCK_SIZE - r;
    
    total = (slen / DLC_DES_BLOCK_SIZE + 1) * DLC_DES_BLOCK_SIZE;
    nblock = slen / DLC_DES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        enc_func(ctx, (char *)&crt, (char *)&u3, 0);
        
        u1 = (uint64_t *)src;
        u2 = (uint64_t *)cur;
        
        *u2 = u3 ^ *u1;
        
        crt++;
        
        src += DLC_DES_BLOCK_SIZE;
        cur  += DLC_DES_BLOCK_SIZE;
    }
    
    pad_cur = padding;
    if(plen == 0){
        plen = 8;
    }else{
        pad_cur = dl_cpymem(pad_cur, src, 8 - plen);
    }
    
    /* pkcs#7 padding*/
    memset(pad_cur, (uchar)plen, plen);
    enc_func(ctx, (char *)&crt, (char *)&u3, 0);
        
    u1 = (uint64_t *)padding;
    u2 = (uint64_t *)cur;
    
    *u2 = u3 ^ *u1;
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    
    return ss;
}


dl_str * _dlc_des_crt_decrypt(void *ctx, char * src, int slen, char * to, int triple)
{
    dlc_des_crypt_func      enc_func;
    int         r,plen,total,nblock,i;
    char        *cur, vi_mid[8];
    dl_str      *ss;
    des_ctx     *d_ctx;
    
    uint64_t    *u1, *u2, u3, crt;
    
    d_ctx = (des_ctx *)ctx;
    crt = *(uint64_t *)d_ctx->vi;
    
    if(!triple)
        enc_func = (dlc_des_crypt_func)dlc_des_crypt;
    else
        enc_func = (dlc_des_crypt_func)dlc_3des_crypt;
    
    total = slen;
    nblock = total / 8;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        enc_func(ctx, (char *)&crt, (char *)&u3, 0);
        
        u1 = (uint64_t *)src;
        u2 = (uint64_t *)cur;
        
        *u2 = u3 ^ *u1;
        
        crt++;
        
        src += DLC_DES_BLOCK_SIZE;
        cur  += DLC_DES_BLOCK_SIZE;
    }
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    /* pkcs#7 unpadding */
    uchar b = to[total - 1];
    ss->len -= b;
    
    return ss;
}






/* 3 des */

/*
 * Fill a Triple-DES context with subkeys calculated from three 64bit keys.
 * Does not check the parity bits of the keys, but simply ignore them.
 * Does not check for weak keys.
 */
int
dlc_3des_set3keys (tripledes_ctx *ctx,
		    const char * key1,
		    const char * key2,
		    const char * key3)
{
    int i;

    des_key_schedule (key1, ctx->encrypt_subkeys);
    des_key_schedule (key2, &(ctx->decrypt_subkeys[32]));
    des_key_schedule (key3, &(ctx->encrypt_subkeys[64]));

    for(i=0; i<32; i+=2)
    {
      ctx->decrypt_subkeys[i]	 = ctx->encrypt_subkeys[94-i];
      ctx->decrypt_subkeys[i+1]  = ctx->encrypt_subkeys[95-i];

      ctx->encrypt_subkeys[i+32] = ctx->decrypt_subkeys[62-i];
      ctx->encrypt_subkeys[i+33] = ctx->decrypt_subkeys[63-i];

      ctx->decrypt_subkeys[i+64] = ctx->encrypt_subkeys[30-i];
      ctx->decrypt_subkeys[i+65] = ctx->encrypt_subkeys[31-i];
     }

  return 0;
}


int
dlc_3des_crypt(tripledes_ctx *ctx, const char * from,
                     char * to, int mode)
{
  u32 *keys;

  keys = mode ? ctx->decrypt_subkeys : ctx->encrypt_subkeys;

  _gcry_3des_amd64_crypt_block(keys, to, from);

  return 0;
}
