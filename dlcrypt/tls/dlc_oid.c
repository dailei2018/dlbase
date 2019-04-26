#include "dlc_base.h"
#include "dlc_oid.h"

#define dlc_snprintf snprintf
#define ADD_LEN(s)      s, DLC_OID_SIZE(s)

/*
 * Macro to generate an internal function for oid_XXX_from_asn1() (used by
 * the other functions)
 */
#define FN_OID_TYPED_FROM_ASN1( TYPE_T, NAME, LIST )                        \
static const TYPE_T * oid_ ## NAME ## _from_asn1( const dlc_asn1_buf *oid )     \
{                                                                           \
    const TYPE_T *p = LIST;                                                 \
    const dlc_oid_descriptor_t *cur = (const dlc_oid_descriptor_t *) p;             \
    if( p == NULL || oid == NULL ) return( NULL );                          \
    while( cur->asn1 != NULL ) {                                            \
        if( cur->asn1_len == oid->len &&                                    \
            memcmp( cur->asn1, oid->p, oid->len ) == 0 ) {                  \
            return( p );                                                    \
        }                                                                   \
        p++;                                                                \
        cur = (const dlc_oid_descriptor_t *) p;                                 \
    }                                                                       \
    return( NULL );                                                         \
}

/*
 * Macro to generate a function for retrieving a single attribute from the
 * descriptor of an dlc_oid_descriptor_t wrapper.
 */
#define FN_OID_GET_DESCRIPTOR_ATTR1(FN_NAME, TYPE_T, TYPE_NAME, ATTR1_TYPE, ATTR1) \
int FN_NAME( const dlc_asn1_buf *oid, ATTR1_TYPE * ATTR1 )                  \
{                                                                       \
    const TYPE_T *data = oid_ ## TYPE_NAME ## _from_asn1( oid );        \
    if( data == NULL ) return( DLC_ERR_OID_NOT_FOUND );            \
    *ATTR1 = data->descriptor.ATTR1;                                    \
    return( 0 );                                                        \
}

/*
 * Macro to generate a function for retrieving a single attribute from an
 * dlc_oid_descriptor_t wrapper.
 */
#define FN_OID_GET_ATTR1(FN_NAME, TYPE_T, TYPE_NAME, ATTR1_TYPE, ATTR1) \
int FN_NAME( const dlc_asn1_buf *oid, ATTR1_TYPE * ATTR1 )                  \
{                                                                       \
    const TYPE_T *data = oid_ ## TYPE_NAME ## _from_asn1( oid );        \
    if( data == NULL ) return( DLC_ERR_OID_NOT_FOUND );            \
    *ATTR1 = data->ATTR1;                                               \
    return( 0 );                                                        \
}

/*
 * Macro to generate a function for retrieving two attributes from an
 * dlc_oid_descriptor_t wrapper.
 */
#define FN_OID_GET_ATTR2(FN_NAME, TYPE_T, TYPE_NAME, ATTR1_TYPE, ATTR1,     \
                         ATTR2_TYPE, ATTR2)                                 \
int FN_NAME( const dlc_asn1_buf *oid, ATTR1_TYPE * ATTR1, ATTR2_TYPE * ATTR2 )  \
{                                                                           \
    const TYPE_T *data = oid_ ## TYPE_NAME ## _from_asn1( oid );            \
    if( data == NULL ) return( DLC_ERR_OID_NOT_FOUND );                \
    *ATTR1 = data->ATTR1;                                                   \
    *ATTR2 = data->ATTR2;                                                   \
    return( 0 );                                                            \
}

/*
 * Macro to generate a function for retrieving the OID based on a single
 * attribute from a dlc_oid_descriptor_t wrapper.
 */
#define FN_OID_GET_OID_BY_ATTR1(FN_NAME, TYPE_T, LIST, ATTR1_TYPE, ATTR1)   \
int FN_NAME( ATTR1_TYPE ATTR1, const char **oid, size_t *olen )             \
{                                                                           \
    const TYPE_T *cur = LIST;                                               \
    while( cur->descriptor.asn1 != NULL ) {                                 \
        if( cur->ATTR1 == ATTR1 ) {                                         \
            *oid = cur->descriptor.asn1;                                    \
            *olen = cur->descriptor.asn1_len;                               \
            return( 0 );                                                    \
        }                                                                   \
        cur++;                                                              \
    }                                                                       \
    return( DLC_ERR_OID_NOT_FOUND );                                   \
}

/*
 * Macro to generate a function for retrieving the OID based on two
 * attributes from a dlc_oid_descriptor_t wrapper.
 */
#define FN_OID_GET_OID_BY_ATTR2(FN_NAME, TYPE_T, LIST, ATTR1_TYPE, ATTR1,   \
                                ATTR2_TYPE, ATTR2)                          \
int FN_NAME( ATTR1_TYPE ATTR1, ATTR2_TYPE ATTR2, const char **oid ,         \
             size_t *olen )                                                 \
{                                                                           \
    const TYPE_T *cur = LIST;                                               \
    while( cur->descriptor.asn1 != NULL ) {                                 \
        if( cur->ATTR1 == ATTR1 && cur->ATTR2 == ATTR2 ) {                  \
            *oid = cur->descriptor.asn1;                                    \
            *olen = cur->descriptor.asn1_len;                               \
            return( 0 );                                                    \
        }                                                                   \
        cur++;                                                              \
    }                                                                       \
    return( DLC_ERR_OID_NOT_FOUND );                                   \
}


/*
 * For X520 attribute types
 */
typedef struct {
    dlc_oid_descriptor_t    descriptor;
    const char          *short_name;
} oid_x520_attr_t;

static const oid_x520_attr_t oid_x520_attr_type[] =
{
    {
        { ADD_LEN( DLC_OID_AT_CN ),          "id-at-commonName",               "Common Name" },
        "CN",
    },
    {
        { ADD_LEN( DLC_OID_AT_COUNTRY ),     "id-at-countryName",              "Country" },
        "C",
    },
    {
        { ADD_LEN( DLC_OID_AT_LOCALITY ),    "id-at-locality",                 "Locality" },
        "L",
    },
    {
        { ADD_LEN( DLC_OID_AT_STATE ),       "id-at-state",                    "State" },
        "ST",
    },
    {
        { ADD_LEN( DLC_OID_AT_ORGANIZATION ),"id-at-organizationName",         "Organization" },
        "O",
    },
    {
        { ADD_LEN( DLC_OID_AT_ORG_UNIT ),    "id-at-organizationalUnitName",   "Org Unit" },
        "OU",
    },
    {
        { ADD_LEN( DLC_OID_PKCS9_EMAIL ),    "emailAddress",                   "E-mail address" },
        "emailAddress",
    },
    {
        { ADD_LEN( DLC_OID_AT_SERIAL_NUMBER ),"id-at-serialNumber",            "Serial number" },
        "serialNumber",
    },
    {
        { ADD_LEN( DLC_OID_AT_POSTAL_ADDRESS ),"id-at-postalAddress",          "Postal address" },
        "postalAddress",
    },
    {
        { ADD_LEN( DLC_OID_AT_POSTAL_CODE ), "id-at-postalCode",               "Postal code" },
        "postalCode",
    },
    {
        { ADD_LEN( DLC_OID_AT_SUR_NAME ),    "id-at-surName",                  "Surname" },
        "SN",
    },
    {
        { ADD_LEN( DLC_OID_AT_GIVEN_NAME ),  "id-at-givenName",                "Given name" },
        "GN",
    },
    {
        { ADD_LEN( DLC_OID_AT_INITIALS ),    "id-at-initials",                 "Initials" },
        "initials",
    },
    {
        { ADD_LEN( DLC_OID_AT_GENERATION_QUALIFIER ), "id-at-generationQualifier", "Generation qualifier" },
        "generationQualifier",
    },
    {
        { ADD_LEN( DLC_OID_AT_TITLE ),       "id-at-title",                    "Title" },
        "title",
    },
    {
        { ADD_LEN( DLC_OID_AT_DN_QUALIFIER ),"id-at-dnQualifier",              "Distinguished Name qualifier" },
        "dnQualifier",
    },
    {
        { ADD_LEN( DLC_OID_AT_PSEUDONYM ),   "id-at-pseudonym",                "Pseudonym" },
        "pseudonym",
    },
    {
        { ADD_LEN( DLC_OID_DOMAIN_COMPONENT ), "id-domainComponent",           "Domain component" },
        "DC",
    },
    {
        { ADD_LEN( DLC_OID_AT_UNIQUE_IDENTIFIER ), "id-at-uniqueIdentifier",    "Unique Identifier" },
        "uniqueIdentifier",
    },
    {
        { NULL, 0, NULL, NULL },
        NULL,
    }
};

FN_OID_TYPED_FROM_ASN1(oid_x520_attr_t, x520_attr, oid_x520_attr_type)
FN_OID_GET_ATTR1(dlc_oid_get_attr_short_name, oid_x520_attr_t, x520_attr, const char *, short_name)

/*
 * For X509 extensions
 */
typedef struct {
    dlc_oid_descriptor_t    descriptor;
    int                 ext_type;
} oid_x509_ext_t;

static const oid_x509_ext_t oid_x509_ext[] =
{
    {
        { ADD_LEN( DLC_OID_BASIC_CONSTRAINTS ),    "id-ce-basicConstraints",   "Basic Constraints" },
        DLC_X509_EXT_BASIC_CONSTRAINTS,
    },
    {
        { ADD_LEN( DLC_OID_KEY_USAGE ),            "id-ce-keyUsage",           "Key Usage" },
        DLC_X509_EXT_KEY_USAGE,
    },
    {
        { ADD_LEN( DLC_OID_EXTENDED_KEY_USAGE ),   "id-ce-extKeyUsage",        "Extended Key Usage" },
        DLC_X509_EXT_EXTENDED_KEY_USAGE,
    },
    {
        { ADD_LEN( DLC_OID_SUBJECT_ALT_NAME ),     "id-ce-subjectAltName",     "Subject Alt Name" },
        DLC_X509_EXT_SUBJECT_ALT_NAME,
    },
    {
        { ADD_LEN( DLC_OID_NS_CERT_TYPE ),         "id-netscape-certtype",     "Netscape Certificate Type" },
        DLC_X509_EXT_NS_CERT_TYPE,
    },
    {
        { NULL, 0, NULL, NULL },
        0,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_x509_ext_t, x509_ext, oid_x509_ext)
FN_OID_GET_ATTR1(dlc_oid_get_x509_ext_type, oid_x509_ext_t, x509_ext, int, ext_type)

static const dlc_oid_descriptor_t oid_ext_key_usage[] =
{
    { ADD_LEN( DLC_OID_SERVER_AUTH ),      "id-kp-serverAuth",      "TLS Web Server Authentication" },
    { ADD_LEN( DLC_OID_CLIENT_AUTH ),      "id-kp-clientAuth",      "TLS Web Client Authentication" },
    { ADD_LEN( DLC_OID_CODE_SIGNING ),     "id-kp-codeSigning",     "Code Signing" },
    { ADD_LEN( DLC_OID_EMAIL_PROTECTION ), "id-kp-emailProtection", "E-mail Protection" },
    { ADD_LEN( DLC_OID_TIME_STAMPING ),    "id-kp-timeStamping",    "Time Stamping" },
    { ADD_LEN( DLC_OID_OCSP_SIGNING ),     "id-kp-OCSPSigning",     "OCSP Signing" },
    { NULL, 0, NULL, NULL },
};

FN_OID_TYPED_FROM_ASN1(dlc_oid_descriptor_t, ext_key_usage, oid_ext_key_usage)
FN_OID_GET_ATTR1(dlc_oid_get_extended_key_usage, dlc_oid_descriptor_t, ext_key_usage, const char *, description)



/*
 * For SignatureAlgorithmIdentifier
 */
typedef struct {
    dlc_oid_descriptor_t    descriptor;
    dlc_md_type           md_alg;
    dlc_pk_type           pk_alg;
} oid_sig_alg_t;

static const oid_sig_alg_t oid_sig_alg[] =
{

    {
        { ADD_LEN( DLC_OID_PKCS1_MD2 ),        "md2WithRSAEncryption",     "RSA with MD2" },
        DLC_MD_MD2,      DLC_PK_RSA,
    },

    {
        { ADD_LEN( DLC_OID_PKCS1_MD4 ),        "md4WithRSAEncryption",     "RSA with MD4" },
        DLC_MD_MD4,      DLC_PK_RSA,
    },

    {
        { ADD_LEN( DLC_OID_PKCS1_MD5 ),        "md5WithRSAEncryption",     "RSA with MD5" },
        DLC_MD_MD5,      DLC_PK_RSA,
    },

    {
        { ADD_LEN( DLC_OID_PKCS1_SHA1 ),       "sha-1WithRSAEncryption",   "RSA with SHA1" },
        DLC_MD_SHA1,     DLC_PK_RSA,
    },

    {
        { ADD_LEN( DLC_OID_PKCS1_SHA224 ),     "sha224WithRSAEncryption",  "RSA with SHA-224" },
        DLC_MD_SHA224,   DLC_PK_RSA,
    },
    {
        { ADD_LEN( DLC_OID_PKCS1_SHA256 ),     "sha256WithRSAEncryption",  "RSA with SHA-256" },
        DLC_MD_SHA256,   DLC_PK_RSA,
    },

    {
        { ADD_LEN( DLC_OID_PKCS1_SHA384 ),     "sha384WithRSAEncryption",  "RSA with SHA-384" },
        DLC_MD_SHA384,   DLC_PK_RSA,
    },
    {
        { ADD_LEN( DLC_OID_PKCS1_SHA512 ),     "sha512WithRSAEncryption",  "RSA with SHA-512" },
        DLC_MD_SHA512,   DLC_PK_RSA,
    },

    {
        { ADD_LEN( DLC_OID_RSA_SHA_OBS ),      "sha-1WithRSAEncryption",   "RSA with SHA1" },
        DLC_MD_SHA1,     DLC_PK_RSA,
    },

    {
        { ADD_LEN( DLC_OID_ECDSA_SHA1 ),       "ecdsa-with-SHA1",      "ECDSA with SHA1" },
        DLC_MD_SHA1,     DLC_PK_ECDSA,
    },

    {
        { ADD_LEN( DLC_OID_ECDSA_SHA224 ),     "ecdsa-with-SHA224",    "ECDSA with SHA224" },
        DLC_MD_SHA224,   DLC_PK_ECDSA,
    },
    {
        { ADD_LEN( DLC_OID_ECDSA_SHA256 ),     "ecdsa-with-SHA256",    "ECDSA with SHA256" },
        DLC_MD_SHA256,   DLC_PK_ECDSA,
    },

    {
        { ADD_LEN( DLC_OID_ECDSA_SHA384 ),     "ecdsa-with-SHA384",    "ECDSA with SHA384" },
        DLC_MD_SHA384,   DLC_PK_ECDSA,
    },
    {
        { ADD_LEN( DLC_OID_ECDSA_SHA512 ),     "ecdsa-with-SHA512",    "ECDSA with SHA512" },
        DLC_MD_SHA512,   DLC_PK_ECDSA,
    },

    {
        { ADD_LEN( DLC_OID_RSASSA_PSS ),        "RSASSA-PSS",           "RSASSA-PSS" },
        DLC_MD_NONE,     DLC_PK_RSASSA_PSS,
    },

    {
        { NULL, 0, NULL, NULL },
        DLC_MD_NONE, DLC_PK_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_sig_alg_t, sig_alg, oid_sig_alg)
FN_OID_GET_DESCRIPTOR_ATTR1(dlc_oid_get_sig_alg_desc, oid_sig_alg_t, sig_alg, const char *, description)
FN_OID_GET_ATTR2(dlc_oid_get_sig_alg, oid_sig_alg_t, sig_alg, dlc_md_type, md_alg, dlc_pk_type, pk_alg)
FN_OID_GET_OID_BY_ATTR2(dlc_oid_get_oid_by_sig_alg, oid_sig_alg_t, oid_sig_alg, dlc_pk_type, pk_alg, dlc_md_type, md_alg)


/*
 * For PublicKeyInfo (PKCS1, RFC 5480)
 */
typedef struct {
    dlc_oid_descriptor_t    descriptor;
    dlc_pk_type           pk_alg;
} oid_pk_alg_t;

static const oid_pk_alg_t oid_pk_alg[] =
{
    {
        { ADD_LEN( DLC_OID_PKCS1_RSA ),      "rsaEncryption",   "RSA" },
        DLC_PK_RSA,
    },
    {
        { ADD_LEN( DLC_OID_EC_ALG_UNRESTRICTED ),  "id-ecPublicKey",   "Generic EC key" },
        DLC_PK_ECKEY,
    },
    {
        { ADD_LEN( DLC_OID_EC_ALG_ECDH ),          "id-ecDH",          "EC key for ECDH" },
        DLC_PK_ECKEY_DH,
    },
    {
        { NULL, 0, NULL, NULL },
        DLC_PK_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_pk_alg_t, pk_alg, oid_pk_alg)
FN_OID_GET_ATTR1(dlc_oid_get_pk_alg, oid_pk_alg_t, pk_alg, dlc_pk_type, pk_alg)
FN_OID_GET_OID_BY_ATTR1(dlc_oid_get_oid_by_pk_alg, oid_pk_alg_t, oid_pk_alg, dlc_pk_type, pk_alg)






/*
 * For PKCS#5 PBES2 encryption algorithm
 */
typedef struct {
    dlc_oid_descriptor_t    descriptor;
    dlc_cipher_type_t       cipher_alg;
} oid_cipher_alg_t;

static const oid_cipher_alg_t oid_cipher_alg[] =
{
    {
        { ADD_LEN( DLC_OID_DES_CBC ),              "desCBC",       "DES-CBC" },
        DLC_CIPHER_DES_CBC,
    },
    {
        { ADD_LEN( DLC_OID_DES_EDE3_CBC ),         "des-ede3-cbc", "DES-EDE3-CBC" },
        DLC_CIPHER_DES_EDE3_CBC,
    },
    {
        { NULL, 0, NULL, NULL },
        DLC_CIPHER_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_cipher_alg_t, cipher_alg, oid_cipher_alg)
FN_OID_GET_ATTR1(dlc_oid_get_cipher_alg, oid_cipher_alg_t, cipher_alg, dlc_cipher_type_t, cipher_alg)



/*
 * For digestAlgorithm
 */
typedef struct {
    dlc_oid_descriptor_t    descriptor;
    dlc_md_type           md_alg;
} oid_md_alg_t;

static const oid_md_alg_t oid_md_alg[] =
{

    {
        { ADD_LEN( DLC_OID_DIGEST_ALG_MD2 ),       "id-md2",       "MD2" },
        DLC_MD_MD2,
    },

    {
        { ADD_LEN( DLC_OID_DIGEST_ALG_MD4 ),       "id-md4",       "MD4" },
        DLC_MD_MD4,
    },

    {
        { ADD_LEN( DLC_OID_DIGEST_ALG_MD5 ),       "id-md5",       "MD5" },
        DLC_MD_MD5,
    },

    {
        { ADD_LEN( DLC_OID_DIGEST_ALG_SHA1 ),      "id-sha1",      "SHA-1" },
        DLC_MD_SHA1,
    },

    {
        { ADD_LEN( DLC_OID_DIGEST_ALG_SHA224 ),    "id-sha224",    "SHA-224" },
        DLC_MD_SHA224,
    },
    {
        { ADD_LEN( DLC_OID_DIGEST_ALG_SHA256 ),    "id-sha256",    "SHA-256" },
        DLC_MD_SHA256,
    },

    {
        { ADD_LEN( DLC_OID_DIGEST_ALG_SHA384 ),    "id-sha384",    "SHA-384" },
        DLC_MD_SHA384,
    },
    {
        { ADD_LEN( DLC_OID_DIGEST_ALG_SHA512 ),    "id-sha512",    "SHA-512" },
        DLC_MD_SHA512,
    },

    {
        { NULL, 0, NULL, NULL },
        DLC_MD_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_md_alg_t, md_alg, oid_md_alg)
FN_OID_GET_ATTR1(dlc_oid_get_md_alg, oid_md_alg_t, md_alg, dlc_md_type, md_alg)
FN_OID_GET_OID_BY_ATTR1(dlc_oid_get_oid_by_md, oid_md_alg_t, oid_md_alg, dlc_md_type, md_alg)

/*
 * For HMAC digestAlgorithm
 */
typedef struct {
    dlc_oid_descriptor_t    descriptor;
    dlc_md_type           md_hmac;
} oid_md_hmac_t;

static const oid_md_hmac_t oid_md_hmac[] =
{

    {
        { ADD_LEN( DLC_OID_HMAC_SHA1 ),      "hmacSHA1",      "HMAC-SHA-1" },
        DLC_MD_SHA1,
    },

    {
        { ADD_LEN( DLC_OID_HMAC_SHA224 ),    "hmacSHA224",    "HMAC-SHA-224" },
        DLC_MD_SHA224,
    },
    {
        { ADD_LEN( DLC_OID_HMAC_SHA256 ),    "hmacSHA256",    "HMAC-SHA-256" },
        DLC_MD_SHA256,
    },

    {
        { ADD_LEN( DLC_OID_HMAC_SHA384 ),    "hmacSHA384",    "HMAC-SHA-384" },
        DLC_MD_SHA384,
    },
    {
        { ADD_LEN( DLC_OID_HMAC_SHA512 ),    "hmacSHA512",    "HMAC-SHA-512" },
        DLC_MD_SHA512,
    },

    {
        { NULL, 0, NULL, NULL },
        DLC_MD_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_md_hmac_t, md_hmac, oid_md_hmac)
FN_OID_GET_ATTR1(dlc_oid_get_md_hmac, oid_md_hmac_t, md_hmac, dlc_md_type, md_hmac)


/*
 * For PKCS#12 PBEs
 */
typedef struct {
    dlc_oid_descriptor_t    descriptor;
    dlc_md_type           md_alg;
    dlc_cipher_type_t       cipher_alg;
} oid_pkcs12_pbe_alg_t;

static const oid_pkcs12_pbe_alg_t oid_pkcs12_pbe_alg[] =
{
    {
        { ADD_LEN( DLC_OID_PKCS12_PBE_SHA1_DES3_EDE_CBC ), "pbeWithSHAAnd3-KeyTripleDES-CBC", "PBE with SHA1 and 3-Key 3DES" },
        DLC_MD_SHA1,      DLC_CIPHER_DES_EDE3_CBC,
    },
    {
        { ADD_LEN( DLC_OID_PKCS12_PBE_SHA1_DES2_EDE_CBC ), "pbeWithSHAAnd2-KeyTripleDES-CBC", "PBE with SHA1 and 2-Key 3DES" },
        DLC_MD_SHA1,      DLC_CIPHER_DES_EDE_CBC,
    },
    {
        { NULL, 0, NULL, NULL },
        DLC_MD_NONE, DLC_CIPHER_NONE,
    },
};

FN_OID_TYPED_FROM_ASN1(oid_pkcs12_pbe_alg_t, pkcs12_pbe_alg, oid_pkcs12_pbe_alg)
FN_OID_GET_ATTR2(dlc_oid_get_pkcs12_pbe_alg, oid_pkcs12_pbe_alg_t, pkcs12_pbe_alg, dlc_md_type, md_alg, dlc_cipher_type_t, cipher_alg)


#define OID_SAFE_SNPRINTF                               \
    do {                                                \
        if( ret < 0 || (size_t) ret >= n )              \
            return( DLC_ERR_OID_BUF_TOO_SMALL );    \
                                                        \
        n -= (size_t) ret;                              \
        p += (size_t) ret;                              \
    } while( 0 )

/* Return the x.y.z.... style numeric string for the given OID */
int dlc_oid_get_numeric_string( char *buf, size_t size,
                            const dlc_asn1_buf *oid )
{
    int ret;
    size_t i, n;
    unsigned int value;
    char *p;

    p = buf;
    n = size;

    /* First byte contains first two dots */
    if( oid->len > 0 )
    {
        ret = dlc_snprintf( p, n, "%d.%d", oid->p[0] / 40, oid->p[0] % 40 );
        OID_SAFE_SNPRINTF;
    }

    value = 0;
    for( i = 1; i < oid->len; i++ )
    {
        /* Prevent overflow in value. */
        if( ( ( value << 7 ) >> 7 ) != value )
            return( DLC_ERR_OID_BUF_TOO_SMALL );

        value <<= 7;
        value += oid->p[i] & 0x7F;

        if( !( oid->p[i] & 0x80 ) )
        {
            /* Last byte */
            ret = dlc_snprintf( p, n, ".%d", value );
            OID_SAFE_SNPRINTF;
            value = 0;
        }
    }

    return( (int) ( size - n ) );
}

