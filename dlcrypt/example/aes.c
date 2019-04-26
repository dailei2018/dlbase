#include "dlc_base.h"

/*
    key size can be 16 24 32
*/

void aes_test(){
    aes_ctx     ctx;
    char    *txt = "1234567890123456";
    char    *key = "abcdefghijklmnop";
    char    enc[16], res[16];
    
    dlc_aes_set_encrypt_key(&ctx, 16, key);
    dlc_aes_encrypt(&ctx, txt, 16, enc);
    
    dlc_aes_set_decrypt_key(&ctx, 16, key);
    dlc_aes_decrypt(&ctx, enc, 16, res);
    
}

void aes_ecb_test(){
    dl_str          *enc, *res, b64;
    aes_ctx         ctx;
    
    dl_str      txt = dl_string("Hi! How are you and what's the time");
    char *key = "abcdefghijklmnop";
    
    dlc_aes_set_encrypt_key(&ctx, 16, key);
    enc = dlc_aes_ecb_encrypt(&ctx, txt.data, txt.len, NULL);
    
    b64.data = dl_alloc(base64_len(enc->len), NULL);
    dl_encode_base64(&b64, enc, 1);
    
    dlc_aes_set_decrypt_key(&ctx, 16, key);
    res = dlc_aes_ecb_decrypt(&ctx, enc->data, enc->len, NULL);
    
    dl_str_free(enc);
    dl_str_free(res);
}

void aes_cbc_test(){
    dl_str          *enc, *res, b64;
    aes_ctx         ctx;
    
    dl_str      txt = dl_string("Hi! How are you and what's the time");
    char *key = "abcdefghijklmnop";
    
    dlc_urandom(ctx.vi, AES_BLOCK_SIZE);
    
    dlc_aes_set_encrypt_key(&ctx, 16, key);
    enc = dlc_aes_cbc_encrypt(&ctx, txt.data, txt.len, NULL);
    
    b64.data = dl_alloc(base64_len(enc->len), NULL);
    dl_encode_base64(&b64, enc, 1);
    
    dlc_aes_set_decrypt_key(&ctx, 16, key);
    res = dlc_aes_cbc_decrypt(&ctx, enc->data, enc->len, NULL);
    
    dl_str_free(enc);
    dl_str_free(res);
}

void aes_cfb_test(){
    dl_str          *enc, *res, b64;
    aes_ctx         ctx;
    
    dl_str      txt = dl_string("Hi! How are you and what's the time");
    char *key = "abcdefghijklmnop";
    
    dlc_urandom(ctx.vi, AES_BLOCK_SIZE);
    
    dlc_aes_set_encrypt_key(&ctx, 16, key);
    enc = dlc_aes_cfb_encrypt(&ctx, txt.data, txt.len, NULL);
    
    b64.data = dl_alloc(base64_len(enc->len), NULL);
    dl_encode_base64(&b64, enc, 1);
    
    //dlc_aes_set_decrypt_key(&ctx, 16, key);
    res = dlc_aes_cfb_decrypt(&ctx, enc->data, enc->len, NULL);
    
    dl_str_free(enc);
    dl_str_free(res);
}

void aes_ofb_test(){
    dl_str          *enc, *res, b64;
    aes_ctx         ctx;
    
    dl_str      txt = dl_string("Hi! How are you and what's the time");
    char *key = "abcdefghijklmnop";
    
    dlc_urandom(ctx.vi, AES_BLOCK_SIZE);
    
    dlc_aes_set_encrypt_key(&ctx, 16, key);
    enc = dlc_aes_ofb_encrypt(&ctx, txt.data, txt.len, NULL);
    
    b64.data = dl_alloc(base64_len(enc->len), NULL);
    dl_encode_base64(&b64, enc, 1);
    
    //dlc_aes_set_decrypt_key(&ctx, 16, key);
    res = dlc_aes_ofb_decrypt(&ctx, enc->data, enc->len, NULL);
    
    dl_str_free(enc);
    dl_str_free(res);
}

void aes_crt_test(){
    dl_str          *enc, *res, b64;
    aes_ctx         ctx;
    
    dl_str      txt = dl_string("Hi! How are you and what's the time");
    char *key = "abcdefghijklmnop";
    
    dlc_urandom(ctx.vi, AES_BLOCK_SIZE);
    
    dlc_aes_set_encrypt_key(&ctx, 16, key);
    enc = dlc_aes_crt_encrypt(&ctx, txt.data, txt.len, NULL);
    
    b64.data = dl_alloc(base64_len(enc->len), NULL);
    dl_encode_base64(&b64, enc, 1);
    
    //dlc_aes_set_decrypt_key(&ctx, 16, key);
    res = dlc_aes_crt_decrypt(&ctx, enc->data, enc->len, NULL);
    
    dl_str_free(enc);
    dl_str_free(res);
}

int main(){
    aes_test();
    aes_ecb_test();
    aes_cbc_test();
    aes_cfb_test();
    aes_ofb_test();
    aes_crt_test();
    
    return 1;
}