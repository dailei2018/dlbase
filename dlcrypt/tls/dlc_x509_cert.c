#include "dlc_base.h"
#include "dlc_oid.h"

int dlc_pem_mark_line(char *data, int *skip)
{
    char *cur, *e;
    
    cur = data;
    *skip = 1;
    
    while(*cur != '\n') cur++;
    
    if(*(cur - 1) == '\r'){
        cur--;
        *skip = 2;
    }
    
    return cur - data;
}

char *dlc_pem_to_der(char *pem, size_t len, int *slen)
{
    char    *data, *cur, *dcur, *end;
    int     n, skip;
    
    data = dl_alloc(len, NULL);
    if(data == NULL) return NULL;
    dcur = data;
    
    cur = pem;
    end = pem + len;
    
    while(cur < end){
        
        n = dlc_pem_mark_line(cur, &skip);
        if(n > 0 && *cur != '-'){
            dcur = dl_cpymem(dcur, cur, n);
        }
        
        cur += n + skip;
        
    }
    
    
    dl_str  s1, s2;
    int     res_len;
    
    s1.data = data;
    s1.len = dcur - data;
    
    len = s1.len;
    
    if(len % 64 == 0){
        res_len = len * 3 / 4;
    }else{
        res_len = len / 64 * 48;
        res_len += (len % 64) * 3 / 4;
    }
    
    s2.data = dl_alloc(res_len, NULL);
    
    dl_decode_base64(&s2, &s1);
    *slen = s2.len;
    
    dl_free(data);
    
    return s2.data;
}

/*
 *  Validity ::= SEQUENCE {
 *       notBefore      Time,
 *       notAfter       Time }
 */
static int dlc_x509_get_dates( unsigned char **p,
                           const unsigned char *end,
                           dlc_x509_time *from,
                           dlc_x509_time *to )
{
    int ret;
    size_t len;

    if( ( ret = dlc_asn1_get_tag( p, end, &len,
            DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
        return( DLC_ERR_X509_INVALID_DATE + ret );

    end = *p + len;

    if( ( ret = dlc_x509_get_time( p, end, from ) ) != 0 )
        return( ret );

    if( ( ret = dlc_x509_get_time( p, end, to ) ) != 0 )
        return( ret );

    if( *p != end )
        return( DLC_ERR_X509_INVALID_DATE +
                DLC_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}

/*
 * X.509 v2/v3 unique identifier (not parsed)
 */
static int dlc_x509_get_uid( unsigned char **p,
                         const unsigned char *end,
                         dlc_asn1_buf *uid, int n )
{
    int ret;

    if( *p == end )
        return( 0 );

    uid->tag = **p;

    if( ( ret = dlc_asn1_get_tag( p, end, &uid->len,
            DLC_ASN1_CONTEXT_SPECIFIC | DLC_ASN1_CONSTRUCTED | n ) ) != 0 )
    {
        if( ret == DLC_ERR_ASN1_UNEXPECTED_TAG )
            return( 0 );

        return( ret );
    }

    uid->p = *p;
    *p += uid->len;

    return( 0 );
}


static int dlc_x509_get_basic_constraints( uchar **p,
                                       const uchar *end,
                                       int *ca_istrue,
                                       int *max_pathlen )
{
    int ret;
    size_t len;

    /*
     * BasicConstraints ::= SEQUENCE {
     *      cA                      BOOLEAN DEFAULT FALSE,
     *      pathLenConstraint       INTEGER (0..MAX) OPTIONAL }
     */
    *ca_istrue = 0; /* DEFAULT FALSE */
    *max_pathlen = 0; /* endless */

    if( ( ret = dlc_asn1_get_tag( p, end, &len,
            DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
        return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

    if( *p == end )
        return( 0 );

    if( ( ret = dlc_asn1_get_bool( p, end, ca_istrue ) ) != 0 )
    {
        if( ret == DLC_ERR_ASN1_UNEXPECTED_TAG )
            ret = dlc_asn1_get_int( p, end, ca_istrue );

        if( ret != 0 )
            return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

        if( *ca_istrue != 0 )
            *ca_istrue = 1;
    }

    if( *p == end )
        return( 0 );

    if( ( ret = dlc_asn1_get_int( p, end, max_pathlen ) ) != 0 )
        return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

    if( *p != end )
        return( DLC_ERR_X509_INVALID_EXTENSIONS +
                DLC_ERR_ASN1_LENGTH_MISMATCH );

    (*max_pathlen)++;

    return( 0 );
}

static int dlc_x509_get_key_usage( uchar **p,
                               const uchar *end,
                               unsigned int *key_usage)
{
    int ret;
    size_t i;
    dlc_asn1_bitstring bs = { 0, 0, NULL };

    if( ( ret = dlc_asn1_get_bitstring( p, end, &bs ) ) != 0 )
        return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

    if( bs.len < 1 )
        return( DLC_ERR_X509_INVALID_EXTENSIONS +
                DLC_ERR_ASN1_INVALID_LENGTH );

    /* Get actual bitstring */
    *key_usage = 0;
    for( i = 0; i < bs.len && i < sizeof( unsigned int ); i++ )
    {
        *key_usage |= (unsigned int) bs.p[i] << (8*i);
    }

    return( 0 );
}

/*
 * ExtKeyUsageSyntax ::= SEQUENCE SIZE (1..MAX) OF KeyPurposeId
 *
 * KeyPurposeId ::= OBJECT IDENTIFIER
 */
static int dlc_x509_get_ext_key_usage( uchar **p, const uchar *end,
                               dlc_asn1_sequence *ext_key_usage)
{
    int ret;

    if( ( ret = dlc_asn1_get_sequence_of( p, end, ext_key_usage, DLC_ASN1_OID ) ) != 0 )
        return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

    /* Sequence length must be >= 1 */
    if( ext_key_usage->buf.p == NULL )
        return( DLC_ERR_X509_INVALID_EXTENSIONS +
                DLC_ERR_ASN1_INVALID_LENGTH );

    return( 0 );
}

/*
 * SubjectAltName ::= GeneralNames
 *
 * GeneralNames ::= SEQUENCE SIZE (1..MAX) OF GeneralName
 *
 * GeneralName ::= CHOICE {
 *      otherName                       [0]     OtherName,
 *      rfc822Name                      [1]     IA5String,
 *      dNSName                         [2]     IA5String,
 *      x400Address                     [3]     ORAddress,
 *      directoryName                   [4]     Name,
 *      ediPartyName                    [5]     EDIPartyName,
 *      uniformResourceIdentifier       [6]     IA5String,
 *      iPAddress                       [7]     OCTET STRING,
 *      registeredID                    [8]     OBJECT IDENTIFIER }
 *
 * OtherName ::= SEQUENCE {
 *      type-id    OBJECT IDENTIFIER,
 *      value      [0] EXPLICIT ANY DEFINED BY type-id }
 *
 * EDIPartyName ::= SEQUENCE {
 *      nameAssigner            [0]     DirectoryString OPTIONAL,
 *      partyName               [1]     DirectoryString }
 *
 * NOTE: we only parse and use dNSName at this point.
 */
static int dlc_x509_get_subject_alt_name( unsigned char **p,
                                      const unsigned char *end,
                                      dlc_asn1_sequence *subject_alt_name )
{
    int ret;
    size_t len, tag_len;
    dlc_asn1_buf *buf;
    unsigned char tag;
    dlc_asn1_sequence *cur = subject_alt_name;

    /* Get main sequence tag */
    if( ( ret = dlc_asn1_get_tag( p, end, &len,
            DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
        return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

    if( *p + len != end )
        return( DLC_ERR_X509_INVALID_EXTENSIONS +
                DLC_ERR_ASN1_LENGTH_MISMATCH );

    while( *p < end )
    {
        if( ( end - *p ) < 1 )
            return( DLC_ERR_X509_INVALID_EXTENSIONS +
                    DLC_ERR_ASN1_OUT_OF_DATA );

        tag = **p;
        (*p)++;
        if( ( ret = dlc_asn1_get_len( p, end, &tag_len ) ) != 0 )
            return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

        if( ( tag & DLC_ASN1_TAG_CLASS_MASK ) !=
                DLC_ASN1_CONTEXT_SPECIFIC )
        {
            return( DLC_ERR_X509_INVALID_EXTENSIONS +
                    DLC_ERR_ASN1_UNEXPECTED_TAG );
        }

        /* Skip everything but DNS name */
        if( tag != ( DLC_ASN1_CONTEXT_SPECIFIC | 2 ) )
        {
            *p += tag_len;
            continue;
        }

        /* Allocate and assign next pointer */
        if( cur->buf.p != NULL )
        {
            if( cur->next != NULL )
                return( DLC_ERR_X509_INVALID_EXTENSIONS );

            cur->next = dl_calloc(sizeof( dlc_asn1_sequence ), NULL);

            if( cur->next == NULL )
                return( DLC_ERR_X509_INVALID_EXTENSIONS +
                        DLC_ERR_ASN1_ALLOC_FAILED );

            cur = cur->next;
        }

        buf = &(cur->buf);
        buf->tag = tag;
        buf->p = *p;
        buf->len = tag_len;
        *p += buf->len;
    }

    /* Set final sequence entry's next pointer to NULL */
    cur->next = NULL;

    if( *p != end )
        return( DLC_ERR_X509_INVALID_EXTENSIONS +
                DLC_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}

static int dlc_x509_get_ns_cert_type( unsigned char **p,
                                       const unsigned char *end,
                                       unsigned char *ns_cert_type)
{
    int ret;
    dlc_asn1_bitstring bs = { 0, 0, NULL };

    if( ( ret = dlc_asn1_get_bitstring( p, end, &bs ) ) != 0 )
        return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

    if( bs.len != 1 )
        return( DLC_ERR_X509_INVALID_EXTENSIONS +
                DLC_ERR_ASN1_INVALID_LENGTH );

    /* Get actual bitstring */
    *ns_cert_type = *bs.p;
    return( 0 );
}

/*
 * X.509 v3 extensions
 *
 */
static int dlc_x509_get_crt_ext( uchar **p, const uchar *end, dlc_x509_crt *crt )
{
    int ret;
    size_t len;
    unsigned char *end_ext_data, *end_ext_octet;

    if( ( ret = dlc_x509_get_ext( p, end, &crt->v3_ext, 3 ) ) != 0 )
    {
        if( ret == DLC_ERR_ASN1_UNEXPECTED_TAG )
            return( 0 );

        return( ret );
    }

    while( *p < end )
    {
        /*
         * Extension  ::=  SEQUENCE  {
         *      extnID      OBJECT IDENTIFIER,
         *      critical    BOOLEAN DEFAULT FALSE,
         *      extnValue   OCTET STRING  }
         */
        dlc_asn1_buf extn_oid = {0, 0, NULL};
        int is_critical = 0; /* DEFAULT FALSE */
        int ext_type = 0;

        if( ( ret = dlc_asn1_get_tag( p, end, &len,
                DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
            return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

        end_ext_data = *p + len;

        /* Get extension ID */
        if( ( ret = dlc_asn1_get_tag( p, end_ext_data, &extn_oid.len,
                                          DLC_ASN1_OID ) ) != 0 )
            return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

        extn_oid.tag = DLC_ASN1_OID;
        extn_oid.p = *p;
        *p += extn_oid.len;

        /* Get optional critical */
        if( ( ret = dlc_asn1_get_bool( p, end_ext_data, &is_critical ) ) != 0 &&
            ( ret != DLC_ERR_ASN1_UNEXPECTED_TAG ) )
            return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

        /* Data should be octet string type */
        if( ( ret = dlc_asn1_get_tag( p, end_ext_data, &len,
                DLC_ASN1_OCTET_STRING ) ) != 0 )
            return( DLC_ERR_X509_INVALID_EXTENSIONS + ret );

        end_ext_octet = *p + len;

        if( end_ext_octet != end_ext_data )
            return( DLC_ERR_X509_INVALID_EXTENSIONS +
                    DLC_ERR_ASN1_LENGTH_MISMATCH );

        /*
         * Detect supported extensions
         */
        ret = dlc_oid_get_x509_ext_type( &extn_oid, &ext_type );

        if( ret != 0 )
        {
            /* No parser found, skip extension */
            *p = end_ext_octet;


            if( is_critical )
            {
                /* Data is marked as critical: fail */
                return( DLC_ERR_X509_INVALID_EXTENSIONS +
                        DLC_ERR_ASN1_UNEXPECTED_TAG );
            }
            
            continue;
        }

        /* Forbid repeated extensions */
        if( ( crt->ext_types & ext_type ) != 0 )
            return( DLC_ERR_X509_INVALID_EXTENSIONS );

        crt->ext_types |= ext_type;

        switch( ext_type )
        {
        case DLC_X509_EXT_BASIC_CONSTRAINTS:
            /* Parse basic constraints */
            if( ( ret = dlc_x509_get_basic_constraints( p, end_ext_octet,
                    &crt->ca_istrue, &crt->max_pathlen ) ) != 0 )
                return( ret );
            break;

        case DLC_X509_EXT_KEY_USAGE:
            /* Parse key usage */
            if( ( ret = dlc_x509_get_key_usage( p, end_ext_octet,
                    &crt->key_usage ) ) != 0 )
                return( ret );
            break;

        case DLC_X509_EXT_EXTENDED_KEY_USAGE:
            /* Parse extended key usage */
            if( ( ret = dlc_x509_get_ext_key_usage( p, end_ext_octet,
                    &crt->ext_key_usage ) ) != 0 )
                return( ret );
            break;

        case DLC_X509_EXT_SUBJECT_ALT_NAME:
            /* Parse subject alt name */
            if( ( ret = dlc_x509_get_subject_alt_name( p, end_ext_octet,
                    &crt->subject_alt_names ) ) != 0 )
                return( ret );
            break;

        case DLC_X509_EXT_NS_CERT_TYPE:
            /* Parse netscape certificate type */
            if( ( ret = dlc_x509_get_ns_cert_type( p, end_ext_octet,
                    &crt->ns_cert_type ) ) != 0 )
                return( ret );
            break;

        default:
            return( DLC_ERR_X509_FEATURE_UNAVAILABLE );
        }
    }

    if( *p != end )
        return( DLC_ERR_X509_INVALID_EXTENSIONS +
                DLC_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}


/*
 * Parse and fill a single X.509 certificate in DER format
 */
int dlc_x509_crt_parse_der_core( dlc_x509_crt *crt, uchar *buf, size_t buflen )
{
    int         ret;
    size_t      len;
    uchar       *p, *end, *crt_end;
    dlc_asn1_buf      sig_params1, sig_params2, sig_oid2;

    memset( &sig_params1, 0, sizeof( dlc_asn1_buf ) );
    memset( &sig_params2, 0, sizeof( dlc_asn1_buf ) );
    memset( &sig_oid2, 0, sizeof( dlc_asn1_buf ) );

    /*
     * Check for valid input
     */
    if( crt == NULL || buf == NULL ) return -1;

    // Use the original buffer until we figure out actual length
    p = (uchar*) buf;
    end = p + buflen;

    /*
     * Certificate  ::=  SEQUENCE  {
     *      tbsCertificate       TBSCertificate,
     *      signatureAlgorithm   AlgorithmIdentifier,
     *      signatureValue       BIT STRING  }
     */
    ret = dlc_asn1_get_tag( &p, end, &len, DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE );
    if(ret == -1) return -1;
    
    if(len > ( end - p )) return -1;
    
    crt_end = p + len;
    
    //Create and populate a new buffer for the raw field
    crt->raw.len = crt_end - buf;
    crt->raw.p = p = dl_calloc(crt->raw.len, NULL);
    if(p == NULL) return -1;

    memcpy(p, buf, crt->raw.len);

    //pointers to certificate body
    p += crt->raw.len - len;
    end = crt_end = p + len;

    /*
     * TBSCertificate  ::=  SEQUENCE  {
     */
    crt->tbs.p = p;
    ret = dlc_asn1_get_tag( &p, end, &len, DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE );
    if(ret == -1) return -1;

    end = p + len;
    crt->tbs.len = end - crt->tbs.p;
    
    
    ret = dlc_x509_get_version(&p, end, &crt->version);
    if(ret != 0) return ret;
    
    if( crt->version < 0 || crt->version > 2 )
        return DLC_ERR_X509_UNKNOWN_VERSION;
    
    crt->version++;
    
    ret = dlc_x509_get_serial(&p, end, &crt->serial);
    if(ret != 0) return ret;
    
    ret = dlc_x509_get_alg(&p, end, &crt->sig_oid, &sig_params1);
    if(ret != 0) return ret;
    
    
    if( ( ret = dlc_x509_get_sig_alg( &crt->sig_oid, &sig_params1,
                                  &crt->sig_md, &crt->sig_pk,
                                  &crt->sig_opts ) ) != 0 ){
        return ret;
    }
    
    
     /*
     * issuer               Name
     */
    crt->issuer_raw.p = p;

    if( ( ret = dlc_asn1_get_tag( &p, end, &len,
            DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
    {
        return( DLC_ERR_X509_INVALID_FORMAT + ret );
    }

    if( ( ret = dlc_x509_get_name( &p, p + len, &crt->issuer ) ) != 0 )
    {
        return( ret );
    }

    crt->issuer_raw.len = p - crt->issuer_raw.p;



    /*
     * Validity ::= SEQUENCE {
     *      notBefore      Time,
     *      notAfter       Time }
     *
     */
    if( ( ret = dlc_x509_get_dates( &p, end, &crt->valid_from,
                                         &crt->valid_to ) ) != 0 )
    {
        return( ret );
    }

    /*
     * subject              Name
     */
    crt->subject_raw.p = p;

    if( ( ret = dlc_asn1_get_tag( &p, end, &len,
            DLC_ASN1_CONSTRUCTED | DLC_ASN1_SEQUENCE ) ) != 0 )
    {
        return( DLC_ERR_X509_INVALID_FORMAT + ret );
    }

    if( len && ( ret = dlc_x509_get_name( &p, p + len, &crt->subject ) ) != 0 )
    {
        return( ret );
    }

    crt->subject_raw.len = p - crt->subject_raw.p;


    /*
     * SubjectPublicKeyInfo
     */
    if( ( ret = dlc_pk_parse_subpubkey( &p, end, &crt->pk ) ) != 0 )
    {
        return( ret );
    }
    
    
    /*
     *  issuerUniqueID  [1]  IMPLICIT UniqueIdentifier OPTIONAL,
     *                       -- If present, version shall be v2 or v3
     *  subjectUniqueID [2]  IMPLICIT UniqueIdentifier OPTIONAL,
     *                       -- If present, version shall be v2 or v3
     *  extensions      [3]  EXPLICIT Extensions OPTIONAL
     *                       -- If present, version shall be v3
     */
    if( crt->version == 2 || crt->version == 3 )
    {
        ret = dlc_x509_get_uid( &p, end, &crt->issuer_id,  1 );
        if( ret != 0 )
            return( ret );
    }

    if( crt->version == 2 || crt->version == 3 )
    {
        ret = dlc_x509_get_uid( &p, end, &crt->subject_id,  2 );
        if( ret != 0 )
            return( ret );
    }
    
    if( crt->version == 3 )
    {
        ret = dlc_x509_get_crt_ext( &p, end, crt );
        if( ret != 0 )
        {
            return( ret );
        }
    }
    
    if( p != end )
        return( DLC_ERR_X509_INVALID_FORMAT + DLC_ERR_ASN1_LENGTH_MISMATCH );

    end = crt_end;
    
    /*
     *  }
     *  -- end of TBSCertificate
     *
     *  signatureAlgorithm   AlgorithmIdentifier,
     *  signatureValue       BIT STRING
     */
    if( ( ret = dlc_x509_get_alg( &p, end, &sig_oid2, &sig_params2 ) ) != 0 )
    {
        return( ret );
    }

    if( crt->sig_oid.len != sig_oid2.len ||
        memcmp( crt->sig_oid.p, sig_oid2.p, crt->sig_oid.len ) != 0 ||
        sig_params1.len != sig_params2.len ||
        ( sig_params1.len != 0 &&
          memcmp( sig_params1.p, sig_params2.p, sig_params1.len ) != 0 ) )
    {
        return( DLC_ERR_X509_SIG_MISMATCH );
    }

    if( ( ret = dlc_x509_get_sig( &p, end, &crt->sig ) ) != 0 )
    {
        return( ret );
    }

    if( p != end )
    {
        return( DLC_ERR_X509_INVALID_FORMAT + DLC_ERR_ASN1_LENGTH_MISMATCH );
    }
    
    return 0;
}

