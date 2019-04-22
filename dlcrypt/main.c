#include "dlc_base.h"

int main(){
    dl_str      *res, *pem;
    char        *n, *e, *d, *q, *p, *a, *b, *c, *tmp;
    
    dl_pool *pool = dl_create_pool(1024, NULL);
    
    res = dlc_rsa_pem_to_der("/root/openssl/rsa.pem");
    
    rsa_public_key      pub;
    rsa_private_key     key;
    
    rsa_public_key_init(&pub);
    rsa_private_key_init(&key);
    
    //dlc_read_from_der(res, &pub, &key);
    
    dlc_rsa_keypair_from_der(res, &pub, &key);
    
    
    n = mpz_get_str(NULL, 10, pub.n);
    e = mpz_get_str(NULL, 10, pub.e);
    d = mpz_get_str(NULL, 10, key.d);
    q = mpz_get_str(NULL, 10, key.q);
    p = mpz_get_str(NULL, 10, key.p);
    
    printf("n: %s\n\n", n);
    printf("e: %s\n\n", e);
    
    printf("d: %s\n\n", d);
    printf("q: %s\n\n", q);
    printf("p: %s\n\n\n", p);
    
    tmp = dl_pbc2hex(res->data, res->len, NULL, pool);
    //printf("%s\n", tmp);
    
    res = dl_rsa_keypair_to_der(NULL, &pub, &key);
    
    tmp = dl_pbc2hex(res->data, res->len, NULL, pool);
    //printf("%s\n", tmp);
    
    pem = dlc_rsa_der_to_pem(res, 0);
    
    dl_printf("%V", pem);
    
    return 0;
}