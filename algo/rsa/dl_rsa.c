#include "dl_rsa.h"
#include <endian.h>

RSA_secret_key * dl_rsa_key_gen(size_t size, RSA_secret_key *key, dl_log *log)
{
    int rc;
    gcry_sexp_t sexp, key_s;
    
    if(key == NULL){
        key = dl_alloc(sizeof(RSA_secret_key), log);
    }
    
    rc = dl_rsa_gen_sexp(&sexp, size, log);
    if(rc == -1) return NULL;
    
    rc = gcry_pk_genkey (&key_s, sexp);
    dl_free_sexp(sexp);
    if(rc) return NULL;
    
    rc = dl_sexp_to_rsa(key, key_s);
    dl_free_sexp(key_s);
    if(rc == -1) return NULL;
    
    return key;
}

RSA_secret_key_ex * dl_rsa_keyEx_gen(size_t size, RSA_secret_key_ex *key_ex, dl_log *log)
{
    int rc;
    RSA_secret_key      *key;
    
    key = dl_rsa_key_gen(size, NULL, log);
    if(key == NULL) return NULL;
    
    if(key_ex == NULL){
        key_ex = dl_alloc(sizeof(RSA_secret_key_ex), log);
    }
    
    rc = dl_rsa_to_rsaEx(key_ex, key);
    
    dl_free_rsa(key, 0);
    dl_free(key);
    
    if(rc == -1) return NULL;
    
    return key_ex;
}

int dl_rsa_gen_sexp(gcry_sexp_t *sexp, int size, dl_log *log)
{
    int         rc;
    char        *fmt;
    
    if (!gcry_check_version (GCRYPT_VERSION)){
        if(log){
            dl_log_error(DL_LOG_ERR, log, "libgcrypt version mismatch");
        }
        
        return -1;
    }

    gcry_control (GCRYCTL_DISABLE_SECMEM, 0);
    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
    
    
    int buf_len;
    int size_len;
    char *buf;
    
    fmt = "(genkey\n"
          " (rsa\n"
          "  (nbits %d:%d)\n"
          " ))";
    buf_len = strlen(fmt) + 2*DL_INT32_LEN;
    buf = dl_alloc(buf_len, log);
    if(buf == NULL) return -1;
    
    sprintf(buf, "%d", size);
    size_len = strlen(buf);
    
    sprintf(buf, fmt, size_len, size);

    rc = gcry_sexp_new (sexp,buf, 0, 1);
    dl_free(buf);
    if(rc){
        if(log){
            dl_log_error(DL_LOG_ERR, log, "error creating S-expression: %s\n", gpg_strerror (rc));
        }
        return -1;
    }
    
    return 0;
}

int dl_sexp_to_rsa(RSA_secret_key *k, gcry_sexp_t sexp)
{
    int rc;
    
    rc = gcry_sexp_extract_param (sexp, NULL, "nedpqu",
                       &k->n, &k->e, &k->d,
                       &k->p, &k->q, &k->u,
                       NULL);
    if(rc){
        gcry_mpi_release (k->n);
        gcry_mpi_release (k->e);
        gcry_mpi_release (k->d);
        gcry_mpi_release (k->p);
        gcry_mpi_release (k->q);
        gcry_mpi_release (k->u);
        
        return -1;
    }

    return 0;

}

int dl_rsa_to_sexp(gcry_sexp_t *sexp, RSA_secret_key *k)
{
    int rc;
    
    gcry_sexp_t swap_info = NULL;
    if(0){
        rc = gcry_sexp_new(&swap_info, "(misc-key-info(p-q-swapped))", 0, 1);
    }
    
    rc = gcry_sexp_build(sexp, NULL,
               "(key-data"
               " (public-key"
               "  (rsa(n%m)(e%m)))"
               " (private-key"
               "  (rsa(n%m)(e%m)(d%m)(p%m)(q%m)(u%m)))"
               " %S)",
               k->n, k->e,
               k->n, k->e, k->d, k->p, k->q, k->u,
               swap_info);
    
    return 0;
}

void dl_free_rsa(void *key, int ex)
{
    RSA_secret_key_ex *k = (RSA_secret_key_ex *)key;
    
    gcry_mpi_release (k->n);
    gcry_mpi_release (k->e);
    gcry_mpi_release (k->d);
    gcry_mpi_release (k->p);
    gcry_mpi_release (k->q);
    gcry_mpi_release (k->u);
    
    if(ex){
        gcry_mpi_release(k->e1);
        gcry_mpi_release(k->e2);
    }
}

void dl_free_sexp(gcry_sexp_t sexp)
{
    gcry_sexp_release(sexp);
}

/*

static const char sample_private_key_1[] =
"(private-key\n"
" (rsa-pkcs1\n"
"  (n #D12D4810571DAA913A5D0190B84AD76B408A8EEECB0A1EBF2A97C1EA6B508827"
      "34A6A9C4106055837B11EB799801DE7EE517439F661BD2EC8C9EBE3417650A33#)\n"
"  (e #010001#)\n"
"  (d #B32625FF20AD5EC52D0834EBFE7AD2CE9FA34BD14681EF5D689C3ACF8B839866"
      "F6EBA1D3959EC313BD111FEAEB148E42C88B016972D107DA92E46B5DDD56A851#)\n"
"  (p #F0720E561FDDCB3CA813D1ED1D3CD6AFA98AF501B5C21394D93ED7351A6B74C9#)\n"

"  (q #DEB565229F7974C75C04592C65068BEDC27F62DE73F2B6908A40DD33D3EB711B#)\n"

"  (u #7403A1E02DF9867665D71726EABEA339C25486D097AF8F8F5A30E17A3FFA2686#)\n"
" )\n"
")\n";

*/

int dl_openssl_pkcs1_to_rsa(char *secret, RSA_secret_key *k)
{
    int rc;
    gcry_sexp_t sec_key;
    
    rc = gcry_sexp_sscan(&sec_key, NULL, secret, strlen(secret));
    if(rc){
        return -1;
    }
    
    rc = gcry_sexp_extract_param (sec_key, NULL, "nedpqu",
                       &k->n, &k->e, &k->d,
                       &k->p, &k->q, &k->u,
                       NULL);
    gcry_sexp_release(sec_key);
    if(rc){
        gcry_mpi_release (k->n);
        gcry_mpi_release (k->e);
        gcry_mpi_release (k->d);
        gcry_mpi_release (k->p);
        gcry_mpi_release (k->q);
        gcry_mpi_release (k->u);
        
        return -1;
    }
    
    if (gcry_mpi_cmp (k->p, k->q) > 0){
        gcry_mpi_swap (k->p, k->q);
    }
    
    return 0;
}

int dl_rsa_to_rsaEx(RSA_secret_key_ex *key_ex, RSA_secret_key *key)
{
    key_ex->n = gcry_mpi_copy(key->n);
    key_ex->e = gcry_mpi_copy(key->e);
    key_ex->d = gcry_mpi_copy(key->d);
    key_ex->p = gcry_mpi_copy(key->p);
    key_ex->q = gcry_mpi_copy(key->q);
    key_ex->u = gcry_mpi_copy(key->u);
    
    if (gcry_mpi_cmp (key_ex->p, key_ex->q) < 0){
        gcry_mpi_swap (key_ex->p, key_ex->q);
    }
    
    gcry_mpi_t  one, mpi_1;
    int rc;
    
    rc = gcry_mpi_scan(&one, GCRYMPI_FMT_HEX, "0x01", 0, NULL);
    if(rc){
        return -1;
    }
    
    mpi_1 = gcry_mpi_new(gcry_mpi_get_nbits(key_ex->p));
    key_ex->e1 = gcry_mpi_new(gcry_mpi_get_nbits(key_ex->p));
    key_ex->e2 = gcry_mpi_new(gcry_mpi_get_nbits(key_ex->p));
    
    gcry_mpi_sub(mpi_1, key_ex->p, one);
    gcry_mpi_mod(key_ex->e1, key_ex->d, mpi_1);
    
    gcry_mpi_sub(mpi_1, key_ex->q, one);
    gcry_mpi_mod(key_ex->e2, key_ex->d, mpi_1);
    
    gcry_mpi_release(mpi_1);
    
    return 0;
}

int dl_rsaEx_to_rsa(RSA_secret_key *key, RSA_secret_key_ex *key_ex)
{
    key->n = gcry_mpi_copy(key_ex->n);
    key->e = gcry_mpi_copy(key_ex->e);
    key->d = gcry_mpi_copy(key_ex->d);
    key->p = gcry_mpi_copy(key_ex->p);
    key->q = gcry_mpi_copy(key_ex->q);
    key->u = gcry_mpi_copy(key_ex->u);
    
    if (gcry_mpi_cmp (key->p, key->q) > 0){
        gcry_mpi_swap (key->p, key->q);
    }
    
    return 0;
}

static inline der_rep * dl_cal(gcry_mpi_t n)
{
    int     res_len;
    int     rc;
    size_t  wlen;
    uchar    *cur;
    
    der_rep *res = dl_alloc(sizeof(der_rep), NULL);
    if(res == NULL) return NULL;
    
    //获取有效位数并计算占用的字节数
    res->len = (gcry_mpi_get_nbits(n) - 1)  / 8 + 1;
    
    if(res->len < 128){
        res->bs = 1;
        
        res_len = res->len + 2;
        res->data = dl_alloc(res_len, NULL);
        cur = (uchar *)res->data;
        
        if(cur == NULL){
            return NULL;
        }
        
        *cur++ = 2;             // type int
        *cur++ = res->len;      // length
        
    }else{
        if(res->len >> 15)
            res->bs = 3;
        else
            res->bs = 2;
        
        res_len = res->len + 2 + res->bs;
        res->data = dl_alloc(res_len, NULL);
        cur = (uchar *)res->data;
        if(cur == NULL){
            return NULL;
        }
        
        *cur++ = 2;
        *cur++ = 0x80 | res->bs;
        
        if(res->bs == 2){
            *cur++ = (res->len >> 8) & 0xff;
            *cur++ = (res->len) & 0xff;
        }else{
            *cur++ = (res->len >> 16) & 0xff;
            *cur++ = (res->len >> 8) & 0xff;
            *cur++ = (res->len) & 0xff;
        }

    }
    
    if(__BYTE_ORDER == __LITTLE_ENDIAN)
        dl_memcpy_rev(cur, (char *)n->d, res->len);
    else
        memcpy(cur, n->d, res->len);
    
    res->len = res_len;
    printf("%d\n", res->len);
    
    return res;
}

dl_str * dl_rsaEx_to_der(RSA_secret_key_ex *key_ex, dl_str *str)
{
    int     len;
    int     bs;
    uchar   *buf, *cur;

    if(str == NULL){
        str = dl_alloc(sizeof(dl_str), NULL);
        if(str == NULL) return NULL;
    }
    
    der_rep *der_n = dl_cal(key_ex->n);
    der_rep *der_e = dl_cal(key_ex->e);
    der_rep *der_d = dl_cal(key_ex->d);
    der_rep *der_p = dl_cal(key_ex->p);
    der_rep *der_q = dl_cal(key_ex->q);
    der_rep *der_u = dl_cal(key_ex->u);
    
    der_rep *der_e1 = dl_cal(key_ex->e1);
    der_rep *der_e2 = dl_cal(key_ex->e2);
    
    len = der_n->len + der_e->len + der_d->len + 
          der_p->len + der_q->len + der_u->len + 
          der_e1->len + der_e2->len + 3;
    
    if(len >= 128){
        
        if(len >> 15)
            bs = 3;
        else
            bs = 2;
        
        buf = dl_alloc(len + 2 + bs, NULL);
        cur = buf;
        if(cur == NULL) return NULL;
        
        str->len = len + 2 + bs;
        
        *cur++ = 0x30;
        *cur++ = 0x80 | bs;
        
        if(bs == 2){
            *cur++ = (len >> 8) & 0xff;
            *cur++ = (len) & 0xff;
        }else{
            *cur++ = (len >> 16) & 0xff;
            *cur++ = (len >> 8) & 0xff;
            *cur++ = (len) & 0xff;
        }
        
    }else{
        buf = dl_alloc(len + 2, NULL);
        cur = buf;
        if(cur == NULL) return NULL;
        
        str->len = len + 2;
        
        *cur++ = 0x30;
        *cur++ = len;
    }
    
    // set version
    *cur++ = 2;
    *cur++ = 1;
    *cur++ = 0;
    
    memcpy(cur, der_n->data, der_n->len);
    cur += der_n->len;
    
    memcpy(cur, der_e->data, der_e->len);
    cur += der_e->len;
    
    memcpy(cur, der_d->data, der_d->len);
    cur += der_d->len;
    
    memcpy(cur, der_p->data, der_p->len);
    cur += der_p->len;
    
    memcpy(cur, der_q->data, der_q->len);
    cur += der_q->len;
    
    memcpy(cur, der_e1->data, der_e1->len);
    cur += der_e1->len;
    
    memcpy(cur, der_e2->data, der_e2->len);
    cur += der_e2->len;
    
    memcpy(cur, der_u->data, der_u->len);
    cur += der_u->len;
    
    str->data = (char *)buf;
    
    return str;
}

dl_str * dl_rsaDer_to_pem(dl_str *der)
{
    int     len,b_left;
    dl_str  *res;
    
    dl_str head = dl_string("-----BEGIN RSA PRIVATE KEY-----\n");
    dl_str tail = dl_string("-----END RSA PRIVATE KEY-----\n");
    
    if(der->len % 48 == 0){
        len = der->len/48 * 65 + head.len + tail.len;
    }else{
        len = der->len/48 * 65 + head.len + tail.len;
        len += base64_len(der->len % 48) + 1;
    }
    
    //len = base64_len(der->len);
    res = dl_alloc(sizeof(dl_str), NULL);
    res->data = dl_alloc(len, NULL);
    res->len = len;
    
    memcpy(res->data, head.data, head.len);
    
    b_left = der->len;
    
    dl_str  s1, s2;
    s1.data = der->data;
    
    s2.data = res->data + head.len;
    
    while(b_left){
        if(b_left >= 48){
            s1.len = 48;
            b_left -= 48;
        }else{
            s1.len = b_left;
            b_left = 0;
        }
        
        dl_encode_base64(&s2, &s1);
        
        s1.data += s1.len;
        s2.data += s2.len;
        
        *s2.data++ = '\n';
    }
    
    memcpy(s2.data, tail.data, tail.len);
    
    return res;
}

dl_str *dl_rsaPem_to_der(char *data, size_t len)
{
    char    *cur, *last;
    int     res_len;
    
    cur = data;
    
    dl_str head = dl_string("-----BEGIN RSA PRIVATE KEY-----\n");
    dl_str tail = dl_string("-----END RSA PRIVATE KEY-----\n");
    
    
    for(; *cur != '-'; cur++){
        len--;
    };
    cur += head.len;
    
    last = cur;
    
    for(; *last != '-'; last++){
        len--;
    };
    
    if(len < 0) return NULL;
    
    len = last - cur;
    
    if(len % 65 == 0){
        res_len = len/65 * 48;
    }else{
        res_len = len/65 * 48;
        res_len += (len % 65 - 1) * 3 / 4;
    }
    
    dl_str  s1, s2;
    
    dl_str  *res = dl_alloc(sizeof(dl_str), NULL);
    if(res == NULL) return NULL;
    
    res->data = dl_alloc(res_len, NULL);
    if(res->data == NULL) return NULL;
    res->len = 0;
    
    s1.data = cur;
    s2.data = res->data;
    
    while(len){
        
        if(len >= 65){
            s1.len = 64;
            len -= 65;
        }else{
            s1.len = len - 1;
            len = 0;
        }
        
        dl_decode_base64(&s2, &s1);
        
        s2.data += s2.len;
        s1.data += s1.len + 1;
        
        res->len += s2.len;
        
    }
    
    return res;
    
}

static inline int dl_der_get_item(uchar *data, dl_str *res)
{
    int bs;
    uchar   *cur, *tmp;
    int32_t     len;
    
    tmp = (uchar *)&len;
    
    
    cur = data;
    if(*cur++ != 2) return -1;
    
    if(*cur < 128){
        len = *cur++;
    }else{
        bs = *cur & 0x7f;
        
        if(bs == 2){
            if(__BYTE_ORDER == __LITTLE_ENDIAN){
                tmp[0] = *(cur+2);
                tmp[1] = *(cur+1);
            }else{
                tmp[0] = *(cur+1);
                tmp[1] = *(cur+2);
            }
        }else{
            if(__BYTE_ORDER == __LITTLE_ENDIAN){
                tmp[0] = *(cur+3);
                tmp[1] = *(cur+2);
                tmp[2] = *(cur+1);
            }else{
                tmp[0] = *(cur+1);
                tmp[1] = *(cur+2);
                tmp[2] = *(cur+3);
            }
        }
        
        cur += bs + 1;
    }
    
    res->data = cur;
    res->len = len;
    
    return 0;
}

RSA_secret_key_ex *dl_der_to_rsaEx(dl_str *der, RSA_secret_key_ex *key)
{
    int     bs, rc;
    dl_str  s1, s2;
    uchar   *cur, *last;
    if(key == NULL){
        key = dl_alloc(sizeof(RSA_secret_key_ex), NULL);
    }
    
    cur = (uchar *)der->data;
    last = cur + der->len;
    
    if(*cur++ != 0x30) return NULL;
    
    if(*cur < 128){
        cur++;
    }else{
        bs = *cur & 0x7f;
        cur += bs + 1;
    }
    
    // skip version
    cur += 3;
    
    if(cur > last) return NULL;
    
    // n
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return NULL;
    gcry_mpi_scan(&key->n, GCRYMPI_FMT_STD, s1.data, s1.len, NULL);
    cur = s1.data + s1.len;
    
    // e
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return NULL;
    gcry_mpi_scan(&key->e, GCRYMPI_FMT_STD, s1.data, s1.len, NULL);
    cur = s1.data + s1.len;
    
    if(cur == last) return key;  // public key
    
    // d
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return NULL;
    gcry_mpi_scan(&key->d, GCRYMPI_FMT_STD, s1.data, s1.len, NULL);
    cur = s1.data + s1.len;
    
    // p
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return NULL;
    gcry_mpi_scan(&key->p, GCRYMPI_FMT_STD, s1.data, s1.len, NULL);
    cur = s1.data + s1.len;
    
    // q
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return NULL;
    gcry_mpi_scan(&key->q, GCRYMPI_FMT_STD, s1.data, s1.len, NULL);
    cur = s1.data + s1.len;
    
    // e1
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return NULL;
    gcry_mpi_scan(&key->e1, GCRYMPI_FMT_STD, s1.data, s1.len, NULL);
    cur = s1.data + s1.len;
    
    // e2
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return NULL;
    gcry_mpi_scan(&key->e2, GCRYMPI_FMT_STD, s1.data, s1.len, NULL);
    cur = s1.data + s1.len;
    
    // u
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return NULL;
    gcry_mpi_scan(&key->u, GCRYMPI_FMT_STD, s1.data, s1.len, NULL);
    cur = s1.data + s1.len;
    
    if(cur != last) return NULL;
    
    return key;
}

