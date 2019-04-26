#include "dlc_base.h"

/* rsa sha1 */

static const uint8_t
sha1_prefix[] =
{
  /* 15 octets prefix, 20 octets hash, total 35 */
  0x30,       33, /* SEQUENCE */
    0x30,      9, /* SEQUENCE */
      0x06,    5, /* OBJECT IDENTIFIER */
  	  0x2b, 0x0e, 0x03, 0x02, 0x1a,
      0x05,    0, /* NULL */
    0x04,     20  /* OCTET STRING */
      /* Here comes the raw hash value */
};

/* {01ffff..00[prefix][digest]} */
char * rsa_sha1_encode(int bf_len, char *msg, int len)
{
    USHAContext     ctx;
    uchar           digest[SHA1HashSize];
    int             rc;
    char            *res, *end;
    
    rc = USHAReset(&ctx, SHA1);
    if(rc) return NULL;
    rc = USHAInput(&ctx, msg, len);
    if(rc) return NULL;
    rc = USHAResult(&ctx, digest);
    if(rc) return NULL;
    
    
    res = dl_alloc(bf_len, NULL);
    if(res == NULL) return NULL;
    end = res + bf_len;
    
    /* pkcs#1 padding */
    int total       = 35;
    int pre_len     = 15;
    int h_len       = 20;
    
    memcpy(end - total, sha1_prefix, pre_len);
    *(end - total - 1) = 0;
    
    res[0] = 1;
    memset(res+1, 0xff, bf_len - total - 2);
    
    memcpy(end - h_len, digest, h_len);
    return res;
}


int rsa_sha1_sign(rsa_private_key *key, char *msg, int len, mpz_t s)
{
    char *res;
    
    res = rsa_sha1_encode(key->size - 1, msg, len);
    if(res == NULL) return -1;
    
    /* convert to mpi */
    nettle_mpz_set_str_256_u(s, key->size - 1, res);
    dl_free(res);
    
    /* private key encrypt */
    rsa_compute_root(key, s, s);
    return 0;
}

int rsa_verify(rsa_public_key *key, mpz_t m, mpz_t s)
{
    int rc;
  
    mpz_t m1;
  
    if ( (mpz_sgn(s) <= 0) || (mpz_cmp(s, key->n) >= 0) )
        return -1;
       
    mpz_init(m1);
  
    mpz_powm(m1, s, key->e, key->n);

    rc = mpz_cmp(m, m1);

    mpz_clear(m1);

    return rc;
}

int rsa_sha1_verify(rsa_public_key *key, char *msg, size_t len, mpz_t s)
{
    int     rc;
    char    *res;
    mpz_t   m;
    
    res = rsa_sha1_encode(key->size - 1, msg, len);
    if(res == NULL) return -1;
    
    mpz_init(m);
    
    nettle_mpz_set_str_256_u(m, key->size - 1, res);
    dl_free(res);
    
    rc = rsa_verify(key, m, s);
    
    mpz_clear(m);
    
    return rc;
}




/* sha-256 */
static const uint8_t
sha256_prefix[] =
{
  /* 19 octets prefix, 32 octets hash, total 51 */
  0x30,      49, /* SEQUENCE */
    0x30,    13, /* SEQUENCE */
      0x06,   9, /* OBJECT IDENTIFIER */
        0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,
      0x05,   0, /* NULL */
    0x04,    32  /* OCTET STRING */
      /* Here comes the raw hash value */
};

/* {01ffff..00[prefix][digest]} */
char * rsa_sha256_encode(int bf_len, char *msg, int len)
{
    USHAContext     ctx;
    uchar           digest[SHA256HashSize];
    int             rc;
    char            *res, *end;
    
    rc = USHAReset(&ctx, SHA256);
    if(rc) return NULL;
    rc = USHAInput(&ctx, msg, len);
    if(rc) return NULL;
    rc = USHAResult(&ctx, digest);
    if(rc) return NULL;
    
    
    res = dl_alloc(bf_len, NULL);
    if(res == NULL) return NULL;
    end = res + bf_len;
    
    /* pkcs#1 padding */
    int total       = 51;
    int pre_len     = 19;
    int h_len       = 32;
    
    memcpy(end - total, sha256_prefix, pre_len);
    *(end - total - 1) = 0;
    
    res[0] = 1;
    memset(res+1, 0xff, bf_len - total - 2);
    
    memcpy(end - h_len, digest, h_len);
    return res;
}


int rsa_sha256_sign(rsa_private_key *key, char *msg, int len, mpz_t s)
{
    char *res;
    
    res = rsa_sha256_encode(key->size - 1, msg, len);
    if(res == NULL) return -1;
    
    /* convert to mpi */
    nettle_mpz_set_str_256_u(s, key->size - 1, res);
    dl_free(res);
    
    /* private key encrypt */
    rsa_compute_root(key, s, s);
    return 0;
}

int rsa_sha256_verify(rsa_public_key *key, char *msg, size_t len, mpz_t s)
{
    int     rc;
    char    *res;
    mpz_t   m;
    
    res = rsa_sha256_encode(key->size - 1, msg, len);
    if(res == NULL) return -1;
    
    mpz_init(m);
    
    nettle_mpz_set_str_256_u(m, key->size - 1, res);
    dl_free(res);
    
    rc = rsa_verify(key, m, s);
    
    mpz_clear(m);
    
    return rc;
}






/* rsa md5 */
static const uint8_t
md5_prefix[] =
{
  /* 18 octets prefix, 16 octets hash, 34 total. */
  0x30,       32, /* SEQUENCE */
    0x30,     12, /* SEQUENCE */
      0x06,    8, /* OBJECT IDENTIFIER */
  	0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05,
      0x05,    0, /* NULL */
    0x04,     16  /* OCTET STRING */
      /* Here comes the raw hash value */
};

/* {01ffff..00[prefix][digest]} */
char * rsa_md5_encode(int bf_len, char *msg, int len)
{
    int             rc;
    char            *res, *end;
    uchar           digest[16];
    md5_ctx         context;
    
    md5Init(&context);
    md5Update(&context, msg, len);
    md5Final(digest, &context);
    
    res = dl_alloc(bf_len, NULL);
    if(res == NULL) return NULL;
    end = res + bf_len;
    
    /* pkcs#1 padding */
    int total       = 34;
    int pre_len     = 18;
    int h_len       = 16;
    
    memcpy(end - total, md5_prefix, pre_len);
    *(end - total - 1) = 0;
    
    res[0] = 1;
    memset(res+1, 0xff, bf_len - total - 2);
    
    memcpy(end - h_len, digest, h_len);
    return res;
}


int rsa_md5_sign(rsa_private_key *key, char *msg, int len, mpz_t s)
{
    char *res;
    
    res = rsa_md5_encode(key->size - 1, msg, len);
    if(res == NULL) return -1;
    
    /* convert to mpi */
    nettle_mpz_set_str_256_u(s, key->size - 1, res);
    dl_free(res);
    
    /* private key encrypt */
    rsa_compute_root(key, s, s);
    return 0;
}

int rsa_md5_verify(rsa_public_key *key, char *msg, size_t len, mpz_t s)
{
    int     rc;
    char    *res;
    mpz_t   m;
    
    res = rsa_md5_encode(key->size - 1, msg, len);
    if(res == NULL) return -1;
    
    mpz_init(m);
    
    nettle_mpz_set_str_256_u(m, key->size - 1, res);
    dl_free(res);
    
    rc = rsa_verify(key, m, s);
    
    mpz_clear(m);
    
    return rc;
}