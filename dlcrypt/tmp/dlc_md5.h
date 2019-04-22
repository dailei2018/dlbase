#ifndef __DLC_MD5__
#define __DLC_MD5__

#define MD5_DATA_LENGTH 16

/* MD5 functions */

#define F1(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define F2(x, y, z) F1((z), (x), (y))
#define F3(x, y, z) ((x) ^ (y) ^ (z))
#define F4(x, y, z) ((y) ^ ((x) | ~(z)))

#define ROUND(f, w, x, y, z, data, s) \
( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )


#define MD5_DIGEST_SIZE 16
#define MD5_DATA_SIZE 64

/* Digest is kept internally as 4 32-bit words. */
#define _MD5_DIGEST_LENGTH 4

struct md5_ctx
{
  uint32_t digest[_MD5_DIGEST_LENGTH];
  uint32_t count_l, count_h;    /* Block count */
  uint8_t block[MD5_DATA_SIZE]; /* Block buffer */
  unsigned index;               /* Into buffer */
};

void
md5_init(struct md5_ctx *ctx);

void
md5_update(struct md5_ctx *ctx,
	   unsigned length,
	   const uint8_t *data);

void
md5_digest(struct md5_ctx *ctx,
	   unsigned length,
	   uint8_t *digest);

/* Internal compression function. STATE points to 4 uint32_t words,
   and DATA points to 64 bytes of input data, possibly unaligned. */
void
_nettle_md5_compress(uint32_t *state, const uint8_t *data);



#endif
