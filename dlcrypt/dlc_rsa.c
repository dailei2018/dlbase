#include "dlc_base.h"


void rsa_public_key_init(rsa_public_key *key)
{
  mpz_init(key->n);
  mpz_init(key->e);

  key->size = 0;
}

void rsa_private_key_init(rsa_private_key *key)
{
  mpz_init(key->d);
  mpz_init(key->p);
  mpz_init(key->q);
  mpz_init(key->a);
  mpz_init(key->b);
  mpz_init(key->c);

  key->size = 0;
}

void
rsa_private_key_clear(rsa_private_key *key)
{
  mpz_clear(key->d);
  mpz_clear(key->p);
  mpz_clear(key->q);
  mpz_clear(key->a);
  mpz_clear(key->b);
  mpz_clear(key->c);
}


void
rsa_public_key_clear(rsa_public_key *key)
{
  mpz_clear(key->n);
  mpz_clear(key->e);
}

int rsa_generate_keypair(rsa_public_key *pub, rsa_private_key *key, int nsize, int esize)
{
    int     size;
    mpz_t p1, q1, euler, gcd;

    if(esize > 0){
        /* we need set e */
        if(esize > nsize) return -1;
    
    }else{
        /* check e */
        /* It must be odd */
        if (!mpz_tstbit(pub->e, 0)) return -1;

        /* And 3 or larger */
        if (mpz_cmp_ui(pub->e, 3) < 0) return -1;
    }
    
    //if (nsize < RSA_MINIMUM_N_BITS) return -1;
    
    mpz_init(p1); mpz_init(q1); mpz_init(euler); mpz_init(gcd);
    
    
    for(;;){
        
        /* generate p */
        for(;;){
            bignum_random_prime(key->p, (nsize + 1) / 2);
            mpz_sub_ui(p1, key->p, 1);
            
            if(esize) break;
            
            /* e p-1 必须互质 */
            mpz_gcd(gcd, pub->e, p1);
            if (mpz_cmp_ui(gcd, 1) == 0)
                break;
        }
        
        /* generate q */
        for(;;){
            bignum_random_prime(key->q, nsize / 2);
            mpz_sub_ui(q1, key->q, 1);
            
            if(esize) break;
            
            /* e q-1 必须互质 */
            mpz_gcd(gcd, pub->e, q1);
            if (mpz_cmp_ui(gcd, 1) == 0)
                break;
            
        }
        
        /* Now we have the primes. Is the product of the right size? */
        mpz_mul(pub->n, key->p, key->q);
        
        /* We might get an n of size n_size-1. Then just try again. */
        size = mpz_sizeinbase(pub->n, 2);
        if (size != nsize)
            continue;
        
        /* This should succeed everytime. But if it doesn't, we try again. */
        if(!mpz_invert(key->c, key->q, key->p))
            continue;
        
        /* done */
        break;
    }
    
    /* φ(n) = φ(pq) = φ(p)φ(p) = (p - 1)(q - 1) */
    mpz_mul(euler, p1, q1);
    
    /* 如果还没有 e，则生成 */
    if(esize){
        for(;;){
            nettle_mpz_random_size(pub->e, esize);
	
            /* Make sure it's odd and that the most significant bit is set */
            mpz_setbit(pub->e, 0);
            mpz_setbit(pub->e, esize - 1);
            
            /* ed ≡ 1 mod φ(n) */
            if(!mpz_invert(key->d, pub->e, euler))
                continue;
            
            /* done */
            break;
        }
        
    }else{
        /*  已经有e，求模反(乘法逆元)
            ed ≡ 1 mod φ(n)
        */
        if(!mpz_invert(key->d, pub->e, euler)) return -1;
        
    }
    
    /* Done! Almost, we must compute the auxillary private values. */
    /* a = d % (p-1) */
    mpz_fdiv_r(key->a, key->d, p1);

    /* b = d % (q-1) */
    mpz_fdiv_r(key->b, key->d, q1);
    
    pub->size = key->size = (mpz_sizeinbase(pub->n, 2) + 7) / 8;
  
    mpz_clear(p1); mpz_clear(q1); mpz_clear(euler); mpz_clear(gcd);
    
    return 0;
}


/* Computing an rsa root. */
void
rsa_compute_root(const rsa_private_key *key,
		 mpz_t x, const mpz_t m)
{
  mpz_t xp; /* modulo p */
  mpz_t xq; /* modulo q */

  mpz_init(xp); mpz_init(xq);    

  /* Compute xq = m^d % q = (m%q)^b % q */
  mpz_fdiv_r(xq, m, key->q);
  mpz_powm(xq, xq, key->b, key->q);

  /* Compute xp = m^d % p = (m%p)^a % p */
  mpz_fdiv_r(xp, m, key->p);
  mpz_powm(xp, xp, key->a, key->p);

  /* Set xp' = (xp - xq) c % p. */
  mpz_sub(xp, xp, xq);
  mpz_mul(xp, xp, key->c);
  mpz_fdiv_r(xp, xp, key->p);

  /* Finally, compute x = xq + q xp'
   *
   * To prove that this works, note that
   *
   *   xp  = x + i p,
   *   xq  = x + j q,
   *   c q = 1 + k p
   *
   * for some integers i, j and k. Now, for some integer l,
   *
   *   xp' = (xp - xq) c + l p
   *       = (x + i p - (x + j q)) c + l p
   *       = (i p - j q) c + l p
   *       = (i c + l) p - j (c q)
   *       = (i c + l) p - j (1 + kp)
   *       = (i c + l - j k) p - j
   *
   * which shows that xp' = -j (mod p). We get
   *
   *   xq + q xp' = x + j q + (i c + l - j k) p q - j q
   *              = x + (i c + l - j k) p q
   *
   * so that
   *
   *   xq + q xp' = x (mod pq)
   *
   * We also get 0 <= xq + q xp' < p q, because
   *
   *   0 <= xq < q and 0 <= xp' < p.
   */
  mpz_mul(x, key->q, xp);
  mpz_add(x, x, xq);

  mpz_clear(xp); mpz_clear(xq);
}

int rsa_encrypt(rsa_public_key *key, int length, char *message, mpz_t gibbberish)
{
    char *em;
    unsigned padding;
    unsigned i;
  
    /* The message is encoded as a string of the same length as the
     * modulo n, of the form
     *
     *   00 02 pad 00 message
     *
     * where padding should be at least 8 pseudorandomly generated
     * *non-zero* octets. */
  
    /* Message too long for this key. */
    if (length + 11 > key->size)
        return -1;

    /* At least 8 octets of random padding */
    padding = key->size - length - 3;
    assert(padding >= 8);
  
    TMP_ALLOC(em, key->size - 1);
    em[0] = 2;

    dlc_urandom(em + 1, padding);

    /* Replace 0-octets with 1 */
    for (i = 0; i<padding; i++)
        if (!em[i+1])
            em[i+1] = 1;

    em[padding+1] = 0;
    memcpy(em + padding + 2, message, length);
    
    /* char * to mpz  */
    nettle_mpz_set_str_256_u(gibbberish, key->size - 1, em);
    
    /* encrypt y = x ^ e mod n */
    mpz_powm(gibbberish, gibbberish, key->e, key->n);

    return 1;  
}


int
rsa_decrypt(rsa_private_key *key,
	    int *length, char *message,
	    const mpz_t gibberish)
{
    char        *em;
    char        *terminator;
    unsigned  padding;
    unsigned  message_length;
  
    mpz_t m;

    mpz_init(m);
    
    /* decrypt */
    rsa_compute_root(key, m, gibberish);

    /* mpz to char * */
    TMP_ALLOC(em, key->size);
    nettle_mpz_get_str_256(key->size, em, m);
    mpz_clear(m);

    /* Check format */
    if (em[0] || em[1] != 2)
        return -1;

    terminator = memchr(em + 2, 0, key->size - 2);

    if (!terminator)
        return -1;
  
    padding = terminator - (em + 2);
    if (padding < 8) return -1;

    message_length = key->size - 3 - padding;

    if (*length < message_length)
        return -1;
  
    memcpy(message, terminator + 1, message_length);
    *length = message_length;

    return 0;
}
