
#ifndef NETTLE_BIGNUM_H_INCLUDED
#define NETTLE_BIGNUM_H_INCLUDED


#include <gmp.h>



#ifdef __cplusplus
extern "C" {
#endif

void
bignum_random_prime(mpz_t x, unsigned bits);

/* Size needed for signed encoding, including extra sign byte if
 * necessary. */
unsigned
nettle_mpz_sizeinbase_256_s(const mpz_t x);

/* Size needed for unsigned encoding */
unsigned
nettle_mpz_sizeinbase_256_u(const mpz_t x);

/* Writes an integer as length octets, using big endian byte order,
 * and two's complement for negative numbers. */
void
nettle_mpz_get_str_256(unsigned length, uint8_t *s, const mpz_t x);

/* Reads a big endian, two's complement, integer. */
void
nettle_mpz_set_str_256_s(mpz_t x,
			 unsigned length, const uint8_t *s);

void
nettle_mpz_init_set_str_256_s(mpz_t x,
			      unsigned length, const uint8_t *s);

/* Similar, but for unsigned format. These function don't interpret
 * the most significant bit as the sign. */
void
nettle_mpz_set_str_256_u(mpz_t x,
			 unsigned length, const uint8_t *s);

void
nettle_mpz_init_set_str_256_u(mpz_t x,
			      unsigned length, const uint8_t *s);

/* Returns a uniformly distributed random number 0 <= x < 2^n */
void
nettle_mpz_random_size(mpz_t x, unsigned bits);

/* Returns a number x, almost uniformly random in the range
 * 0 <= x < n. */
void
nettle_mpz_random(mpz_t x, const mpz_t n);

void
nettle_next_prime(mpz_t p, mpz_t n, unsigned count, unsigned prime_limit);

/* sexp parsing */
struct sexp_iterator;

/* If LIMIT is non-zero, the number must be at most LIMIT bits.
 * Implies sexp_iterator_next. */
int
nettle_mpz_set_sexp(mpz_t x, unsigned limit, struct sexp_iterator *i);


/* der parsing */
struct asn1_der_iterator;

int
nettle_asn1_der_get_bignum(struct asn1_der_iterator *iterator,
			   mpz_t x, unsigned limit);

#ifdef __cplusplus
}
#endif

#endif /* NETTLE_BIGNUM_H_INCLUDED */
