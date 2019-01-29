#ifndef __DL_RSA
#define __DL_RSA

#include "dl_base.h"
#include "gcrypt.h"


struct gcry_mpi
{
  int alloced;         /* Array size (# of allocated limbs). */
  int nlimbs;          /* Number of valid limbs. */
  int sign;	       /* Indicates a negative number and is also used
		          for opaque MPIs to store the length.  */
  unsigned int flags; /* Bit 0: Array to be allocated in secure memory space.*/
                      /* Bit 2: The limb is a pointer to some m_alloced data.*/
                      /* Bit 4: Immutable MPI - the MPI may not be modified.  */
                      /* Bit 5: Constant MPI - the MPI will not be freed.  */
  uintptr_t *d;      /* Array with the limbs */
};
//typedef struct{
//  int alloced;         /* Array size (# of allocated limbs). */
//  int nlimbs;          /* Number of valid limbs. */
//  int sign;	       /* Indicates a negative number and is also used
//		          for opaque MPIs to store the length.  */
//  unsigned int      flags; /* Bit 0: Array to be allocated in secure memory space.*/
                      /* Bit 2: The limb is a pointer to some m_alloced data.*/
                      /* Bit 4: Immutable MPI - the MPI may not be modified.  */
                      /* Bit 5: Constant MPI - the MPI will not be freed.  */
//  unsigned long     *d;      /* Array with the limbs */
//} gcry_mpi_t;

typedef struct{
    int bs;
    int len;
    char *data;
} der_rep;

typedef struct
{
  gcry_mpi_t n;	    /* modulus */
  gcry_mpi_t e;	    /* exponent */
} RSA_public_key;

typedef struct
{
  gcry_mpi_t n;	    /* public modulus */
  gcry_mpi_t e;	    /* public exponent */
  gcry_mpi_t d;	    /* e对于模数(q-1)(p-1)的模反元素 */
  gcry_mpi_t p;	    /* prime  p < q */
  gcry_mpi_t q;	    /* prime  q. */
  gcry_mpi_t u;	    /* p对模数q的模反元素*/
} RSA_secret_key;

typedef struct
{
  gcry_mpi_t n;	    /* public modulus */
  gcry_mpi_t e;	    /* public exponent */
  gcry_mpi_t d;	    /* e对于模数(q-1)(p-1)的模反元素 */
  gcry_mpi_t p;	    /* prime  p > q */
  gcry_mpi_t q;	    /* prime  q. */
  gcry_mpi_t u;	    /* q对模数p的模反元素*/
  
  gcry_mpi_t e1;    // d mod (p-1)
  gcry_mpi_t e2;    // d mod (q-1)
  
} RSA_secret_key_ex;

typedef struct{
    RSA_secret_key  key;
} dl_rsa;

RSA_secret_key * dl_rsa_key_gen(size_t size, RSA_secret_key *key, dl_log *log);
RSA_secret_key_ex * dl_rsa_keyEx_gen(size_t size, RSA_secret_key_ex *key_ex, dl_log *log);
int dl_rsa_gen_sexp(gcry_sexp_t *sexp, int size, dl_log *log);
int dl_sexp_to_rsa(RSA_secret_key *k, gcry_sexp_t sexp);
int dl_rsa_to_sexp(gcry_sexp_t *sexp, RSA_secret_key *k);

void dl_free_rsa(void *k, int ex);
void dl_free_sexp(gcry_sexp_t sexp);

int dl_openssl_pkcs1_to_rsa(char *secret, RSA_secret_key *k);
int dl_rsa_to_rsaEx(RSA_secret_key_ex *key_ex, RSA_secret_key *key);
int dl_rsaEx_to_rsa(RSA_secret_key *key, RSA_secret_key_ex *key_ex);

dl_str * dl_rsaPub_to_der(RSA_public_key *key, dl_str *str);
dl_str * dl_rsaEx_to_der(RSA_secret_key_ex *key_ex, dl_str *str);
dl_str * dl_rsaDer_to_pem(dl_str *der, int is_pub);
dl_str *dl_rsaPem_to_der(char *data, size_t len);

RSA_secret_key_ex *dl_der_to_rsaEx(dl_str *der, RSA_secret_key_ex *key);

#endif