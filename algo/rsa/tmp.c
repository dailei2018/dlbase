int PEM_write_bio_RSAPrivateKey(BIO *bp, type *x, const EVP_CIPHER *enc,
             unsigned char *kstr, int klen, pem_password_cb *cb, void *u)
{
    return PEM_ASN1_write_bio((i2d_of_void *)i2d_##asn1,str,bp,(void *)x,enc,kstr,klen,cb,u);
}


int PEM_write_RSAPrivateKey(FILE *fp, type *x, const EVP_CIPHER *enc,
             unsigned char *kstr, int klen, pem_password_cb *cb,
                  void *u)
        {
        return PEM_ASN1_write((i2d_of_void *)i2d_##asn1,str,fp,x,enc,kstr,klen,cb,u);
        }