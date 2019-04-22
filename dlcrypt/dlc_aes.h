#ifndef __DLC_AES__
#define __DLC_AES__


#define AES_BLOCK_SIZE 16

/* Variable key size between 128 and 256 bits. But the only valid
 * values are 16 (128 bits), 24 (192 bits) and 32 (256 bits). */
#define AES_MIN_KEY_SIZE 16
#define AES_MAX_KEY_SIZE 32

#define AES_KEY_SIZE 32

typedef struct _aes_ctx aes_ctx;

struct _aes_ctx
{
  uint32_t keys[60];  /* maximum size of key schedule */
  unsigned  nrounds;  /* number of rounds to use for our key size */
};

void
aes_set_encrypt_key(aes_ctx *ctx,
		    unsigned length, const uint8_t *key);
void
aes_set_decrypt_key(aes_ctx *ctx,
		   unsigned length, const uint8_t *key);

void
aes_encrypt(aes_ctx *ctx,
	    unsigned length, uint8_t *dst,
	    const uint8_t *src);
void
aes_decrypt(aes_ctx *ctx,
	    unsigned length, uint8_t *dst,
	    const uint8_t *src);


#endif
