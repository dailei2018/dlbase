#ifndef __DLC_AES__
#define __DLC_AES__

#include <stdint.h>
#include <assert.h>

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
  
  char      vi[AES_BLOCK_SIZE];
};

void
dlc_aes_set_encrypt_key(aes_ctx *ctx,
		    unsigned length, const uint8_t *key);
void
dlc_aes_set_decrypt_key(aes_ctx *ctx,
		   unsigned length, const uint8_t *key);

void dlc_aes_encrypt(aes_ctx *ctx, char * src, int slen, char * to);
void dlc_aes_decrypt(aes_ctx *ctx, char * src, int slen, char * to);

dl_str * dlc_aes_ecb_encrypt(aes_ctx *ctx, char * src, int slen, char * to);
dl_str * dlc_aes_ecb_decrypt(aes_ctx *ctx, char * src, int slen, char * to);

dl_str * dlc_aes_cbc_encrypt(aes_ctx *ctx, char * src, int slen, char * to);
dl_str * dlc_aes_cbc_decrypt(aes_ctx *ctx, char * src, int slen, char * to);

dl_str * dlc_aes_cfb_encrypt(aes_ctx *ctx, char * src, int slen, char * to);
dl_str * dlc_aes_cfb_decrypt(aes_ctx *ctx, char * src, int slen, char * to);

dl_str * dlc_aes_ofb_encrypt(aes_ctx *ctx, char * src, int slen, char * to);
dl_str * dlc_aes_ofb_decrypt(aes_ctx *ctx, char * src, int slen, char * to);

dl_str * dlc_aes_crt_encrypt(aes_ctx *ctx, char * src, int slen, char * to);
dl_str * dlc_aes_crt_decrypt(aes_ctx *ctx, char * src, int slen, char * to);

#endif
