#include "dlc_base.h"

int main(){
    int     len;
    char    *txt = "abcd";
    char    buf[1024];
    mpz_t       enc;
    
    rsa_public_key      pub;
    rsa_private_key     key;
    
    rsa_public_key_init(&pub);
    rsa_private_key_init(&key);
    
    mpz_set_ui(pub.e, 3);
    rsa_generate_keypair(&pub, &key, 512, 0);
    
    mpz_init(enc);
    rsa_encrypt(&pub, 4, txt, enc);
    
    rsa_decrypt(&key, &len, buf, enc);
    
    printf("%d: %s\n", len, buf);
    
    
    return 0;
}