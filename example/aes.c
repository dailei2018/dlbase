#include <stdio.h>

#include "dl_base.h"
#include "dl_aes.h"

dl_log      *logger;
dl_pool     *pool;

void cbc_test()
{
    dl_str      *enc, *dec;
    char        *key1, *key2;
    
    key1 = "dfg2342dfds323";
    key2 = "sdkfhs89723uisdyis8736843";
    
    char *src = "hello, this is a aes cbc test program.";
    enc = dl_aes_encrypt(src, strlen(src), key2, strlen(key2), DL_AES_CBC);
    if (enc == NULL)
        exit_log(-1, DL_LOG_DEBUG, logger, "cbc encrypt failed");
    
    
    dec = dl_aes_decrypt(enc->data, enc->len, key2, strlen(key2), DL_AES_CBC);
    if (dec == NULL)
        exit_log(-1, DL_LOG_DEBUG, logger, "cbc decrypt failed");
    
    dl_printf("aes-cbc: %V\n", dec);
    
}

int main()
{
    logger = dl_log_init(DL_LOG_DEBUG, NULL);
    if (logger == NULL) return -1;
    
    pool = dl_create_pool(1024, logger);
    if (pool == NULL) return -1;
    
    cbc_test();
    
}