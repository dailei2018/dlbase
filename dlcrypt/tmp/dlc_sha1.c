#include "dlc_base.h"

/* A block, treated as a sequence of 32-bit words. */
#define SHA1_DATA_LENGTH 16

/* The SHA f()-functions.  The f1 and f3 functions can be optimized to
   save one boolean operation each - thanks to Rich Schroeppel,
   rcs@cs.arizona.edu for discovering this */

/* #define f1(x,y,z) ( ( x & y ) | ( ~x & z ) )            Rounds  0-19 */
#define f1(x,y,z)   ( z ^ ( x & ( y ^ z ) ) )           /* Rounds  0-19 */
#define f2(x,y,z)   ( x ^ y ^ z )                       /* Rounds 20-39 */
/* #define f3(x,y,z) ( ( x & y ) | ( x & z ) | ( y & z ) ) Rounds 40-59 */
#define f3(x,y,z)   ( ( x & y ) | ( z & ( x | y ) ) )   /* Rounds 40-59 */
#define f4(x,y,z)   ( x ^ y ^ z )                       /* Rounds 60-79 */

/* The SHA Mysterious Constants */

#define K1  0x5A827999L                                 /* Rounds  0-19 */
#define K2  0x6ED9EBA1L                                 /* Rounds 20-39 */
#define K3  0x8F1BBCDCL                                 /* Rounds 40-59 */
#define K4  0xCA62C1D6L                                 /* Rounds 60-79 */

/* 32-bit rotate left - kludged with shifts */

#define ROTL(n,X)  ( ( (X) << (n) ) | ( (X) >> ( 32 - (n) ) ) )

/* The initial expanding function.  The hash function is defined over an
   80-word expanded input array W, where the first 16 are copies of the input
   data, and the remaining 64 are defined by

        W[ i ] = W[ i - 16 ] ^ W[ i - 14 ] ^ W[ i - 8 ] ^ W[ i - 3 ]

   This implementation generates these values on the fly in a circular
   buffer - thanks to Colin Plumb, colin@nyx10.cs.du.edu for this
   optimization.

   The updated SHA changes the expanding function by adding a rotate of 1
   bit.  Thanks to Jim Gillogly, jim@rand.org, and an anonymous contributor
   for this information */

#define expand(W,i) ( W[ i & 15 ] = \
		      ROTL( 1, ( W[ i & 15 ] ^ W[ (i - 14) & 15 ] ^ \
				 W[ (i - 8) & 15 ] ^ W[ (i - 3) & 15 ] ) ) )


/* The prototype SHA sub-round.  The fundamental sub-round is:

        a' = e + ROTL( 5, a ) + f( b, c, d ) + k + data;
        b' = a;
        c' = ROTL( 30, b );
        d' = c;
        e' = d;

   but this is implemented by unrolling the loop 5 times and renaming the
   variables ( e, a, b, c, d ) = ( a', b', c', d', e' ) each iteration.
   This code is then replicated 20 times for each of the 4 functions, using
   the next 20 values from the W[] array each time */

#define subRound(a, b, c, d, e, f, k, data) \
    ( e += ROTL( 5, a ) + f( b, c, d ) + k + data, b = ROTL( 30, b ) )

/* Perform the SHA transformation.  Note that this code, like MD5, seems to
   break some optimizing compilers due to the complexity of the expressions
   and the size of the basic block.  It may be necessary to split it into
   sections, e.g. based on the four subrounds. */

void
_nettle_sha1_compress(uint32_t *state, const uint8_t *input)
{
  uint32_t data[16];
  uint32_t A, B, C, D, E;     /* Local vars */
  int i;

  for (i = 0; i < 16; i++, input+= 4)
    {
      data[i] = READ_UINT32(input);
    }

  /* Set up first buffer and local data buffer */
  A = state[0];
  B = state[1];
  C = state[2];
  D = state[3];
  E = state[4];

  /* Heavy mangling, in 4 sub-rounds of 20 interations each. */
  subRound( A, B, C, D, E, f1, K1, data[ 0] );
  subRound( E, A, B, C, D, f1, K1, data[ 1] );
  subRound( D, E, A, B, C, f1, K1, data[ 2] );
  subRound( C, D, E, A, B, f1, K1, data[ 3] );
  subRound( B, C, D, E, A, f1, K1, data[ 4] );
  subRound( A, B, C, D, E, f1, K1, data[ 5] );
  subRound( E, A, B, C, D, f1, K1, data[ 6] );
  subRound( D, E, A, B, C, f1, K1, data[ 7] );
  subRound( C, D, E, A, B, f1, K1, data[ 8] );
  subRound( B, C, D, E, A, f1, K1, data[ 9] );
  subRound( A, B, C, D, E, f1, K1, data[10] );
  subRound( E, A, B, C, D, f1, K1, data[11] );
  subRound( D, E, A, B, C, f1, K1, data[12] );
  subRound( C, D, E, A, B, f1, K1, data[13] );
  subRound( B, C, D, E, A, f1, K1, data[14] );
  subRound( A, B, C, D, E, f1, K1, data[15] );
  subRound( E, A, B, C, D, f1, K1, expand( data, 16 ) );
  subRound( D, E, A, B, C, f1, K1, expand( data, 17 ) );
  subRound( C, D, E, A, B, f1, K1, expand( data, 18 ) );
  subRound( B, C, D, E, A, f1, K1, expand( data, 19 ) );

  subRound( A, B, C, D, E, f2, K2, expand( data, 20 ) );
  subRound( E, A, B, C, D, f2, K2, expand( data, 21 ) );
  subRound( D, E, A, B, C, f2, K2, expand( data, 22 ) );
  subRound( C, D, E, A, B, f2, K2, expand( data, 23 ) );
  subRound( B, C, D, E, A, f2, K2, expand( data, 24 ) );
  subRound( A, B, C, D, E, f2, K2, expand( data, 25 ) );
  subRound( E, A, B, C, D, f2, K2, expand( data, 26 ) );
  subRound( D, E, A, B, C, f2, K2, expand( data, 27 ) );
  subRound( C, D, E, A, B, f2, K2, expand( data, 28 ) );
  subRound( B, C, D, E, A, f2, K2, expand( data, 29 ) );
  subRound( A, B, C, D, E, f2, K2, expand( data, 30 ) );
  subRound( E, A, B, C, D, f2, K2, expand( data, 31 ) );
  subRound( D, E, A, B, C, f2, K2, expand( data, 32 ) );
  subRound( C, D, E, A, B, f2, K2, expand( data, 33 ) );
  subRound( B, C, D, E, A, f2, K2, expand( data, 34 ) );
  subRound( A, B, C, D, E, f2, K2, expand( data, 35 ) );
  subRound( E, A, B, C, D, f2, K2, expand( data, 36 ) );
  subRound( D, E, A, B, C, f2, K2, expand( data, 37 ) );
  subRound( C, D, E, A, B, f2, K2, expand( data, 38 ) );
  subRound( B, C, D, E, A, f2, K2, expand( data, 39 ) );

  subRound( A, B, C, D, E, f3, K3, expand( data, 40 ) );
  subRound( E, A, B, C, D, f3, K3, expand( data, 41 ) );
  subRound( D, E, A, B, C, f3, K3, expand( data, 42 ) );
  subRound( C, D, E, A, B, f3, K3, expand( data, 43 ) );
  subRound( B, C, D, E, A, f3, K3, expand( data, 44 ) );
  subRound( A, B, C, D, E, f3, K3, expand( data, 45 ) );
  subRound( E, A, B, C, D, f3, K3, expand( data, 46 ) );
  subRound( D, E, A, B, C, f3, K3, expand( data, 47 ) );
  subRound( C, D, E, A, B, f3, K3, expand( data, 48 ) );
  subRound( B, C, D, E, A, f3, K3, expand( data, 49 ) );
  subRound( A, B, C, D, E, f3, K3, expand( data, 50 ) );
  subRound( E, A, B, C, D, f3, K3, expand( data, 51 ) );
  subRound( D, E, A, B, C, f3, K3, expand( data, 52 ) );
  subRound( C, D, E, A, B, f3, K3, expand( data, 53 ) );
  subRound( B, C, D, E, A, f3, K3, expand( data, 54 ) );
  subRound( A, B, C, D, E, f3, K3, expand( data, 55 ) );
  subRound( E, A, B, C, D, f3, K3, expand( data, 56 ) );
  subRound( D, E, A, B, C, f3, K3, expand( data, 57 ) );
  subRound( C, D, E, A, B, f3, K3, expand( data, 58 ) );
  subRound( B, C, D, E, A, f3, K3, expand( data, 59 ) );

  subRound( A, B, C, D, E, f4, K4, expand( data, 60 ) );
  subRound( E, A, B, C, D, f4, K4, expand( data, 61 ) );
  subRound( D, E, A, B, C, f4, K4, expand( data, 62 ) );
  subRound( C, D, E, A, B, f4, K4, expand( data, 63 ) );
  subRound( B, C, D, E, A, f4, K4, expand( data, 64 ) );
  subRound( A, B, C, D, E, f4, K4, expand( data, 65 ) );
  subRound( E, A, B, C, D, f4, K4, expand( data, 66 ) );
  subRound( D, E, A, B, C, f4, K4, expand( data, 67 ) );
  subRound( C, D, E, A, B, f4, K4, expand( data, 68 ) );
  subRound( B, C, D, E, A, f4, K4, expand( data, 69 ) );
  subRound( A, B, C, D, E, f4, K4, expand( data, 70 ) );
  subRound( E, A, B, C, D, f4, K4, expand( data, 71 ) );
  subRound( D, E, A, B, C, f4, K4, expand( data, 72 ) );
  subRound( C, D, E, A, B, f4, K4, expand( data, 73 ) );
  subRound( B, C, D, E, A, f4, K4, expand( data, 74 ) );
  subRound( A, B, C, D, E, f4, K4, expand( data, 75 ) );
  subRound( E, A, B, C, D, f4, K4, expand( data, 76 ) );
  subRound( D, E, A, B, C, f4, K4, expand( data, 77 ) );
  subRound( C, D, E, A, B, f4, K4, expand( data, 78 ) );
  subRound( B, C, D, E, A, f4, K4, expand( data, 79 ) );

  /* Build message digest */
  state[0] += A;
  state[1] += B;
  state[2] += C;
  state[3] += D;
  state[4] += E;

}








/* SHA initial values */

#define h0init  0x67452301L
#define h1init  0xEFCDAB89L
#define h2init  0x98BADCFEL
#define h3init  0x10325476L
#define h4init  0xC3D2E1F0L

/* Initialize the SHA values */

void
sha1_init(struct sha1_ctx *ctx)
{
  /* Set the h-vars to their initial values */
  ctx->digest[ 0 ] = h0init;
  ctx->digest[ 1 ] = h1init;
  ctx->digest[ 2 ] = h2init;
  ctx->digest[ 3 ] = h3init;
  ctx->digest[ 4 ] = h4init;

  /* Initialize bit count */
  ctx->count_low = ctx->count_high = 0;
  
  /* Initialize buffer */
  ctx->index = 0;
}

#define SHA1_INCR(ctx) ((ctx)->count_high += !++(ctx)->count_low)

void
sha1_update(struct sha1_ctx *ctx,
	    unsigned length, const uint8_t *buffer)
{
  if (ctx->index)
    { /* Try to fill partial block */
      unsigned left = SHA1_DATA_SIZE - ctx->index;
      if (length < left)
	{
	  memcpy(ctx->block + ctx->index, buffer, length);
	  ctx->index += length;
	  return; /* Finished */
	}
      else
	{
	  memcpy(ctx->block + ctx->index, buffer, left);

	  _nettle_sha1_compress(ctx->digest, ctx->block);
	  SHA1_INCR(ctx);

	  buffer += left;
	  length -= left;
	}
    }
  while (length >= SHA1_DATA_SIZE)
    {
      _nettle_sha1_compress(ctx->digest, buffer);
      SHA1_INCR(ctx);

      buffer += SHA1_DATA_SIZE;
      length -= SHA1_DATA_SIZE;
    }
  if ((ctx->index = length))     /* This assignment is intended */
    /* Buffer leftovers */
    memcpy(ctx->block, buffer, length);
}
	  
/* Final wrapup - pad to SHA1_DATA_SIZE-byte boundary with the bit pattern
   1 0* (64-bit count of bits processed, MSB-first) */

static void
sha1_final(struct sha1_ctx *ctx)
{
  uint32_t bitcount_high;
  uint32_t bitcount_low;
  unsigned i;
  
  i = ctx->index;
  
  /* Set the first char of padding to 0x80.  This is safe since there is
     always at least one byte free */

  assert(i < SHA1_DATA_SIZE);
  ctx->block[i++] = 0x80;

  if (i > (SHA1_DATA_SIZE - 8))
    { /* No room for length in this block. Process it and
	 pad with another one */
      memset(ctx->block + i, 0, SHA1_DATA_SIZE - i);
      
      _nettle_sha1_compress(ctx->digest, ctx->block);
      i = 0;
    }
  if (i < (SHA1_DATA_SIZE - 8))
    memset(ctx->block + i, 0, (SHA1_DATA_SIZE - 8) - i);

  /* There are 512 = 2^9 bits in one block */  
  bitcount_high = (ctx->count_high << 9) | (ctx->count_low >> 23);
  bitcount_low = (ctx->count_low << 9) | (ctx->index << 3);

  /* This is slightly inefficient, as the numbers are converted to
     big-endian format, and will be converted back by the compression
     function. It's probably not worth the effort to fix this. */
  WRITE_UINT32(ctx->block + (SHA1_DATA_SIZE - 8), bitcount_high);
  WRITE_UINT32(ctx->block + (SHA1_DATA_SIZE - 4), bitcount_low);

  _nettle_sha1_compress(ctx->digest, ctx->block);
}

void
sha1_digest(struct sha1_ctx *ctx,
	    unsigned length,
	    uint8_t *digest)
{
  unsigned i;
  unsigned words;
  unsigned leftover;
  
  assert(length <= SHA1_DIGEST_SIZE);

  sha1_final(ctx);
  
  words = length / 4;
  leftover = length % 4;

  for (i = 0; i < words; i++, digest += 4)
    WRITE_UINT32(digest, ctx->digest[i]);

  if (leftover)
    {
      uint32_t word;
      unsigned j = leftover;
      
      assert(i < _SHA1_DIGEST_LENGTH);
      
      word = ctx->digest[i];
      
      switch (leftover)
	{
	default:
	  abort();
	case 3:
	  digest[--j] = (word >> 8) & 0xff;
	  /* Fall through */
	case 2:
	  digest[--j] = (word >> 16) & 0xff;
	  /* Fall through */
	case 1:
	  digest[--j] = (word >> 24) & 0xff;
	}
    }
  sha1_init(ctx);
}
