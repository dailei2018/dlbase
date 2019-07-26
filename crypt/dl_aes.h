#ifndef __DL_AES
#define __DL_AES

#include "dl_base.h"


#include <openssl/aes.h>

enum {
    DL_AES_ECB,
    DL_AES_CBC,
    DL_AES_CFB,
    DL_AES_OFB,
    DL_AES_CRT
};

dl_str *dl_aes_encrypt(char *src, size_t len, char *key, size_t key_len, int type);
dl_str *dl_aes_decrypt(char *src, size_t len, char *key, size_t key_len, int type);

#endif