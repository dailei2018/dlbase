#include "dlc_base.h"
#include "dlc_pk.h"
#include "dlc_oid.h"


dlc_pk_info dlc_rsa_info = {
    DLC_PK_RSA,
    "RSA"
};

dlc_pk_info dlc_eckey_info = {
    DLC_PK_ECKEY,
    "EC"
};

dlc_pk_info dlc_eckeydh_info = {
    DLC_PK_ECKEY_DH,
    "EC_DH"
};

dlc_pk_info dlc_ecdsa_info = {
    DLC_PK_ECDSA,
    "ECDSA"
};

dlc_pk_info * dlc_pk_info_from_type(dlc_pk_type pk_type)
{
    switch( pk_type ) {

        case DLC_PK_RSA:
            return( &dlc_rsa_info );

        case DLC_PK_ECKEY:
            return( &dlc_eckey_info );
        case DLC_PK_ECKEY_DH:
            return( &dlc_eckeydh_info );

        case DLC_PK_ECDSA:
            return( &dlc_ecdsa_info );
        /* DLC_PK_RSA_ALT omitted on purpose */
        default:
            return( NULL );
    }
}

/*
 * Initialise context
 */
int dlc_pk_setup( dlc_pk_context *ctx, dlc_pk_info *info )
{
    switch(info->type){
        case DLC_PK_RSA:
            ctx->pk_ctx = dl_calloc(sizeof(dlc_rsa_context), NULL);
            if(ctx->pk_ctx == NULL) return DLC_ERR_PK_ALLOC_FAILED;
            rsa_public_key_init(&((dlc_rsa_context *)(ctx->pk_ctx))->pub);
            break;
        
        default:
            return -1;
    }
    
    ctx->pk_info = info;

    return 0;
}

int dlc_pk_get_pk_alg( uchar **p, const uchar *end, dlc_pk_type *pk_alg, dlc_asn1_buf *params )
{
    int ret;
    dlc_asn1_buf alg_oid;

    memset( params, 0, sizeof(dlc_asn1_buf) );

    if( ( ret = dlc_asn1_get_alg( p, end, &alg_oid, params ) ) != 0 )
        return( DLC_ERR_PK_INVALID_ALG + ret );

    if( dlc_oid_get_pk_alg( &alg_oid, pk_alg ) != 0 )
        return( DLC_ERR_PK_UNKNOWN_PK_ALG );

    /*
     * No parameters with RSA (only for EC)
     */
    if( *pk_alg == DLC_PK_RSA &&
            ( ( params->tag != DLC_ASN1_NULL && params->tag != 0 ) ||
                params->len != 0 ) )
    {
        return( DLC_ERR_PK_INVALID_ALG );
    }

    return( 0 );
}

/*
 *  RSAPublicKey ::= SEQUENCE {
 *      modulus           INTEGER,  -- n
 *      publicExponent    INTEGER   -- e
 *  }
 */
int dlc_pk_get_rsapubkey( uchar **p, const uchar *end, dlc_rsa_context *rsa )
{
    int ret;
    size_t len;

    if( ( ret = dlc_asn1_get_tag( p, end, &len,
            DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
        return( DLC_ERR_PK_INVALID_PUBKEY + ret );

    if( *p + len != end )
        return( DLC_ERR_PK_INVALID_PUBKEY +
                DLC_ERR_ASN1_LENGTH_MISMATCH );

    /* Import N */
    if( ( ret = dlc_asn1_get_tag( p, end, &len, DLC_ASN1_INTEGER ) ) != 0 )
        return( DLC_ERR_PK_INVALID_PUBKEY + ret );

    nettle_mpz_set_str_256_u(rsa->pub.n, len, *p);

    *p += len;

    /* Import E */
    if( ( ret = dlc_asn1_get_tag( p, end, &len, DLC_ASN1_INTEGER ) ) != 0 )
        return( DLC_ERR_PK_INVALID_PUBKEY + ret );

    nettle_mpz_set_str_256_u(rsa->pub.e, len, *p);

    *p += len;

    if( *p != end )
        return( DLC_ERR_PK_INVALID_PUBKEY + DLC_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}