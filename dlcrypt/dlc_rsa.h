#ifndef __DLC_RSA__
#define __DLC_RSA__


/* For PKCS#1 to make sense, the size of the modulo, in octets, must
 * be at least 11 + the length of the DER-encoded Digest Info.
 *
 * And a DigestInfo is 34 octets for md5, 35 octets for sha1, and 51
 * octets for sha256. 62 octets is 496 bits, and as the upper 7 bits
 * may be zero, the smallest useful size of n is 489 bits. */

#define RSA_MINIMUM_N_OCTETS 62
#define RSA_MINIMUM_N_BITS 489

typedef struct _rsa_public_key  rsa_public_key;
typedef struct _rsa_private_key rsa_private_key;
typedef struct _rsa_key_pair    rsa_key_pair;


struct _rsa_public_key
{
  /* Size of the modulo, in octets. This is also the size of all
   * signatures that are created or verified with this key. */
  unsigned size;
  
  /* Modulo */
  mpz_t n;

  /* Public exponent */
  mpz_t e;
};

struct _rsa_private_key
{
  unsigned size;

  /* d is filled in by the key generation function; otherwise it's
   * completely unused. */
  mpz_t d;
  
  /* The two factors */
  mpz_t p; mpz_t q;

  /* d % (p-1), i.e. a e = 1 (mod (p-1)) */
  mpz_t a;

  /* d % (q-1), i.e. b e = 1 (mod (q-1)) */
  mpz_t b;

  /* modular inverse of q , i.e. c q = 1 (mod p) */
  mpz_t c;
};

struct _rsa_key_pair{
    rsa_public_key      pub;
    rsa_private_key     key;
};

/*
gnupg
typedef struct
{
  gcry_mpi_t n;	    // public modulus
  gcry_mpi_t e;	    // public exponent
  gcry_mpi_t d;	    // e对于模数(q-1)(p-1)的模反元素
  gcry_mpi_t p;	    // prime  p < q
  gcry_mpi_t q;	    // prime  q.
  gcry_mpi_t u;	    // p对模数q的模反元素
} RSA_secret_key;

openssl
typedef struct
{
  gcry_mpi_t n;	    // public modulus
  gcry_mpi_t e;	    // public exponent
  gcry_mpi_t d;	    // e对于模数(q-1)(p-1)的模反元素 
  gcry_mpi_t p;	    // prime  p > q
  gcry_mpi_t q;	    // prime  q.
  gcry_mpi_t u;	    // q对模数p的模反元素
 
  gcry_mpi_t e1;    // d mod (p-1)
  gcry_mpi_t e2;    // d mod (q-1)
  
} RSA_secret_key_ex;

*/

void rsa_compute_root(const rsa_private_key *key, mpz_t x, const mpz_t m);

void rsa_public_key_init(rsa_public_key *key);
void rsa_private_key_init(rsa_private_key *key);
void rsa_private_key_clear(rsa_private_key *key);
void rsa_public_key_clear(rsa_public_key *key);


int rsa_generate_keypair(rsa_public_key *pub, rsa_private_key *key, int nsize, int esize);
int rsa_encrypt(rsa_public_key *key, int length, char *message, mpz_t gibbberish);
int
rsa_decrypt(rsa_private_key *key, int *length, char *message, const mpz_t gibberish);


int rsa_sha1_sign(rsa_private_key *key, char *msg, int len, mpz_t s);
int rsa_sha1_verify(rsa_public_key *key, char *msg, size_t len, mpz_t s);

int rsa_sha256_sign(rsa_private_key *key, char *msg, int len, mpz_t s);
int rsa_sha256_verify(rsa_public_key *key, char *msg, size_t len, mpz_t s);

int rsa_md5_sign(rsa_private_key *key, char *msg, int len, mpz_t s);
int rsa_md5_verify(rsa_public_key *key, char *msg, size_t len, mpz_t s);
#endif