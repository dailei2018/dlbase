#include "dlc_base.h"

int main(){
    int     len, rc;
    char    *txt = "abcd";
    mpz_t       enc;
    
    rsa_public_key      pub;
    rsa_private_key     key;
    
    rsa_public_key_init(&pub);
    rsa_private_key_init(&key);
    
    mpz_set_ui(pub.e, 65537);
    rsa_generate_keypair(&pub, &key, 512, 0);
    
    mpz_init(enc);
    
    rsa_sha1_sign(&key, txt, strlen(txt), enc);
    rc = rsa_sha1_verify(&pub, txt, strlen(txt), enc);
    if(rc == 0){
        printf("rsa sha1 verify successfully\n");
    }
    
    
    rsa_sha256_sign(&key, txt, strlen(txt), enc);
    rc = rsa_sha256_verify(&pub, txt, strlen(txt), enc);
    if(rc == 0){
        printf("rsa sha256 verify successfully\n");
    }
    
    rsa_md5_sign(&key, txt, strlen(txt), enc);
    rc = rsa_md5_verify(&pub, txt, strlen(txt), enc);
    if(rc == 0){
        printf("rsa md5 verify successfully\n");
    }
    
    
    return 0;
}