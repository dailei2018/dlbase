#include "dlc_base.h"


void
_nettle_md5_compress(uint32_t *digest, const uint8_t *input)
{
  uint32_t data[MD5_DATA_LENGTH];
  uint32_t a, b, c, d;
  unsigned i;

  for (i = 0; i < MD5_DATA_LENGTH; i++, input += 4)
    data[i] = LE_READ_UINT32(input);

  a = digest[0];
  b = digest[1];
  c = digest[2];
  d = digest[3];

  ROUND(F1, a, b, c, d, data[ 0] + 0xd76aa478, 7);
  ROUND(F1, d, a, b, c, data[ 1] + 0xe8c7b756, 12);
  ROUND(F1, c, d, a, b, data[ 2] + 0x242070db, 17);
  ROUND(F1, b, c, d, a, data[ 3] + 0xc1bdceee, 22);
  ROUND(F1, a, b, c, d, data[ 4] + 0xf57c0faf, 7);
  ROUND(F1, d, a, b, c, data[ 5] + 0x4787c62a, 12);
  ROUND(F1, c, d, a, b, data[ 6] + 0xa8304613, 17);
  ROUND(F1, b, c, d, a, data[ 7] + 0xfd469501, 22);
  ROUND(F1, a, b, c, d, data[ 8] + 0x698098d8, 7);
  ROUND(F1, d, a, b, c, data[ 9] + 0x8b44f7af, 12);
  ROUND(F1, c, d, a, b, data[10] + 0xffff5bb1, 17);
  ROUND(F1, b, c, d, a, data[11] + 0x895cd7be, 22);
  ROUND(F1, a, b, c, d, data[12] + 0x6b901122, 7);
  ROUND(F1, d, a, b, c, data[13] + 0xfd987193, 12);
  ROUND(F1, c, d, a, b, data[14] + 0xa679438e, 17);
  ROUND(F1, b, c, d, a, data[15] + 0x49b40821, 22);

  ROUND(F2, a, b, c, d, data[ 1] + 0xf61e2562, 5);
  ROUND(F2, d, a, b, c, data[ 6] + 0xc040b340, 9);
  ROUND(F2, c, d, a, b, data[11] + 0x265e5a51, 14);
  ROUND(F2, b, c, d, a, data[ 0] + 0xe9b6c7aa, 20);
  ROUND(F2, a, b, c, d, data[ 5] + 0xd62f105d, 5);
  ROUND(F2, d, a, b, c, data[10] + 0x02441453, 9);
  ROUND(F2, c, d, a, b, data[15] + 0xd8a1e681, 14);
  ROUND(F2, b, c, d, a, data[ 4] + 0xe7d3fbc8, 20);
  ROUND(F2, a, b, c, d, data[ 9] + 0x21e1cde6, 5);
  ROUND(F2, d, a, b, c, data[14] + 0xc33707d6, 9);
  ROUND(F2, c, d, a, b, data[ 3] + 0xf4d50d87, 14);
  ROUND(F2, b, c, d, a, data[ 8] + 0x455a14ed, 20);
  ROUND(F2, a, b, c, d, data[13] + 0xa9e3e905, 5);
  ROUND(F2, d, a, b, c, data[ 2] + 0xfcefa3f8, 9);
  ROUND(F2, c, d, a, b, data[ 7] + 0x676f02d9, 14);
  ROUND(F2, b, c, d, a, data[12] + 0x8d2a4c8a, 20);

  ROUND(F3, a, b, c, d, data[ 5] + 0xfffa3942, 4);
  ROUND(F3, d, a, b, c, data[ 8] + 0x8771f681, 11);
  ROUND(F3, c, d, a, b, data[11] + 0x6d9d6122, 16);
  ROUND(F3, b, c, d, a, data[14] + 0xfde5380c, 23);
  ROUND(F3, a, b, c, d, data[ 1] + 0xa4beea44, 4);
  ROUND(F3, d, a, b, c, data[ 4] + 0x4bdecfa9, 11);
  ROUND(F3, c, d, a, b, data[ 7] + 0xf6bb4b60, 16);
  ROUND(F3, b, c, d, a, data[10] + 0xbebfbc70, 23);
  ROUND(F3, a, b, c, d, data[13] + 0x289b7ec6, 4);
  ROUND(F3, d, a, b, c, data[ 0] + 0xeaa127fa, 11);
  ROUND(F3, c, d, a, b, data[ 3] + 0xd4ef3085, 16);
  ROUND(F3, b, c, d, a, data[ 6] + 0x04881d05, 23);
  ROUND(F3, a, b, c, d, data[ 9] + 0xd9d4d039, 4);
  ROUND(F3, d, a, b, c, data[12] + 0xe6db99e5, 11);
  ROUND(F3, c, d, a, b, data[15] + 0x1fa27cf8, 16);
  ROUND(F3, b, c, d, a, data[ 2] + 0xc4ac5665, 23);

  ROUND(F4, a, b, c, d, data[ 0] + 0xf4292244, 6);
  ROUND(F4, d, a, b, c, data[ 7] + 0x432aff97, 10);
  ROUND(F4, c, d, a, b, data[14] + 0xab9423a7, 15);
  ROUND(F4, b, c, d, a, data[ 5] + 0xfc93a039, 21);
  ROUND(F4, a, b, c, d, data[12] + 0x655b59c3, 6);
  ROUND(F4, d, a, b, c, data[ 3] + 0x8f0ccc92, 10);
  ROUND(F4, c, d, a, b, data[10] + 0xffeff47d, 15);
  ROUND(F4, b, c, d, a, data[ 1] + 0x85845dd1, 21);
  ROUND(F4, a, b, c, d, data[ 8] + 0x6fa87e4f, 6);
  ROUND(F4, d, a, b, c, data[15] + 0xfe2ce6e0, 10);
  ROUND(F4, c, d, a, b, data[ 6] + 0xa3014314, 15);
  ROUND(F4, b, c, d, a, data[13] + 0x4e0811a1, 21);
  ROUND(F4, a, b, c, d, data[ 4] + 0xf7537e82, 6);
  ROUND(F4, d, a, b, c, data[11] + 0xbd3af235, 10);
  ROUND(F4, c, d, a, b, data[ 2] + 0x2ad7d2bb, 15);
  ROUND(F4, b, c, d, a, data[ 9] + 0xeb86d391, 21);

  digest[0] += a;
  digest[1] += b;
  digest[2] += c;
  digest[3] += d;
  
}

static void
md5_final(struct md5_ctx *ctx);

void
md5_init(struct md5_ctx *ctx)
{
  ctx->digest[0] = 0x67452301;
  ctx->digest[1] = 0xefcdab89;
  ctx->digest[2] = 0x98badcfe;
  ctx->digest[3] = 0x10325476;
  
  ctx->count_l = ctx->count_h = 0;
  ctx->index = 0;
}

#define MD5_INCR(ctx) ((ctx)->count_h += !++(ctx)->count_l)

void
md5_update(struct md5_ctx *ctx,
	   unsigned length,
	   const uint8_t *data)
{
  if (ctx->index)
    {
      /* Try to fill partial block */
      unsigned left = MD5_DATA_SIZE - ctx->index;
      if (length < left)
	{
	  memcpy(ctx->block + ctx->index, data, length);
	  ctx->index += length;
	  return; /* Finished */
	}
      else
	{
	  memcpy(ctx->block + ctx->index, data, left);

	  _nettle_md5_compress(ctx->digest, ctx->block);
	  MD5_INCR(ctx);
	  
	  data += left;
	  length -= left;
	}
    }
  while (length >= MD5_DATA_SIZE)
    {
      _nettle_md5_compress(ctx->digest, data);
      MD5_INCR(ctx);

      data += MD5_DATA_SIZE;
      length -= MD5_DATA_SIZE;
    }
  if ((ctx->index = length))     /* This assignment is intended */
    /* Buffer leftovers */
    memcpy(ctx->block, data, length);
}

void
md5_digest(struct md5_ctx *ctx,
	   unsigned length,
	   uint8_t *digest)
{
  unsigned i;
  unsigned words;
  unsigned leftover;
  
  assert(length <= MD5_DIGEST_SIZE);

  md5_final(ctx);
  
  words = length / 4;
  leftover = length % 4;
  
  /* Little endian order */
  for (i = 0; i < words; i++, digest += 4)
    LE_WRITE_UINT32(digest, ctx->digest[i]);

  if (leftover)
    {
      uint32_t word;
      unsigned j;

      assert(i < _MD5_DIGEST_LENGTH);
      
      /* Still least significant byte first. */
      for (word = ctx->digest[i], j = 0; j < leftover;
	   j++, word >>= 8)
	digest[j] = word & 0xff;
    }
  md5_init(ctx);
}

/* Final wrapup - pad to MD5_DATA_SIZE-byte boundary with the bit
 * pattern 1 0* (64-bit count of bits processed, LSB-first) */

static void
md5_final(struct md5_ctx *ctx)
{
  uint32_t bitcount_high;
  uint32_t bitcount_low;
  unsigned i;
  
  i = ctx->index;

  /* Set the first char of padding to 0x80. This is safe since there
   * is always at least one byte free */
  assert(i < MD5_DATA_SIZE);
  ctx->block[i++] = 0x80;

  if (i > (MD5_DATA_SIZE - 8))
    {
      /* No room for length in this block. Process it and
	 pad with another one */
      memset(ctx->block + i, 0, MD5_DATA_SIZE - i);
      
      _nettle_md5_compress(ctx->digest, ctx->block);
      i = 0;
    }
  if (i < (MD5_DATA_SIZE - 8))
    memset(ctx->block + i, 0, (MD5_DATA_SIZE - 8) - i);
    
  /* There are 512 = 2^9 bits in one block 
   * Little-endian order => Least significant word first */

  bitcount_low = (ctx->count_l << 9) | (ctx->index << 3);
  bitcount_high = (ctx->count_h << 9) | (ctx->count_l >> 23);
  LE_WRITE_UINT32(ctx->block + (MD5_DATA_SIZE - 8), bitcount_low);
  LE_WRITE_UINT32(ctx->block + (MD5_DATA_SIZE - 4), bitcount_high);
  
  _nettle_md5_compress(ctx->digest, ctx->block);
}
