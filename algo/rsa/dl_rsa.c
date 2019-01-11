#include "dl_rsa.h"

# define RSA_FLAG_FIPS_METHOD                    0x0400
# define RSA_FLAG_NON_FIPS_ALLOW                 0x0400
# define RSA_FLAG_CHECKED                        0x0800

static int rsa_ossl_init(RSA *rsa)
{
    rsa->flags |= RSA_FLAG_CACHE_PUBLIC | RSA_FLAG_CACHE_PRIVATE;
    return 1;
}

static RSA_METHOD rsa_pkcs1_ossl_meth = {
    "OpenSSL PKCS#1 RSA",
    NULL,
    NULL,     /* signature verification */
    NULL,    /* signing */
    NULL,
    NULL,
    NULL,            /* XXX probably we should not use Montgomery
                                 * if e == 3 */
    rsa_ossl_init,
    rsa_ossl_finish,
    RSA_FLAG_FIPS_METHOD,       /* flags */
    NULL,
    0,                          /* rsa_sign */
    0,                          /* rsa_verify */
    NULL,                       /* rsa_keygen */
    NULL                        /* rsa_multi_prime_keygen */
};

RSA *RSA_new(void)
{
    return RSA_new_method();
}

RSA *RSA_new_method()
{
    RSA *ret = calloc(1, sizeof(RSA));

    if (ret == NULL) {
        return NULL;
    }

    ret->references = 1;

    ret->meth = RSA_get_default_method();

    ret->flags = ret->meth->flags & ~RSA_FLAG_NON_FIPS_ALLOW;

    if ((ret->meth->init != NULL) && !ret->meth->init(ret)) {
        return NULL;
    }

    return ret;

}