#include "dlc_base.h"
#include "dlc_asn1.h"



/* mine */

dl_str * dlc_rsa_pem_to_der(char *fname)
{
    char            *cur, *last, *buf, tmpbf[1024];
    int             rc, len, res_len;
    FILE            *fp;
    struct stat     st;
    
    dl_str head         = dl_string("-----BEGIN RSA PRIVATE KEY-----");
    dl_str head_8       = dl_string("-----BEGIN PRIVATE KEY-----");
    dl_str head_pub     = dl_string("-----BEGIN RSA PUBLIC KEY-----");
    dl_str head_pub_8   = dl_string("-----BEGIN PUBLIC KEY-----");
    
    rc = stat(fname, &st);
    if(rc == -1) return NULL;
    
    buf = dl_alloc(st.st_size, NULL);
    if(buf == NULL) return NULL;
    cur = buf;
    
    fp = fopen(fname, "r");
    if(fp == NULL) return NULL;
    
    while(fgets(tmpbf, 1024, fp)){
        len = strlen(tmpbf) - 1;
        if(tmpbf[len - 1] == '\r') len--;
        
        cur = dl_cpymem(cur, tmpbf, len);
    }
    *cur = '\0';
    
    
    cur = buf;
    
    if(memcmp(cur, head.data, head.len) == 0){
        cur += head.len;
    }else if(memcmp(cur, head_pub.data, head_pub.len) == 0){
        cur += head_pub.len;
    }else if(memcmp(cur, head_pub_8.data, head_pub_8.len) == 0){
        cur += head_pub_8.len;
    }else if(memcmp(cur, head_8.data, head_8.len) == 0){
        cur += head_8.len;
    }else{
        return NULL;
    }
    
    last = cur;
    
    for(; *last != '-'; last++){}
    
    len = last - cur;
    
    if(len % 64 == 0){
        res_len = len * 3 / 4;
    }else{
        res_len = len / 64 * 48;
        res_len += (len % 64) * 3 / 4;
    }
    
    
    /* base64 解码 */
    dl_str      s1, *s2;
    
    s2 = dl_alloc(sizeof(dl_str), NULL);
    if(s2 == NULL) return NULL;
    
    s1.data = cur;
    s1.len = len;
    s2->data = dl_alloc(res_len, NULL);
    dl_decode_base64(s2, &s1);
    
    
    return s2;
}

dl_str * dlc_rsa_der_to_pem(dl_str *der, int is_pub)
{
    int     len,b_left;
    dl_str  *res;
    
    dl_str *head, *tail;
    
    dl_str head_pub = dl_string("-----BEGIN RSA PUBLIC KEY-----\n");
    dl_str tail_pub = dl_string("-----END RSA PUBLIC KEY-----\n");
    dl_str head_pri = dl_string("-----BEGIN RSA PRIVATE KEY-----\n");
    dl_str tail_pri = dl_string("-----END RSA PRIVATE KEY-----\n");
    
    if(is_pub){
        head = &head_pub;
        tail = &tail_pub;
    }else{
        head = &head_pri;
        tail = &tail_pri;
    }
    
    if(der->len % 48 == 0){
        len = der->len/48 * 65 + head->len + tail->len;
    }else{
        len = der->len/48 * 65 + head->len + tail->len;
        len += base64_len(der->len % 48) + 1;
    }
    
    //len = base64_len(der->len);
    res = dl_alloc(sizeof(dl_str), NULL);
    res->data = dl_alloc(len, NULL);
    res->len = len;
    
    memcpy(res->data, head->data, head->len);
    
    b_left = der->len;
    
    dl_str  s1, s2;
    s1.data = der->data;
    
    s2.data = res->data + head->len;
    
    while(b_left){
        if(b_left >= 48){
            s1.len = 48;
            b_left -= 48;
        }else{
            s1.len = b_left;
            b_left = 0;
        }
        
        dl_encode_base64(&s2, &s1, 1);
        
        s1.data += s1.len;
        s2.data += s2.len;
        
        *s2.data++ = '\n';
    }
    
    memcpy(s2.data, tail->data, tail->len);
    
    return res;
}







static inline int dl_der_get_item(uchar *data, dl_str *res)
{
    int bs;
    uchar   *cur, *tmp;
    int32_t     len = 0;
    
    tmp = (uchar *)&len;
    
    
    cur = data;
    if(*cur++ != 2) return -1;
    
    if(*cur < 128){
        len = *cur++;
    }else{
        bs = *cur & 0x7f;
        
        if(bs == 1){
            if(__BYTE_ORDER == __LITTLE_ENDIAN){
                tmp[0] = *(cur+1);
            }else{
                tmp[3] = *cur;
            }
        }else if(bs == 2){
            if(__BYTE_ORDER == __LITTLE_ENDIAN){
                tmp[0] = *(cur+2);
                tmp[1] = *(cur+1);
            }else{
                tmp[2] = *(cur+1);
                tmp[3] = *(cur+2);
            }
        }else{
            if(__BYTE_ORDER == __LITTLE_ENDIAN){
                tmp[0] = *(cur+3);
                tmp[1] = *(cur+2);
                tmp[2] = *(cur+1);
            }else{
                tmp[1] = *(cur+1);
                tmp[2] = *(cur+2);
                tmp[3] = *(cur+3);
            }
        }
        
        cur += bs + 1;
    }
    
    res->data = cur;
    res->len = len;
    
    return 0;
}


int dlc_rsa_keypair_from_der(dl_str *der, rsa_public_key *pub, rsa_private_key *key)
{
    int     bs, rc;
    dl_str  s1, s2;
    uchar   *cur, *last;
    
    cur = (uchar *)der->data;
    last = cur + der->len;
    
    if(*cur++ != 0x30) return -1;
    
    if(*cur < 128){
        cur++;
    }else{
        bs = *cur & 0x7f;
        cur += bs + 1;
    }
    
    // skip version
    if(*cur == 2 && *(cur+1) == 1)
        cur += 3;
    
    if(cur > last) return -1;
    
    // n
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return -1;
    nettle_mpz_set_str_256_u(pub->n, s1.len, s1.data);
    cur = s1.data + s1.len;
    
    // e
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return -1;
    nettle_mpz_set_str_256_u(pub->e, s1.len, s1.data);
    cur = s1.data + s1.len;
    
    if(key == NULL) return 0;  // public key
    
    // d
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return -1;
    nettle_mpz_set_str_256_u(key->d, s1.len, s1.data);
    cur = s1.data + s1.len;
    
    // p
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return -1;
    nettle_mpz_set_str_256_u(key->p, s1.len, s1.data);
    cur = s1.data + s1.len;
    
    // q
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return -1;
    nettle_mpz_set_str_256_u(key->q, s1.len, s1.data);
    cur = s1.data + s1.len;
    
    // a
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return -1;
    nettle_mpz_set_str_256_u(key->a, s1.len, s1.data);
    cur = s1.data + s1.len;
    
    // b
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return -1;
    nettle_mpz_set_str_256_u(key->b, s1.len, s1.data);
    cur = s1.data + s1.len;
    
    // c
    rc = dl_der_get_item(cur, &s1);
    if(rc == -1) return -1;
    nettle_mpz_set_str_256_u(key->c, s1.len, s1.data);
    cur = s1.data + s1.len;
    
    if(cur != last) return -1;
    
    return 0;
}



static inline dlc_der_rep * dl_cal(mpz_t n)
{
    int     res_len;
    int     rc;
    size_t  wlen;
    uchar    *cur;
    
    dlc_der_rep *res = dl_alloc(sizeof(dlc_der_rep), NULL);
    if(res == NULL) return NULL;
    
    //获取有效位数并计算占用的字节数
    res->len = (mpz_sizeinbase(n, 2) - 1)  / 8 + 1;
    
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
        dl_memcpy_rev(cur, (char *)n->_mp_d, res->len);
    else
        memcpy(cur, n->_mp_d, res->len);
    
    res->len = res_len;
    //printf("%d\n", res->len);
    
    return res;
}

dl_str * dl_rsa_pub_to_der(dl_str *der, rsa_public_key *pub)
{
    /* RSAPublicKey ::= SEQUENCE {
            modulus           INTEGER,  -- n
            publicExponent    INTEGER   -- e
      }
    */
    
    int     len;
    int     bs;
    uchar   *buf, *cur;

    if(der == NULL){
        der = dl_alloc(sizeof(dl_str), NULL);
        if(der == NULL) return NULL;
    }
    
    dlc_der_rep *der_n = dl_cal(pub->n);
    dlc_der_rep *der_e = dl_cal(pub->e);
    
    len = der_n->len + der_e->len;
    
    if(len >= 128){
        
        if(len >> 15)
            bs = 3;
        else
            bs = 2;
        
        buf = dl_alloc(len + 2 + bs, NULL);
        cur = buf;
        if(cur == NULL) return NULL;
        
        der->len = len + 2 + bs;
        
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
        
        der->len = len + 2;
        
        *cur++ = 0x30;
        *cur++ = len;
    }
    
    memcpy(cur, der_n->data, der_n->len);
    cur += der_n->len;
    
    memcpy(cur, der_e->data, der_e->len);
    cur += der_e->len;
    der->data = (char *)buf;
    
    dl_free(der_n->data);
    dl_free(der_n);
    dl_free(der_e->data);
    dl_free(der_e);
    
    return der;
}

dl_str * dl_rsa_keypair_to_der(dl_str *der, rsa_public_key*pub, rsa_private_key *key)
{
    /* RSAPrivateKey ::= SEQUENCE {
         version           Version,
	 modulus           INTEGER,  -- n
	 publicExponent    INTEGER,  -- e
	 privateExponent   INTEGER,  -- d
	 prime1            INTEGER,  -- p
	 prime2            INTEGER,  -- q
	 exponent1         INTEGER,  -- d mod (p-1)
	 exponent2         INTEGER,  -- d mod (q-1)
	 coefficient       INTEGER,  -- (inverse of q) mod p
	 otherPrimeInfos   OtherPrimeInfos OPTIONAL
    }
  */
    
    int     len;
    int     bs;
    uchar   *buf, *cur;

    if(der == NULL){
        der = dl_alloc(sizeof(dl_str), NULL);
        if(der == NULL) return NULL;
    }
    
    dlc_der_rep *der_n = dl_cal(pub->n);
    dlc_der_rep *der_e = dl_cal(pub->e);
    dlc_der_rep *der_d = dl_cal(key->d);
    dlc_der_rep *der_p = dl_cal(key->p);
    dlc_der_rep *der_q = dl_cal(key->q);
    dlc_der_rep *der_c = dl_cal(key->c);
    
    dlc_der_rep *der_a = dl_cal(key->a);
    dlc_der_rep *der_b = dl_cal(key->b);
    
    len = der_n->len + der_e->len + der_d->len + 
          der_p->len + der_q->len + der_c->len + 
          der_a->len + der_b->len + 3;
    
    if(len >= 128){
        
        if(len >> 15)
            bs = 3;
        else
            bs = 2;
        
        buf = dl_alloc(len + 2 + bs, NULL);
        cur = buf;
        if(cur == NULL) return NULL;
        
        der->len = len + 2 + bs;
        
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
        
        der->len = len + 2;
        
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
    
    memcpy(cur, der_a->data, der_a->len);
    cur += der_a->len;
    
    memcpy(cur, der_b->data, der_b->len);
    cur += der_b->len;
    
    memcpy(cur, der_c->data, der_c->len);
    cur += der_c->len;
    
    der->data = (char *)buf;
    
    dl_free(der_n->data);
    dl_free(der_n);
    dl_free(der_e->data);
    dl_free(der_e);
    dl_free(der_d->data);
    dl_free(der_d);
    dl_free(der_p->data);
    dl_free(der_p);
    dl_free(der_q->data);
    dl_free(der_q);
    dl_free(der_a->data);
    dl_free(der_a);
    dl_free(der_b->data);
    dl_free(der_b);
    dl_free(der_c->data);
    dl_free(der_c);
    
    return der;
}
