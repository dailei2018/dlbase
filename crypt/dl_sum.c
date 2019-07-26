#include "dl_base.h"
#include "dl_sum.h"

// 16
char *dl_md5(char *dst, char *str, size_t len, int bin)
{
    uchar       digest[16];
    MD5_CTX     context;
    
    MD5_Init(&context);
    MD5_Update(&context, str, len);
    MD5_Final(digest, &context);
    
    if (bin) {
        memcpy(dst, digest, 16);
        return dst;
    }
    
    int i;
    for(i = 0; i < 16; i++){
        sprintf(dst, "%02x", digest[i]);
        dst += 2;
    }
    
    return dst;
}

// 20
char *dl_sha1(char *dst, char *str, size_t len, int bin)
{
    SHA_CTX         ctx;
    uchar           digest[SHA_DIGEST_LENGTH];
    int             s,i,hsize;
    
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, str, len);
    SHA1_Final(digest, &ctx);
    
    if(bin){
        memcpy(dst, digest, SHA_DIGEST_LENGTH);
        return dst;
    }
    
    for(i = 0; i < SHA_DIGEST_LENGTH; ++i){
        sprintf(dst, "%02x", digest[i]);
        dst += 2;
    }
    
    return dst;
}

// 32
char *dl_sha256(char *dst, char *str, size_t len, int bin)
{
    SHA256_CTX          ctx;
    uchar               digest[SHA256_DIGEST_LENGTH];
    int                 s,i,hsize;
    
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, str, len);
    SHA256_Final(digest, &ctx);
    
    if(bin){
        memcpy(dst, digest, SHA256_DIGEST_LENGTH);
        return dst;
    }
    
    for(i = 0; i < SHA256_DIGEST_LENGTH; ++i){
        sprintf(dst, "%02x", digest[i]);
        dst += 2;
    }
    
    return dst;
}

// 64
char *dl_sha512(char *dst, char *str, size_t len, int bin)
{
    SHA512_CTX          ctx;
    uchar               digest[SHA512_DIGEST_LENGTH];
    int                 s,i,hsize;
    
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, str, len);
    SHA512_Final(digest, &ctx);
    
    if(bin){
        memcpy(dst, digest, SHA512_DIGEST_LENGTH);
        return dst;
    }
    
    for(i = 0; i < SHA512_DIGEST_LENGTH; ++i){
        sprintf(dst, "%02x", digest[i]);
        dst += 2;
    }
    
    return dst;
}



// 16
char *dl_md5_hmac(char *dst, char *str, size_t len, char *key, size_t key_len, int bin)
{
    uchar       digest[16];
    HMAC_CTX    ctx;
    int         i;
    
    HMAC_CTX_init(&ctx);
 
    // Using sha1 hash engine here.
    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
    HMAC_Init_ex(&ctx, key, key_len, EVP_md5(), NULL);
    HMAC_Update(&ctx, str, len);
    HMAC_Final(&ctx, digest, NULL);
    
    if(bin){
        memcpy(dst, digest, 16);
        return dst;
    }
    
    for(i = 0; i < 16; ++i){
        sprintf(dst, "%02x", digest[i]);
        dst += 2;
    }
    
    return dst;
}

// 20
char *dl_sha1_hmac(char *dst, char *str, size_t len, char *key, size_t key_len, int bin)
{
    uchar       digest[SHA_DIGEST_LENGTH];
    HMAC_CTX    ctx;
    int         i;
    
    HMAC_CTX_init(&ctx);
 
    // Using sha1 hash engine here.
    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
    HMAC_Init_ex(&ctx, key, key_len, EVP_sha1(), NULL);
    HMAC_Update(&ctx, str, len);
    HMAC_Final(&ctx, digest, NULL);
    
    if(bin){
        memcpy(dst, digest, SHA_DIGEST_LENGTH);
        return dst;
    }
    
    for(i = 0; i < SHA_DIGEST_LENGTH; ++i){
        sprintf(dst, "%02x", digest[i]);
        dst += 2;
    }
    
    return dst;
}

// 32
char *dl_sha256_hmac(char *dst, char *str, size_t len, char *key, size_t key_len, int bin)
{
    uchar       digest[SHA256_DIGEST_LENGTH];
    HMAC_CTX    ctx;
    int         i;
    
    HMAC_CTX_init(&ctx);
 
    // Using sha1 hash engine here.
    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
    HMAC_Init_ex(&ctx, key, key_len, EVP_sha256(), NULL);
    HMAC_Update(&ctx, str, len);
    HMAC_Final(&ctx, digest, NULL);
    
    if(bin){
        memcpy(dst, digest, SHA256_DIGEST_LENGTH);
        return dst;
    }
    
    for(i = 0; i < SHA256_DIGEST_LENGTH; ++i){
        sprintf(dst, "%02x", digest[i]);
        dst += 2;
    }
    
    return dst;
}

// 64
char *dl_sha512_hmac(char *dst, char *str, size_t len, char *key, size_t key_len, int bin)
{
    uchar       digest[SHA512_DIGEST_LENGTH];
    HMAC_CTX    ctx;
    int         i;
    
    HMAC_CTX_init(&ctx);
 
    // Using sha1 hash engine here.
    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
    HMAC_Init_ex(&ctx, key, key_len, EVP_sha512(), NULL);
    HMAC_Update(&ctx, str, len);
    HMAC_Final(&ctx, digest, NULL);
    
    if(bin){
        memcpy(dst, digest, SHA512_DIGEST_LENGTH);
        return dst;
    }
    
    for(i = 0; i < SHA512_DIGEST_LENGTH; ++i){
        sprintf(dst, "%02x", digest[i]);
        dst += 2;
    }
    
    return dst;
}