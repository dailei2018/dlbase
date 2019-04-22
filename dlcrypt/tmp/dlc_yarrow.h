#ifndef __DLC_YARROW__
#define __DLC_YARROW__

#include "dlc_base.h"


/* Obsolete alias for backwards compatibility. Will be deleted in some
   later version. */
#define yarrow256_force_reseed yarrow256_slow_reseed
  
enum yarrow_pool_id { YARROW_FAST = 0, YARROW_SLOW = 1 };

struct yarrow_source
{
  /* Indexed by yarrow_pool_id */
  uint32_t estimate[2];
  
  /* The pool next sample should go to. */
  enum yarrow_pool_id next;
};


#define YARROW256_SEED_FILE_SIZE (2 * AES_BLOCK_SIZE)

/* Yarrow-256, based on SHA-256 and AES-256 */


struct yarrow256_ctx
{
  /* Indexed by yarrow_pool_id */
  struct sha256_ctx pools[2];

  int seeded;

  /* The current key and counter block */
  aes_ctx key;
  uint8_t counter[AES_BLOCK_SIZE];

  /* The entropy sources */
  unsigned nsources;
  struct yarrow_source *sources;
};

void
yarrow256_init(struct yarrow256_ctx *ctx,
	       unsigned nsources,
	       struct yarrow_source *sources);

void
yarrow256_seed(struct yarrow256_ctx *ctx,
	       unsigned length,
	       const uint8_t *seed_file);

/* Returns 1 on reseed */
int
yarrow256_update(struct yarrow256_ctx *ctx,
		 unsigned source, unsigned entropy,
		 unsigned length, const uint8_t *data);

void
yarrow256_random(struct yarrow256_ctx *ctx, unsigned length, uint8_t *dst);

int
yarrow256_is_seeded(struct yarrow256_ctx *ctx);

unsigned
yarrow256_needed_sources(struct yarrow256_ctx *ctx);

void
yarrow256_fast_reseed(struct yarrow256_ctx *ctx);

void
yarrow256_slow_reseed(struct yarrow256_ctx *ctx);


/* Key event estimator */
#define YARROW_KEY_EVENT_BUFFER 16

struct yarrow_key_event_ctx
{
  /* Counter for initial priming of the state */
  unsigned index;
  unsigned chars[YARROW_KEY_EVENT_BUFFER];
  unsigned previous;
};

void
yarrow_key_event_init(struct yarrow_key_event_ctx *ctx);

unsigned
yarrow_key_event_estimate(struct yarrow_key_event_ctx *ctx,
			  unsigned key, unsigned time);
  

#endif
