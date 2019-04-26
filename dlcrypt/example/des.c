#include "dlc_base.h"

void des_test(){
    des_ctx     ctx;
    
    char *txt = "abcdefgh";
    char *key = "12345678";
    
    char enc[8], res[8];
    
    dlc_des_setkey(&ctx, key);
    dlc_des_encrypt(&ctx, txt, enc);
    dlc_des_decrypt(&ctx, enc, res);
}

void des_ecb_test(){
    dl_str          *enc, *res, b64;
    des_ctx         ctx;
    
    char *txt = "Hi! How are you";
    char *key = "12345678";
    dlc_des_setkey(&ctx, key);
    
    enc = dlc_des_ecb_encrypt(&ctx, txt, strlen(txt), NULL);
    
    b64.data = dl_alloc(base64_len(enc->len), NULL);
    dl_encode_base64(&b64, enc, 1);
    
    res = dlc_des_ecb_decrypt(&ctx, enc->data, enc->len, NULL);
    
    dl_str_free(enc);
    dl_str_free(res);
}

void triple_des_test(){
    char enc[8], res[8];
    char *txt = "abcdefgh";
    tripledes_ctx       ctx;
    char *k1 = "12345678";
    char *k2 = "abcdefgh";
    char *k3 = "11111111";
    
    dlc_3des_set3keys(&ctx, k1, k2, k3);
    dlc_3des_encrypt(&ctx, txt, enc);
    dlc_3des_decrypt(&ctx, enc, res);
}

void triple_des_ecb_test(){
    tripledes_ctx       ctx;
    dl_str              *enc, *res, b64;

    dl_str txt = dl_string("12345678aa");
    
    
    char *k1 = "12345678";
    char *k2 = "abcdefgh";
    char *k3 = "11111111";
    
    
    dlc_3des_set3keys(&ctx, k1, k2, k3);
    enc = dlc_3des_ecb_encrypt(&ctx, txt.data, txt.len, NULL);
    
    b64.data = dl_alloc(base64_len(enc->len), NULL);
    dl_encode_base64(&b64, enc, 1);

    res = dlc_3des_ecb_decrypt(&ctx, enc->data, enc->len, NULL);
}

void des_cbc_test(){
    dl_str          *enc, *res, b64;
    des_ctx         ctx;
    
    dl_str txt = dl_string("Hi! How are you");
    char *key = "12345678";
    dlc_des_setkey(&ctx, key);
    dlc_urandom(ctx.vi, 8);
    
    enc = dlc_des_cbc_encrypt(&ctx, txt.data, txt.len, NULL);
    
    b64.data = dl_alloc(base64_len(enc->len), NULL);
    dl_encode_base64(&b64, enc, 1);
    
    res = dlc_des_cbc_decrypt(&ctx, enc->data, enc->len, NULL);
    
    dl_str_free(enc);
    dl_str_free(res);
}

void des_cfb_test(){
    dl_str          *enc, *res, b64;
    des_ctx         ctx;
    
    dl_str txt = dl_string("Hi! How are you");
    char *key = "12345678";
    dlc_des_setkey(&ctx, key);
    dlc_urandom(ctx.vi, 8);
    
    enc = dlc_des_cfb_encrypt(&ctx, txt.data, txt.len, NULL);
    
    b64.data = dl_alloc(base64_len(enc->len), NULL);
    dl_encode_base64(&b64, enc, 1);
    
    res = dlc_des_cfb_decrypt(&ctx, enc->data, enc->len, NULL);
    
    dl_str_free(enc);
    dl_str_free(res);
}

void des_ofb_test(){
    dl_str          *enc, *res, b64;
    des_ctx         ctx;
    
    dl_str txt = dl_string("Hi! How are you");
    char *key = "12345678";
    dlc_des_setkey(&ctx, key);
    dlc_urandom(ctx.vi, 8);
    
    enc = dlc_des_ofb_encrypt(&ctx, txt.data, txt.len, NULL);
    
    b64.data = dl_alloc(base64_len(enc->len), NULL);
    dl_encode_base64(&b64, enc, 1);
    
    res = dlc_des_ofb_decrypt(&ctx, enc->data, enc->len, NULL);
    
    dl_str_free(enc);
    dl_str_free(res);
}

void des_crt_test(){
    dl_str          *enc, *res, b64;
    des_ctx         ctx;
    
    dl_str txt = dl_string("Hi! How are you");
    char *key = "12345678";
    dlc_des_setkey(&ctx, key);
    dlc_urandom(ctx.vi, 8);
    
    enc = dlc_des_crt_encrypt(&ctx, txt.data, txt.len, NULL);
    
    b64.data = dl_alloc(base64_len(enc->len), NULL);
    dl_encode_base64(&b64, enc, 1);
    
    res = dlc_des_crt_decrypt(&ctx, enc->data, enc->len, NULL);
    
    dl_str_free(enc);
    dl_str_free(res);
}

int main(){
    
    des_test();
    des_ecb_test();
    triple_des_test();
    triple_des_ecb_test();
    
    des_cbc_test();
    des_cfb_test();
    des_ofb_test();
    des_crt_test();
}


