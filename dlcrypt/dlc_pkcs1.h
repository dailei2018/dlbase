#ifndef __DLC_PKCS1__
#define __DLC_PKCS1__

typedef struct{
    int bs;
    int len;
    char *data;
} dlc_der_rep;

dl_str * dlc_rsa_pem_to_der(char *fname);
dl_str * dlc_rsa_der_to_pem(dl_str *der, int is_pub);
int
rsa_keypair_from_der(rsa_public_key *pub, rsa_private_key *priv, int limit, 
		     int length, uchar *data);

int dlc_rsa_keypair_from_der(dl_str *der, rsa_public_key *pub, rsa_private_key *key);

dl_str * dl_rsa_pub_to_der(dl_str *der, rsa_public_key *pub);
dl_str * dl_rsa_keypair_to_der(dl_str *der, rsa_public_key *pub, rsa_private_key *key);

#endif