#include "dl_base.h"

#include "dl_aes.h"

dl_str *dl_aes_encrypt_cbc(char *src, size_t len, char *key)
{
    dl_str      *res;
    char        *data;
    AES_KEY     aes;
    
    char        iv[AES_BLOCK_SIZE];
    memset(iv, 0, AES_BLOCK_SIZE);
    
    data = dl_alloc(len + 1, NULL);
    if (data == NULL)
        return NULL;
    
    res = dl_alloc(sizeof(dl_str), NULL);
    if (res == NULL)
        return NULL;
    
    AES_set_encrypt_key(key,128,&aes);
    AES_cbc_encrypt(src, data, len, &aes, iv, AES_ENCRYPT);
    
    dl_free(src);
    
    res->data = data;
    res->len = len;
    
    return res;
}

dl_str *dl_aes_decrypt_cbc(char *src, size_t len, char *key)
{
    dl_str      *res;
    char        *data;
    AES_KEY     aes;
    char        iv[AES_BLOCK_SIZE];
    
    memset(iv, 0, AES_BLOCK_SIZE);
    
    data = dl_alloc(len, NULL);
    if (data == NULL)
        return NULL;
    
    res = dl_alloc(sizeof(dl_str), NULL);
    if (res == NULL)
        return NULL;
    
    AES_set_decrypt_key(key,128,&aes);
    AES_cbc_encrypt(src, data, len, &aes, iv, AES_DECRYPT);
    
    res->data = data;
    res->len = len - data[len - 1];
    
    return res;
}

dl_str *dl_aes_encrypt(char *src, size_t len, char *key, size_t key_len, int type)
{
    char        key_block[AES_BLOCK_SIZE], *src_pad, *cur;
    dl_str      *res;
    
    // set key
    memset(key_block, 0, AES_BLOCK_SIZE);
    if (key_len > AES_BLOCK_SIZE)
        memcpy(key_block, key, AES_BLOCK_SIZE);
    else
        memcpy(key_block, key, key_len);
    
    int r, pad_len, total;
    
    r = len % AES_BLOCK_SIZE;
    if (r == 0)
        pad_len = 0;
    else
        pad_len = AES_BLOCK_SIZE - r;
    
    total = (len / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    
    src_pad = dl_alloc(total, NULL);
    if (src_pad == NULL)
        return NULL;
    
    cur = src_pad;

    cur = dl_cpymem(cur, src, len);
    
    if (pad_len == 0)
        memset(cur, (uchar)AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    else
        memset(cur, (uchar)pad_len, pad_len);
    
    res = dl_alloc(sizeof(dl_str), NULL);
    if (res == NULL)
        return NULL;
    
    switch (type) {
    case DL_AES_CBC:
        return dl_aes_encrypt_cbc(src_pad, total, key_block);
        break;
    default:
        return dl_aes_encrypt_cbc(src_pad, total, key_block);
        break;
    }
    
    return NULL;
}

dl_str *dl_aes_decrypt(char *src, size_t len, char *key, size_t key_len, int type)
{
    char        key_block[AES_BLOCK_SIZE], *cur;
    dl_str      *res;
    
    // set key
    memset(key_block, 0, AES_BLOCK_SIZE);
    if (key_len > AES_BLOCK_SIZE)
        memcpy(key_block, key, AES_BLOCK_SIZE);
    else
        memcpy(key_block, key, key_len);
    
    switch (type) {
    case DL_AES_CBC:
        return dl_aes_decrypt_cbc(src, len, key_block);
        break;
    default:
        return dl_aes_decrypt_cbc(src, len, key_block);
        break;
    }
    
    
    return NULL;
}

