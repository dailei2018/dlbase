#ifndef __DL_SUM
#define __DL_SUM

#include "dl_base.h"


#include <openssl/hmac.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

enum {
    DL_SHA_1,
    DL_SHA_256,
    DL_SHA_512
};

char *dl_md5(char *dst, char *str, size_t len, int bin);
char *dl_sha1(char *dst, char *str, size_t len, int bin);
char *dl_sha256(char *dst, char *str, size_t len, int bin);
char *dl_sha512(char *dst, char *str, size_t len, int bin);

char *dl_md5_hmac(char *dst, char *str, size_t len, char *key, size_t key_len, int bin);
char *dl_sha1_hmac(char *dst, char *str, size_t len, char *key, size_t key_len, int bin);
char *dl_sha256_hmac(char *dst, char *str, size_t len, char *key, size_t key_len, int bin);
char *dl_sha512_hmac(char *dst, char *str, size_t len, char *key, size_t key_len, int bin);

#endif