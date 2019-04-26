#ifndef __DLC_DES__
#define __DLC_DES__

#include "dlc_base.h"

#define DLC_DES_BLOCK_SIZE 8

    
#define    DLC_CIPHER_CBC_CTS  4   /* Enable CBC cipher text stealing (CTS). */
#define    DLC_CIPHER_CBC_MAC  8   /* Enable CBC message auth. code (MAC). */

#define dlc_des_encrypt(ctx, from, to)	        dlc_des_crypt(ctx, from, to, 0)
#define dlc_des_decrypt(ctx, from, to)	        dlc_des_crypt(ctx, from, to, 1)

#define dlc_des_ecb_encrypt(ctx, from, len, to)	        _dlc_des_ecb_encrypt(ctx, from, len, to, 0)
#define dlc_des_ecb_decrypt(ctx, from, len, to)	        _dlc_des_ecb_decrypt(ctx, from, len, to, 0)

#define dlc_des_cbc_encrypt(ctx, from, len, to)	        _dlc_des_cbc_encrypt(ctx, from, len, to, 0)
#define dlc_des_cbc_decrypt(ctx, from, len, to)	        _dlc_des_cbc_decrypt(ctx, from, len, to, 0)

#define dlc_des_cfb_encrypt(ctx, from, len, to)	        _dlc_des_cfb_encrypt(ctx, from, len, to, 0)
#define dlc_des_cfb_decrypt(ctx, from, len, to)	        _dlc_des_cfb_decrypt(ctx, from, len, to, 0)

#define dlc_des_ofb_encrypt(ctx, from, len, to)	        _dlc_des_ofb_encrypt(ctx, from, len, to, 0)
#define dlc_des_ofb_decrypt(ctx, from, len, to)	        _dlc_des_ofb_decrypt(ctx, from, len, to, 0)

#define dlc_des_crt_encrypt(ctx, from, len, to)	        _dlc_des_crt_encrypt(ctx, from, len, to, 0)
#define dlc_des_crt_decrypt(ctx, from, len, to)	        _dlc_des_crt_decrypt(ctx, from, len, to, 0)

typedef int (*dlc_des_crypt_func)(void *ctx, const char * from, char * to, int mode);


#define dlc_3des_encrypt(ctx, from, to)    dlc_3des_crypt(ctx,from,to,0)
#define dlc_3des_decrypt(ctx, from, to)    dlc_3des_crypt(ctx,from,to,1)

#define dlc_3des_ecb_encrypt(ctx, from, len, to)	        _dlc_des_ecb_encrypt(ctx, from, len, to, 1)
#define dlc_3des_ecb_decrypt(ctx, from, len, to)	        _dlc_des_ecb_decrypt(ctx, from, len, to, 1)

#define dlc_3des_cbc_encrypt(ctx, from, len, to)	        _dlc_des_cbc_encrypt(ctx, from, len, to, 1)
#define dlc_3des_cbc_decrypt(ctx, from, len, to)	        _dlc_des_cbc_decrypt(ctx, from, len, to, 1)

#define dlc_3des_cfb_encrypt(ctx, from, len, to)	        _dlc_des_cfb_encrypt(ctx, from, len, to, 1)
#define dlc_3des_cfb_decrypt(ctx, from, len, to)	        _dlc_des_cfb_decrypt(ctx, from, len, to, 1)

#define dlc_3des_ofb_encrypt(ctx, from, len, to)	        _dlc_des_ofb_encrypt(ctx, from, len, to, 1)
#define dlc_3des_ofb_decrypt(ctx, from, len, to)	        _dlc_des_ofb_decrypt(ctx, from, len, to, 1)

#define dlc_3des_crt_encrypt(ctx, from, len, to)	        _dlc_des_crt_encrypt(ctx, from, len, to, 1)
#define dlc_3des_crt_decrypt(ctx, from, len, to)	        _dlc_des_crt_decrypt(ctx, from, len, to, 1)

typedef struct _des_ctx{
    char        vi[8];
    int         flag;
    uint32_t encrypt_subkeys[32];
    uint32_t decrypt_subkeys[32];
    
} des_ctx;

typedef struct _tripledes_ctx{
    char        vi[8];
    int         flag;
    uint32_t encrypt_subkeys[96];
    uint32_t decrypt_subkeys[96];
    struct {
      int no_weak_key;
    } flags;
    
} tripledes_ctx;


int dlc_des_setkey(des_ctx *ctx, const char * key);
int dlc_des_crypt(des_ctx *ctx, const char * from, char * to, int mode);

dl_str * _dlc_des_ecb_encrypt(void *ctx, char * src, int slen, char * to, int triple);
dl_str * _dlc_des_ecb_decrypt(void *ctx, char * src, int slen, char * to, int triple);

dl_str * _dlc_des_cbc_encrypt(void *ctx, char * src, int slen, char * to, int triple);
dl_str * _dlc_des_cbc_decrypt(void *ctx, char * src, int slen, char * to, int triple);

dl_str * _dlc_des_cfb_encrypt(void *ctx, char * src, int slen, char * to, int triple);
dl_str * _dlc_des_cfb_decrypt(void *ctx, char * src, int slen, char * to, int triple);

dl_str * _dlc_des_ofb_encrypt(void *ctx, char * src, int slen, char * to, int triple);
dl_str * _dlc_des_ofb_decrypt(void *ctx, char * src, int slen, char * to, int triple);

dl_str * _dlc_des_crt_encrypt(void *ctx, char * src, int slen, char * to, int triple);
dl_str * _dlc_des_crt_decrypt(void *ctx, char * src, int slen, char * to, int triple);

void dlc_des_set_vector(void *ctx, char *vi);

int
dlc_3des_set3keys (tripledes_ctx *ctx, const char * key1, const char * key2, const char * key3);
int dlc_3des_crypt (tripledes_ctx *ctx, const char * from, char * to, int mode);

#endif