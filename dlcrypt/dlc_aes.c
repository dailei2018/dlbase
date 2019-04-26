#include "dlc_base.h"
#include "dlc_aes_internal.h"

static unsigned
xtime(unsigned x)
{
  assert (x < 0x100);

  x <<= 1;
  if (x & 0x100)
    x ^= 0x11b;

  assert (x < 0x100);

  return x;
}

static const uint8_t gf2_log[0x100] =
{
  0x00,0x00,0x19,0x01,0x32,0x02,0x1a,0xc6,
  0x4b,0xc7,0x1b,0x68,0x33,0xee,0xdf,0x03,
  0x64,0x04,0xe0,0x0e,0x34,0x8d,0x81,0xef,
  0x4c,0x71,0x08,0xc8,0xf8,0x69,0x1c,0xc1,
  0x7d,0xc2,0x1d,0xb5,0xf9,0xb9,0x27,0x6a,
  0x4d,0xe4,0xa6,0x72,0x9a,0xc9,0x09,0x78,
  0x65,0x2f,0x8a,0x05,0x21,0x0f,0xe1,0x24,
  0x12,0xf0,0x82,0x45,0x35,0x93,0xda,0x8e,
  0x96,0x8f,0xdb,0xbd,0x36,0xd0,0xce,0x94,
  0x13,0x5c,0xd2,0xf1,0x40,0x46,0x83,0x38,
  0x66,0xdd,0xfd,0x30,0xbf,0x06,0x8b,0x62,
  0xb3,0x25,0xe2,0x98,0x22,0x88,0x91,0x10,
  0x7e,0x6e,0x48,0xc3,0xa3,0xb6,0x1e,0x42,
  0x3a,0x6b,0x28,0x54,0xfa,0x85,0x3d,0xba,
  0x2b,0x79,0x0a,0x15,0x9b,0x9f,0x5e,0xca,
  0x4e,0xd4,0xac,0xe5,0xf3,0x73,0xa7,0x57,
  0xaf,0x58,0xa8,0x50,0xf4,0xea,0xd6,0x74,
  0x4f,0xae,0xe9,0xd5,0xe7,0xe6,0xad,0xe8,
  0x2c,0xd7,0x75,0x7a,0xeb,0x16,0x0b,0xf5,
  0x59,0xcb,0x5f,0xb0,0x9c,0xa9,0x51,0xa0,
  0x7f,0x0c,0xf6,0x6f,0x17,0xc4,0x49,0xec,
  0xd8,0x43,0x1f,0x2d,0xa4,0x76,0x7b,0xb7,
  0xcc,0xbb,0x3e,0x5a,0xfb,0x60,0xb1,0x86,
  0x3b,0x52,0xa1,0x6c,0xaa,0x55,0x29,0x9d,
  0x97,0xb2,0x87,0x90,0x61,0xbe,0xdc,0xfc,
  0xbc,0x95,0xcf,0xcd,0x37,0x3f,0x5b,0xd1,
  0x53,0x39,0x84,0x3c,0x41,0xa2,0x6d,0x47,
  0x14,0x2a,0x9e,0x5d,0x56,0xf2,0xd3,0xab,
  0x44,0x11,0x92,0xd9,0x23,0x20,0x2e,0x89,
  0xb4,0x7c,0xb8,0x26,0x77,0x99,0xe3,0xa5,
  0x67,0x4a,0xed,0xde,0xc5,0x31,0xfe,0x18,
  0x0d,0x63,0x8c,0x80,0xc0,0xf7,0x70,0x07,
};

static const uint8_t gf2_exp[0x100] =
{
  0x01,0x03,0x05,0x0f,0x11,0x33,0x55,0xff,
  0x1a,0x2e,0x72,0x96,0xa1,0xf8,0x13,0x35,
  0x5f,0xe1,0x38,0x48,0xd8,0x73,0x95,0xa4,
  0xf7,0x02,0x06,0x0a,0x1e,0x22,0x66,0xaa,
  0xe5,0x34,0x5c,0xe4,0x37,0x59,0xeb,0x26,
  0x6a,0xbe,0xd9,0x70,0x90,0xab,0xe6,0x31,
  0x53,0xf5,0x04,0x0c,0x14,0x3c,0x44,0xcc,
  0x4f,0xd1,0x68,0xb8,0xd3,0x6e,0xb2,0xcd,
  0x4c,0xd4,0x67,0xa9,0xe0,0x3b,0x4d,0xd7,
  0x62,0xa6,0xf1,0x08,0x18,0x28,0x78,0x88,
  0x83,0x9e,0xb9,0xd0,0x6b,0xbd,0xdc,0x7f,
  0x81,0x98,0xb3,0xce,0x49,0xdb,0x76,0x9a,
  0xb5,0xc4,0x57,0xf9,0x10,0x30,0x50,0xf0,
  0x0b,0x1d,0x27,0x69,0xbb,0xd6,0x61,0xa3,
  0xfe,0x19,0x2b,0x7d,0x87,0x92,0xad,0xec,
  0x2f,0x71,0x93,0xae,0xe9,0x20,0x60,0xa0,
  0xfb,0x16,0x3a,0x4e,0xd2,0x6d,0xb7,0xc2,
  0x5d,0xe7,0x32,0x56,0xfa,0x15,0x3f,0x41,
  0xc3,0x5e,0xe2,0x3d,0x47,0xc9,0x40,0xc0,
  0x5b,0xed,0x2c,0x74,0x9c,0xbf,0xda,0x75,
  0x9f,0xba,0xd5,0x64,0xac,0xef,0x2a,0x7e,
  0x82,0x9d,0xbc,0xdf,0x7a,0x8e,0x89,0x80,
  0x9b,0xb6,0xc1,0x58,0xe8,0x23,0x65,0xaf,
  0xea,0x25,0x6f,0xb1,0xc8,0x43,0xc5,0x54,
  0xfc,0x1f,0x21,0x63,0xa5,0xf4,0x07,0x09,
  0x1b,0x2d,0x77,0x99,0xb0,0xcb,0x46,0xca,
  0x45,0xcf,0x4a,0xde,0x79,0x8b,0x86,0x91,
  0xa8,0xe3,0x3e,0x42,0xc6,0x51,0xf3,0x0e,
  0x12,0x36,0x5a,0xee,0x29,0x7b,0x8d,0x8c,
  0x8f,0x8a,0x85,0x94,0xa7,0xf2,0x0d,0x17,
  0x39,0x4b,0xdd,0x7c,0x84,0x97,0xa2,0xfd,
  0x1c,0x24,0x6c,0xb4,0xc7,0x52,0xf6,0x01,
};

static unsigned
mult(unsigned a, unsigned b)
{
  return (a && b) ? gf2_exp[ (gf2_log[a] + gf2_log[b]) % 255] : 0;
}

static void
inv_mix_column(uint32_t *a)
{
  uint8_t c[4][4];
  unsigned i, j;
	
  for (j = 0; j < 4; j++)
    {
      for(i = 0; i < 4; i++)
	{
	  c[j][i] = mult(0xe, (a[j] >> i*8) & 0xff)
	    ^ mult(0xb, (a[j] >> ((i+1)%4)*8) & 0xff)
	    ^ mult(0xd, (a[j] >> ((i+2)%4)*8) & 0xff)
	    ^ mult(0x9, (a[j] >> ((i+3)%4)*8) & 0xff);
	}
    }
  for (i = 0; i < 4; i++)
    {
      a[i] = 0;
      for(j = 0; j < 4; j++)
	a[i] |= c[i][j] << (j*8);
    }
}

#define SWAP(a, b) \
do { uint32_t t_swap = (a); (a) = (b); (b) = t_swap; } while(0)

void
dlc_aes_set_decrypt_key(aes_ctx *ctx,
		    unsigned keysize, const uint8_t *key)
{
  unsigned nkeys;
  unsigned i, j, k;
  
  /* We first create subkeys for encryption,
   * then modify the subkeys for decryption. */
  dlc_aes_set_encrypt_key(ctx, keysize, key);

  nkeys = (AES_BLOCK_SIZE/4) * (ctx->nrounds + 1);

  /* Reverse the order of subkeys */
  for (i = 0, j = ctx->nrounds * 4;
       i < j;
       i += 4, j -= 4)
    for (k = 0; k<4; k++)
      SWAP(ctx->keys[i+k], ctx->keys[j+k]);

  /* Transform all subkeys but the first and last. */
  for (i = 4; i < 4 * ctx->nrounds; i += 4)
    inv_mix_column(ctx->keys + i);
}

void
dlc_aes_set_encrypt_key(aes_ctx *ctx, unsigned keysize, const uint8_t *key)
{
  unsigned nk, nr, i, lastkey;
  uint32_t temp, rcon;

  assert(keysize >= AES_MIN_KEY_SIZE);
  assert(keysize <= AES_MAX_KEY_SIZE);
  
  /* Truncate keysizes to the valid key sizes provided by Rijndael */
  if (keysize == 32) {
    nk = 8;
    nr = 14;
  } else if (keysize >= 24) {
    nk = 6;
    nr = 12;
  } else { /* must be 16 or more */
    nk = 4;
    nr = 10;
  }

  lastkey = (AES_BLOCK_SIZE/4) * (nr + 1);
  ctx->nrounds = nr;
  rcon = 1;
  for (i=0; i<nk; i++)
    {
      ctx->keys[i] = key[i*4] + (key[i*4+1]<<8) + (key[i*4+2]<<16) +
	(key[i*4+3]<<24);
    }

  for (i=nk; i<lastkey; i++)
    {
      temp = ctx->keys[i-1];
      if (i % nk == 0)
	{
	  temp = SUBBYTE(ROTBYTE(temp), aes_sbox) ^ rcon;
	  rcon = (uint32_t)xtime((uint8_t)rcon&0xff);
	}
      else if (nk > 6 && (i%nk) == 4)
	{
	  temp = SUBBYTE(temp, aes_sbox);
	}
      ctx->keys[i] = ctx->keys[i-nk] ^ temp;
    }
}



dl_str * dlc_aes_ecb_encrypt(aes_ctx *ctx, char * src, int slen, char * to)
{
    int         r,plen,total,nblock,i;
    char        *cur, padding[AES_BLOCK_SIZE], *pad_cur;
    dl_str      *ss;
    

    r = slen % AES_BLOCK_SIZE;
    if(r == 0) plen = 0;
    else plen = AES_BLOCK_SIZE - r;
    
    total = (slen / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    
    nblock = slen / AES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        dlc_aes_encrypt(ctx, src, AES_BLOCK_SIZE, cur);
        src += AES_BLOCK_SIZE;
        cur  += AES_BLOCK_SIZE;
    }
    
    pad_cur = padding;
    if(plen == 0){
        plen = AES_BLOCK_SIZE;
    }else{
        pad_cur = dl_cpymem(pad_cur, src, AES_BLOCK_SIZE - plen);
    }
    
    /* pkcs#7 padding*/
    memset(pad_cur, (uchar)plen, plen);
    dlc_aes_encrypt(ctx, padding, AES_BLOCK_SIZE, cur);
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    return ss;
}


dl_str * dlc_aes_ecb_decrypt(aes_ctx *ctx, char * src, int slen, char * to)
{
    int         r,plen,total,nblock,i;
    char        *cur, padding[AES_BLOCK_SIZE];
    dl_str      *ss;
    
    total = slen;
        
    nblock = total / AES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        dlc_aes_decrypt(ctx, src, AES_BLOCK_SIZE, cur);
        src += AES_BLOCK_SIZE;
        cur  += AES_BLOCK_SIZE;
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

/* cbc */
dl_str * dlc_aes_cbc_encrypt(aes_ctx *ctx, char * src, int slen, char * to)
{
    int         r,plen,total,nblock,i;
    char        *cur, padding[AES_BLOCK_SIZE], *pad_cur, *vi, mid[AES_BLOCK_SIZE];
    dl_str      *ss;
    
    vi = ctx->vi;

    r = slen % AES_BLOCK_SIZE;
    if(r == 0) plen = 0;
    else plen = AES_BLOCK_SIZE - r;
    
    total = (slen / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    
    nblock = slen / AES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        dlc_bxor(mid, vi, src, AES_BLOCK_SIZE);
        
        dlc_aes_encrypt(ctx, mid, AES_BLOCK_SIZE, cur);
        
        vi = cur;
        
        src += AES_BLOCK_SIZE;
        cur  += AES_BLOCK_SIZE;
    }
    
    pad_cur = padding;
    if(plen == 0){
        plen = AES_BLOCK_SIZE;
    }else{
        pad_cur = dl_cpymem(pad_cur, src, AES_BLOCK_SIZE - plen);
    }
    
    /* pkcs#7 padding*/
    memset(pad_cur, (uchar)plen, plen);
    dlc_bxor(mid, vi, padding, AES_BLOCK_SIZE);
    dlc_aes_encrypt(ctx, mid, AES_BLOCK_SIZE, cur);
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    return ss;
}

dl_str * dlc_aes_cbc_decrypt(aes_ctx *ctx, char * src, int slen, char * to)
{
    int         r,plen,total,nblock,i;
    char        *cur, padding[AES_BLOCK_SIZE], *pad_cur, *vi, mid[AES_BLOCK_SIZE];
    dl_str      *ss;
    
    vi = ctx->vi;
    
    total = slen;
    
    nblock = slen / AES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        dlc_aes_decrypt(ctx, src, AES_BLOCK_SIZE, mid);
        dlc_bxor(cur, vi, mid, AES_BLOCK_SIZE);
        
        vi = src;
        
        src += AES_BLOCK_SIZE;
        cur  += AES_BLOCK_SIZE;
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
dl_str * dlc_aes_cfb_encrypt(aes_ctx *ctx, char * src, int slen, char * to)
{
    int         r,plen,total,nblock,i;
    char        *cur, padding[AES_BLOCK_SIZE], *pad_cur, *vi, mid[AES_BLOCK_SIZE];
    dl_str      *ss;
    
    vi = ctx->vi;

    r = slen % AES_BLOCK_SIZE;
    if(r == 0) plen = 0;
    else plen = AES_BLOCK_SIZE - r;
    
    total = (slen / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    
    nblock = slen / AES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        dlc_aes_encrypt(ctx, vi, AES_BLOCK_SIZE, mid);
        dlc_bxor(cur, mid, src, AES_BLOCK_SIZE);
        
        vi = cur;
        
        src += AES_BLOCK_SIZE;
        cur += AES_BLOCK_SIZE;
    }
    
    pad_cur = padding;
    if(plen == 0){
        plen = AES_BLOCK_SIZE;
    }else{
        pad_cur = dl_cpymem(pad_cur, src, AES_BLOCK_SIZE - plen);
    }
    
    /* pkcs#7 padding*/
    memset(pad_cur, (uchar)plen, plen);
    dlc_aes_encrypt(ctx, vi, AES_BLOCK_SIZE, mid);
    dlc_bxor(cur, mid, padding, AES_BLOCK_SIZE);
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    return ss;
}

dl_str * dlc_aes_cfb_decrypt(aes_ctx *ctx, char * src, int slen, char * to)
{
    int         r,plen,total,nblock,i;
    char        *cur, padding[AES_BLOCK_SIZE], *pad_cur, *vi, mid[AES_BLOCK_SIZE];
    dl_str      *ss;
    
    vi = ctx->vi;
    
    total = slen;
    
    nblock = slen / AES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        dlc_aes_encrypt(ctx, vi, AES_BLOCK_SIZE, mid);
        dlc_bxor(cur, mid, src, AES_BLOCK_SIZE);
        
        vi = src;
        
        src += AES_BLOCK_SIZE;
        cur += AES_BLOCK_SIZE;
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
dl_str * dlc_aes_ofb_encrypt(aes_ctx *ctx, char * src, int slen, char * to)
{
    int         r,plen,total,nblock,i;
    char        *cur, padding[AES_BLOCK_SIZE], *pad_cur, *vi, mid[AES_BLOCK_SIZE];
    dl_str      *ss;
    
    vi = ctx->vi;

    r = slen % AES_BLOCK_SIZE;
    if(r == 0) plen = 0;
    else plen = AES_BLOCK_SIZE - r;
    
    total = (slen / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    
    nblock = slen / AES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        dlc_aes_encrypt(ctx, vi, AES_BLOCK_SIZE, mid);
        dlc_bxor(cur, mid, src, AES_BLOCK_SIZE);
        
        vi = mid;
        
        src += AES_BLOCK_SIZE;
        cur += AES_BLOCK_SIZE;
    }
    
    pad_cur = padding;
    if(plen == 0){
        plen = AES_BLOCK_SIZE;
    }else{
        pad_cur = dl_cpymem(pad_cur, src, AES_BLOCK_SIZE - plen);
    }
    
    /* pkcs#7 padding*/
    memset(pad_cur, (uchar)plen, plen);
    dlc_aes_encrypt(ctx, vi, AES_BLOCK_SIZE, mid);
    dlc_bxor(cur, mid, padding, AES_BLOCK_SIZE);
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    return ss;
}

dl_str * dlc_aes_ofb_decrypt(aes_ctx *ctx, char * src, int slen, char * to)
{
    int         r,plen,total,nblock,i;
    char        *cur, padding[AES_BLOCK_SIZE], *pad_cur, *vi, mid[AES_BLOCK_SIZE];
    dl_str      *ss;
    
    vi = ctx->vi;
    
    total = slen;
    
    nblock = slen / AES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        dlc_aes_encrypt(ctx, vi, AES_BLOCK_SIZE, mid);
        dlc_bxor(cur, mid, src, AES_BLOCK_SIZE);
        
        vi = mid;
        
        src += AES_BLOCK_SIZE;
        cur += AES_BLOCK_SIZE;
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

void dlc_bigendian_plus_one(char *res, uint64_t n1)
{
    n1++;
    
    if(__BYTE_ORDER == __LITTLE_ENDIAN){
        dl_memcpy_rev(res, (char *)&n1, 8);
        
    }else{
        memcpy(res, &n1, 8);
        
    }
    
}

/* crt */
dl_str * dlc_aes_crt_encrypt(aes_ctx *ctx, char * src, int slen, char * to)
{
    int         r,plen,total,nblock,i,n;
    char        *cur, padding[AES_BLOCK_SIZE], *pad_cur, mid[AES_BLOCK_SIZE], *vi, *vi1;
    dl_str      *ss;
    
    n = 0;
    vi = ctx->vi;
    vi1 = vi + AES_BLOCK_SIZE - 8;
    dlc_bigendian_plus_one(vi1, n++);
    
    r = slen % AES_BLOCK_SIZE;
    if(r == 0) plen = 0;
    else plen = AES_BLOCK_SIZE - r;
    
    total = (slen / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    
    nblock = slen / AES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        dlc_aes_encrypt(ctx, vi, AES_BLOCK_SIZE, mid);
        dlc_bxor(cur, mid, src, AES_BLOCK_SIZE);
        
        dlc_bigendian_plus_one(vi1, n++);
        
        src += AES_BLOCK_SIZE;
        cur += AES_BLOCK_SIZE;
    }
    
    pad_cur = padding;
    if(plen == 0){
        plen = AES_BLOCK_SIZE;
    }else{
        pad_cur = dl_cpymem(pad_cur, src, AES_BLOCK_SIZE - plen);
    }
    
    /* pkcs#7 padding*/
    memset(pad_cur, (uchar)plen, plen);
    dlc_aes_encrypt(ctx, vi, AES_BLOCK_SIZE, mid);
    dlc_bxor(cur, mid, padding, AES_BLOCK_SIZE);
    
    
    ss = dl_alloc(sizeof(dl_str), NULL);
    if(ss == NULL) return NULL;
    
    ss->data = to;
    ss->len = total;
    
    return ss;
}

dl_str * dlc_aes_crt_decrypt(aes_ctx *ctx, char * src, int slen, char * to)
{
    int         r,plen,total,nblock,i;
    char        *cur, padding[AES_BLOCK_SIZE], *pad_cur, mid[AES_BLOCK_SIZE], *vi, *vi1;
    dl_str      *ss;
    
    uint64_t    *u1,n;
    
    n = 0;
    vi = ctx->vi;
    vi1 = vi + AES_BLOCK_SIZE - 8;
    dlc_bigendian_plus_one(vi1, n++);
    
    total = slen;
    
    nblock = slen / AES_BLOCK_SIZE;
    
    if(to == NULL){
        to = dl_alloc(total, NULL);
        if(to == NULL) return NULL;
    }
    
    cur = to;
    
    for(i = 0; i < nblock; i++){
        dlc_aes_encrypt(ctx, vi, AES_BLOCK_SIZE, mid);
        dlc_bxor(cur, mid, src, AES_BLOCK_SIZE);
        
        dlc_bigendian_plus_one(vi1, n++);
        
        src += AES_BLOCK_SIZE;
        cur += AES_BLOCK_SIZE;
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


