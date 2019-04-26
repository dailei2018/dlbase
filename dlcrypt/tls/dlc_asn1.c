#include "dlc_base.h"

int dlc_asn1_get_len( uchar **p, const uchar *end, size_t *len)
{
    if( ( end - *p ) < 1 ) return DLC_ERR_ASN1_OUT_OF_DATA;

    
    if( ( **p & 0x80 ) == 0 ){
        /* 当前字节最高位为0(0xxx xxxx)，那么 *p 即长度 */
        *len = *(*p)++;
        
    }else
    {
        /* 多少字节来表示长度 */
        switch( **p & 0x7F )
        {
        case 1:
            if( ( end - *p ) < 2 ) return DLC_ERR_ASN1_OUT_OF_DATA;

            *len = (*p)[1];
            (*p) += 2;
            break;

        case 2:
            if( ( end - *p ) < 3 ) return DLC_ERR_ASN1_OUT_OF_DATA;

            *len = ( (size_t)(*p)[1] << 8 ) | (*p)[2];
            (*p) += 3;
            break;

        case 3:
            if( ( end - *p ) < 4 ) return DLC_ERR_ASN1_OUT_OF_DATA;

            *len = ( (size_t)(*p)[1] << 16 ) |
                   ( (size_t)(*p)[2] << 8  ) | (*p)[3];
            (*p) += 4;
            break;

        case 4:
            if( ( end - *p ) < 5 ) return DLC_ERR_ASN1_OUT_OF_DATA;

            *len = ( (size_t)(*p)[1] << 24 ) | ( (size_t)(*p)[2] << 16 ) |
                   ( (size_t)(*p)[3] << 8  ) |           (*p)[4];
            (*p) += 5;
            break;

        default:
            return DLC_ERR_ASN1_INVALID_LENGTH;
        }
    }

    if( *len > (size_t) ( end - *p ) ) return DLC_ERR_ASN1_OUT_OF_DATA;

    return 0;
}

int dlc_asn1_get_tag( uchar **p, const uchar *end, size_t *len, int tag )
{
    if( ( end - *p ) < 1 ) return DLC_ERR_ASN1_OUT_OF_DATA;

    if( **p != tag ) return DLC_ERR_ASN1_UNEXPECTED_TAG;

    (*p)++;

    return dlc_asn1_get_len( p, end, len );
}


int dlc_asn1_get_int( uchar **p, const uchar *end, int *val )
{
    int     ret;
    size_t  len;

    ret = dlc_asn1_get_tag( p, end, &len, DLC_ASN1_INTEGER );
    if(ret != 0) return ret;

    if( len == 0 || len > sizeof( int ) || ( **p & 0x80 ) != 0 )
        return DLC_ERR_ASN1_INVALID_LENGTH;

    *val = 0;

    while( len-- > 0 )
    {
        *val = ( *val << 8 ) | **p;
        (*p)++;
    }

    return 0;
}

/* 获取签名算法 */
int dlc_asn1_get_alg( uchar **p, const uchar *end, dlc_asn1_buf *alg, dlc_asn1_buf *params )
{
    int ret;
    size_t len;

    if( ( ret = dlc_asn1_get_tag( p, end, &len,
            DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
        return( ret );

    if( ( end - *p ) < 1 )
        return( DLC_ERR_ASN1_OUT_OF_DATA );

    alg->tag = **p;
    end = *p + len;

    if( ( ret = dlc_asn1_get_tag( p, end, &alg->len, DLC_ASN1_OID ) ) != 0 )
        return( ret );

    alg->p = *p;
    *p += alg->len;

    if( *p == end )
    {
        memset( params, 0, sizeof(dlc_asn1_buf) );
        return 0;
    }

    params->tag = **p;
    (*p)++;

    if( ( ret = dlc_asn1_get_len( p, end, &params->len ) ) != 0 )
        return( ret );

    params->p = *p;
    *p += params->len;

    if( *p != end )
        return DLC_ERR_ASN1_LENGTH_MISMATCH ;

    return( 0 );
}

/*
 * Get a bit string without unused bits
 */
int dlc_asn1_get_bitstring_null( uchar **p, const uchar *end, size_t *len )
{
    int ret;

    if( ( ret = dlc_asn1_get_tag( p, end, len, DLC_ASN1_BIT_STRING ) ) != 0 )
        return( ret );

    if( (*len)-- < 2 || *(*p)++ != 0 )
        return( DLC_ERR_ASN1_INVALID_DATA );

    return( 0 );
}

int dlc_asn1_get_bitstring( uchar **p, const uchar *end,
                        dlc_asn1_bitstring *bs)
{
    int ret;

    /* Certificate type is a single byte bitstring */
    if( ( ret = dlc_asn1_get_tag( p, end, &bs->len, DLC_ASN1_BIT_STRING ) ) != 0 )
        return( ret );

    /* Check length, subtract one for actual bit string length */
    if( bs->len < 1 )
        return( DLC_ERR_ASN1_OUT_OF_DATA );
    bs->len -= 1;

    /* Get number of unused bits, ensure unused bits <= 7 */
    bs->unused_bits = **p;
    if( bs->unused_bits > 7 )
        return( DLC_ERR_ASN1_INVALID_LENGTH );
    (*p)++;

    /* Get actual bitstring */
    bs->p = *p;
    *p += bs->len;

    if( *p != end )
        return( DLC_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}

int dlc_asn1_get_bool( uchar **p, const uchar *end, int *val )
{
    int ret;
    size_t len;

    if( ( ret = dlc_asn1_get_tag( p, end, &len, DLC_ASN1_BOOLEAN ) ) != 0 )
        return( ret );

    if( len != 1 )
        return( DLC_ERR_ASN1_INVALID_LENGTH );

    *val = ( **p != 0 ) ? 1 : 0;
    (*p)++;

    return( 0 );
}

/*
 *  Parses and splits an ASN.1 "SEQUENCE OF <tag>"
 */
int dlc_asn1_get_sequence_of( uchar **p, const uchar *end, dlc_asn1_sequence *cur, int tag)
{
    int ret;
    size_t len;
    dlc_asn1_buf *buf;

    /* Get main sequence tag */
    if( ( ret = dlc_asn1_get_tag( p, end, &len,
            DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
        return( ret );

    if( *p + len != end )
        return( DLC_ERR_ASN1_LENGTH_MISMATCH );

    while( *p < end )
    {
        buf = &(cur->buf);
        buf->tag = **p;

        if( ( ret = dlc_asn1_get_tag( p, end, &buf->len, tag ) ) != 0 )
            return( ret );

        buf->p = *p;
        *p += buf->len;

        /* Allocate and assign next pointer */
        if( *p < end )
        {
            cur->next = (dlc_asn1_sequence*)dl_calloc( sizeof( dlc_asn1_sequence ), NULL);

            if( cur->next == NULL )
                return( DLC_ERR_ASN1_ALLOC_FAILED );

            cur = cur->next;
        }
    }

    /* Set final sequence entry's next pointer to NULL */
    cur->next = NULL;

    if( *p != end )
        return( DLC_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}
