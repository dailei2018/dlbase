#ifndef __DLC_SHA__
#define __DLC_SHA__



/* SHA1 */

#define SHA1_DIGEST_SIZE 20
#define SHA1_DATA_SIZE 64

/* Digest is kept internally as 5 32-bit words. */
#define _SHA1_DIGEST_LENGTH 5

struct sha1_ctx
{
  uint32_t digest[_SHA1_DIGEST_LENGTH];   /* Message digest */
  uint32_t count_low, count_high;         /* 64-bit block count */
  uint8_t block[SHA1_DATA_SIZE];          /* SHA1 data buffer */
  unsigned int index;                     /* index into buffer */
};

void
sha1_init(struct sha1_ctx *ctx);

void
sha1_update(struct sha1_ctx *ctx,
	    unsigned length,
	    const uint8_t *data);

void
sha1_digest(struct sha1_ctx *ctx,
	    unsigned length,
	    uint8_t *digest);

/* Internal compression function. STATE points to 5 uint32_t words,
   and DATA points to 64 bytes of input data, possibly unaligned. */
void
_nettle_sha1_compress(uint32_t *state, const uint8_t *data);

/* SHA256 */

#define SHA256_DIGEST_SIZE 32
#define SHA256_DATA_SIZE 64

/* Digest is kept internally as 8 32-bit words. */
#define _SHA256_DIGEST_LENGTH 8

struct sha256_ctx
{
  uint32_t state[_SHA256_DIGEST_LENGTH];    /* State variables */
  uint32_t count_low, count_high;           /* 64-bit block count */
  uint8_t block[SHA256_DATA_SIZE];          /* SHA256 data buffer */
  unsigned int index;                       /* index into buffer */
};

void
sha256_init(struct sha256_ctx *ctx);

void
sha256_update(struct sha256_ctx *ctx,
	      unsigned length,
	      const uint8_t *data);

void
sha256_digest(struct sha256_ctx *ctx,
	      unsigned length,
	      uint8_t *digest);


#endif
