#include "dlc_base.h"
#include "dlc_oid.h"

#define CHECK(code) if( ( ret = code ) != 0 ){ return( ret ); }
#define CHECK_RANGE(min, max, val) if( val < min || val > max ){ return( ret ); }

/*  a003          020102
 *  Version  ::=  INTEGER  {  v1(0), v2(1), v3(2)  }
 */
int dlc_x509_get_version( uchar **p, const uchar *end, int *ver )
{
    int     ret;
    size_t  len;

    ret = dlc_asn1_get_tag( p, end, &len, DLC_ASN1_CONTEXT_SPECIFIC | DLC_ASN1_CONSTRUCTED | 0 );
    if(ret != 0){
        if(ret == DLC_ERR_ASN1_UNEXPECTED_TAG){
            *ver = 0;
            return 0;
        }
        
        return ret;
    }

    end = *p + len;

    ret = dlc_asn1_get_int(p, end, ver);
    if(ret != 0 ) return DLC_ERR_X509_INVALID_VERSION + ret;

    if( *p != end )
        return DLC_ERR_X509_INVALID_VERSION + DLC_ERR_ASN1_LENGTH_MISMATCH;

    return( 0 );
}

/*  020100
 *  CertificateSerialNumber  ::=  INTEGER
 */
int dlc_x509_get_serial(uchar **p, const uchar *end, dlc_asn1_buf *serial)
{
    int ret;

    if( ( end - *p ) < 1 )
        return( DLC_ERR_X509_INVALID_SERIAL +
                DLC_ERR_ASN1_OUT_OF_DATA );

    if( **p != ( DLC_ASN1_CONTEXT_SPECIFIC | DLC_ASN1_PRIMITIVE | 2 ) &&
        **p !=   DLC_ASN1_INTEGER )
        return( DLC_ERR_X509_INVALID_SERIAL +
                DLC_ERR_ASN1_UNEXPECTED_TAG );

    serial->tag = *(*p)++;

    if( ( ret = dlc_asn1_get_len( p, end, &serial->len ) ) != 0 )
        return( DLC_ERR_X509_INVALID_SERIAL + ret );

    serial->p = *p;
    *p += serial->len;

    return( 0 );
}

int dlc_x509_get_alg( uchar **p, const uchar *end, dlc_asn1_buf *alg, dlc_asn1_buf *params)
{
    int ret;

    if( ( ret = dlc_asn1_get_alg( p, end, alg, params ) ) != 0 )
        return( DLC_ERR_X509_INVALID_ALG + ret );

    return( 0 );
}

/*
 * 从oid 和 额外参数中获取签名算法
 * Get signature algorithm from alg OID and optional parameters
 */
int dlc_x509_get_sig_alg( const dlc_asn1_buf *sig_oid, const dlc_asn1_buf *sig_params,
                      dlc_md_type *md_alg, dlc_pk_type *pk_alg,
                      void **sig_opts )
{
    int ret;

    if( *sig_opts != NULL )
        return( DLC_ERR_X509_BAD_INPUT_DATA );

    if( ( ret = dlc_oid_get_sig_alg( sig_oid, md_alg, pk_alg ) ) != 0 )
        return( DLC_ERR_X509_UNKNOWN_SIG_ALG + ret );

    {
        /* Make sure parameters are absent or NULL */
        if( ( sig_params->tag != DLC_ASN1_NULL && sig_params->tag != 0 ) ||
              sig_params->len != 0 )
        return( DLC_ERR_X509_INVALID_ALG );
    }

    return( 0 );
}


/*
 *  AttributeTypeAndValue ::= SEQUENCE {
 *    type     AttributeType,
 *    value    AttributeValue }
 *
 *  AttributeType ::= OBJECT IDENTIFIER
 *
 *  AttributeValue ::= ANY DEFINED BY AttributeType
 */
static int dlc_x509_get_attr_type_value( unsigned char **p,
                                     const unsigned char *end,
                                     dlc_asn1_named_data *cur )
{
    int ret;
    size_t len;
    dlc_asn1_buf *oid;
    dlc_asn1_buf *val;

    if( ( ret = dlc_asn1_get_tag( p, end, &len,
            DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
        return( DLC_ERR_X509_INVALID_NAME + ret );

    if( ( end - *p ) < 1 )
        return( DLC_ERR_X509_INVALID_NAME +
                DLC_ERR_ASN1_OUT_OF_DATA );

    oid = &cur->oid;
    oid->tag = **p;

    if( ( ret = dlc_asn1_get_tag( p, end, &oid->len, DLC_ASN1_OID ) ) != 0 )
        return( DLC_ERR_X509_INVALID_NAME + ret );

    oid->p = *p;
    *p += oid->len;

    if( ( end - *p ) < 1 )
        return( DLC_ERR_X509_INVALID_NAME +
                DLC_ERR_ASN1_OUT_OF_DATA );

    if( **p != DLC_ASN1_BMP_STRING && **p != DLC_ASN1_UTF8_STRING      &&
        **p != DLC_ASN1_T61_STRING && **p != DLC_ASN1_PRINTABLE_STRING &&
        **p != DLC_ASN1_IA5_STRING && **p != DLC_ASN1_UNIVERSAL_STRING &&
        **p != DLC_ASN1_BIT_STRING )
        return( DLC_ERR_X509_INVALID_NAME +
                DLC_ERR_ASN1_UNEXPECTED_TAG );

    val = &cur->val;
    val->tag = *(*p)++;

    if( ( ret = dlc_asn1_get_len( p, end, &val->len ) ) != 0 )
        return( DLC_ERR_X509_INVALID_NAME + ret );

    val->p = *p;
    *p += val->len;

    cur->next = NULL;

    return( 0 );
}



/*
 *  Name ::= CHOICE { -- only one possibility for now --
 *       rdnSequence  RDNSequence }
 *
 *  RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
 *
 *  RelativeDistinguishedName ::=
 *    SET OF AttributeTypeAndValue
 *
 *  AttributeTypeAndValue ::= SEQUENCE {
 *    type     AttributeType,
 *    value    AttributeValue }
 *
 *  AttributeType ::= OBJECT IDENTIFIER
 *
 *  AttributeValue ::= ANY DEFINED BY AttributeType
 *
 * The data structure is optimized for the common case where each RDN has only
 * one element, which is represented as a list of AttributeTypeAndValue.
 * For the general case we still use a flat list, but we mark elements of the
 * same set so that they are "merged" together in the functions that consume
 * this list, eg dlc_x509_dn_gets().
 */
int dlc_x509_get_name( unsigned char **p, const unsigned char *end,
                   dlc_asn1_named_data *cur )
{
    int ret;
    size_t set_len;
    const unsigned char *end_set;

    /* don't use recursion, we'd risk stack overflow if not optimized */
    while( 1 )
    {
        /*
         * parse SET
         */
        if( ( ret = dlc_asn1_get_tag( p, end, &set_len,
                DLC_ASN1_CONSTRUCTED | DLC_ASN1_SET ) ) != 0 )
            return( DLC_ERR_X509_INVALID_NAME + ret );

        end_set  = *p + set_len;

        while( 1 )
        {
            if( ( ret = dlc_x509_get_attr_type_value( p, end_set, cur ) ) != 0 )
                return( ret );

            if( *p == end_set )
                break;

            /* Mark this item as being no the only one in a set */
            cur->next_merged = 1;

            cur->next = dl_calloc(sizeof( dlc_asn1_named_data ), NULL);

            if( cur->next == NULL )
                return( DLC_ERR_X509_ALLOC_FAILED );

            cur = cur->next;
        }

        /*
         * continue until end of SEQUENCE is reached
         */
        if( *p == end )
            return( 0 );

        cur->next = dl_calloc(sizeof( dlc_asn1_named_data ), NULL);

        if( cur->next == NULL )
            return( DLC_ERR_X509_ALLOC_FAILED );

        cur = cur->next;
    }
}

static int dlc_x509_parse_int( unsigned char **p, size_t n, int *res )
{
    *res = 0;

    for( ; n > 0; --n )
    {
        if( ( **p < '0') || ( **p > '9' ) )
            return ( DLC_ERR_X509_INVALID_DATE );

        *res *= 10;
        *res += ( *(*p)++ - '0' );
    }

    return( 0 );
}

static int dlc_x509_date_is_valid(const dlc_x509_time *t )
{
    int ret = DLC_ERR_X509_INVALID_DATE;
    int month_len;

    CHECK_RANGE( 0, 9999, t->year );
    CHECK_RANGE( 0, 23,   t->hour );
    CHECK_RANGE( 0, 59,   t->min  );
    CHECK_RANGE( 0, 59,   t->sec  );

    switch( t->mon )
    {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            month_len = 31;
            break;
        case 4: case 6: case 9: case 11:
            month_len = 30;
            break;
        case 2:
            if( ( !( t->year % 4 ) && t->year % 100 ) ||
                !( t->year % 400 ) )
                month_len = 29;
            else
                month_len = 28;
            break;
        default:
            return( ret );
    }
    CHECK_RANGE( 1, month_len, t->day );

    return( 0 );
}

/*
 * Parse an ASN1_UTC_TIME (yearlen=2) or ASN1_GENERALIZED_TIME (yearlen=4)
 * field.
 */
static int dlc_x509_parse_time( unsigned char **p, size_t len, size_t yearlen,
                            dlc_x509_time *tm )
{
    int ret;

    /*
     * Minimum length is 10 or 12 depending on yearlen
     */
    if ( len < yearlen + 8 )
        return ( DLC_ERR_X509_INVALID_DATE );
    len -= yearlen + 8;

    /*
     * Parse year, month, day, hour, minute
     */
    CHECK( dlc_x509_parse_int( p, yearlen, &tm->year ) );
    if ( 2 == yearlen )
    {
        if ( tm->year < 50 )
            tm->year += 100;

        tm->year += 1900;
    }

    CHECK( dlc_x509_parse_int( p, 2, &tm->mon ) );
    CHECK( dlc_x509_parse_int( p, 2, &tm->day ) );
    CHECK( dlc_x509_parse_int( p, 2, &tm->hour ) );
    CHECK( dlc_x509_parse_int( p, 2, &tm->min ) );

    /*
     * Parse seconds if present
     */
    if ( len >= 2 )
    {
        CHECK( dlc_x509_parse_int( p, 2, &tm->sec ) );
        len -= 2;
    }
    else
        return ( DLC_ERR_X509_INVALID_DATE );

    /*
     * Parse trailing 'Z' if present
     */
    if ( 1 == len && 'Z' == **p )
    {
        (*p)++;
        len--;
    }

    /*
     * We should have parsed all characters at this point
     */
    if ( 0 != len )
        return ( DLC_ERR_X509_INVALID_DATE );

    CHECK( dlc_x509_date_is_valid( tm ) );

    return ( 0 );
}

/*
 *  Time ::= CHOICE {
 *       utcTime        UTCTime,
 *       generalTime    GeneralizedTime }
 */
int dlc_x509_get_time( unsigned char **p, const unsigned char *end,
                           dlc_x509_time *tm )
{
    int ret;
    size_t len, year_len;
    unsigned char tag;

    if( ( end - *p ) < 1 )
        return( DLC_ERR_X509_INVALID_DATE +
                DLC_ERR_ASN1_OUT_OF_DATA );

    tag = **p;

    if( tag == DLC_ASN1_UTC_TIME )
        year_len = 2;
    else if( tag == DLC_ASN1_GENERALIZED_TIME )
        year_len = 4;
    else
        return( DLC_ERR_X509_INVALID_DATE +
                DLC_ERR_ASN1_UNEXPECTED_TAG );

    (*p)++;
    ret = dlc_asn1_get_len( p, end, &len );

    if( ret != 0 )
        return( DLC_ERR_X509_INVALID_DATE + ret );

    return dlc_x509_parse_time( p, len, year_len, tm );
}


/*
 *  SubjectPublicKeyInfo  ::=  SEQUENCE  {
 *       algorithm            AlgorithmIdentifier,
 *       subjectPublicKey     BIT STRING }
 */
int dlc_pk_parse_subpubkey( unsigned char **p, const unsigned char *end,
                        dlc_pk_context *pk )
{
    int             ret;
    size_t          len;
    dlc_asn1_buf    alg_params;
    dlc_pk_type     pk_alg = DLC_PK_NONE;
    dlc_pk_info     *pk_info;


    if( ( ret = dlc_asn1_get_tag( p, end, &len,
                    DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
    {
        return( DLC_ERR_PK_KEY_INVALID_FORMAT + ret );
    }

    end = *p + len;

    if( ( ret = dlc_pk_get_pk_alg( p, end, &pk_alg, &alg_params ) ) != 0 )
        return( ret );

    if( ( ret = dlc_asn1_get_bitstring_null( p, end, &len ) ) != 0 )
        return( DLC_ERR_PK_INVALID_PUBKEY + ret );

    if( *p + len != end )
        return( DLC_ERR_PK_INVALID_PUBKEY +
                DLC_ERR_ASN1_LENGTH_MISMATCH );

    if( ( pk_info = dlc_pk_info_from_type( pk_alg ) ) == NULL )
        return( DLC_ERR_PK_UNKNOWN_PK_ALG );

    if( ( ret = dlc_pk_setup( pk, pk_info ) ) != 0 )
        return( ret );

    if( pk_alg == DLC_PK_RSA )
    {
        ret = dlc_pk_get_rsapubkey( p, end, (dlc_rsa_context *)pk->pk_ctx);
    }else
        ret = DLC_ERR_PK_UNKNOWN_PK_ALG;

    if( ret == 0 && *p != end )
        ret = DLC_ERR_PK_INVALID_PUBKEY
              DLC_ERR_ASN1_LENGTH_MISMATCH;

    if( ret != 0 )
        dl_free(pk->pk_ctx);

    return( ret );
}


/*
 * X.509 Extensions (No parsing of extensions, pointer should
 * be either manually updated or extensions should be parsed!)
 */
int dlc_x509_get_ext( uchar **p, const uchar *end, dlc_asn1_buf *ext, int tag )
{
    int ret;
    size_t len;

    if( *p == end )
        return( 0 );

    ext->tag = **p;

    if( ( ret = dlc_asn1_get_tag( p, end, &ext->len,
            DLC_ASN1_CONTEXT_SPECIFIC | DLC_ASN1_CONSTRUCTED | tag ) ) != 0 )
        return( ret );

    ext->p = *p;
    end = *p + ext->len;

    /*
     * Extensions  ::=  SEQUENCE SIZE (1..MAX) OF Extension
     *
     * Extension  ::=  SEQUENCE  {
     *      extnID      OBJECT IDENTIFIER,
     *      critical    BOOLEAN DEFAULT FALSE,
     *      extnValue   OCTET STRING  }
     */
    if( ( ret = dlc_asn1_get_tag( p, end, &len,
            DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
        return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

    if( end != *p + len )
        return( DLC_ERR_X509_INVALID_EXTENSIONS +
                DLC_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}


int dlc_x509_get_sig( unsigned char **p, const unsigned char *end, dlc_asn1_buf *sig )
{
    int ret;
    size_t len;
    int tag_type;

    if( ( end - *p ) < 1 )
        return( DLC_ERR_X509_INVALID_SIGNATURE + DLC_ERR_ASN1_OUT_OF_DATA );

    tag_type = **p;

    if( ( ret = dlc_asn1_get_bitstring_null( p, end, &len ) ) != 0 )
        return( DLC_ERR_X509_INVALID_SIGNATURE + ret );

    sig->tag = tag_type;
    sig->len = len;
    sig->p = *p;

    *p += len;

    return( 0 );
}

