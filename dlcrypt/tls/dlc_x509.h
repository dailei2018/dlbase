#ifndef __DLC_X509__
#define __DLC_X509__

#include "dlc_base.h"
#include "dlc_pk.h"

#define DLC_ERR_X509_FEATURE_UNAVAILABLE              -0x2080  /**< Unavailable feature, e.g. RSA hashing/encryption combination. */
#define DLC_ERR_X509_UNKNOWN_OID                      -0x2100  /**< Requested OID is unknown. */
#define DLC_ERR_X509_INVALID_FORMAT                   -0x2180  /**< The CRT/CRL/CSR format is invalid, e.g. different type expected. */
#define DLC_ERR_X509_INVALID_VERSION                  -0x2200  /**< The CRT/CRL/CSR version element is invalid. */
#define DLC_ERR_X509_INVALID_SERIAL                   -0x2280  /**< The serial tag or value is invalid. */
#define DLC_ERR_X509_INVALID_ALG                      -0x2300  /**< The algorithm tag or value is invalid. */
#define DLC_ERR_X509_INVALID_NAME                     -0x2380  /**< The name tag or value is invalid. */
#define DLC_ERR_X509_INVALID_DATE                     -0x2400  /**< The date tag or value is invalid. */
#define DLC_ERR_X509_INVALID_SIGNATURE                -0x2480  /**< The signature tag or value invalid. */
#define DLC_ERR_X509_INVALID_EXTENSIONS               -0x2500  /**< The extension tag or value is invalid. */
#define DLC_ERR_X509_UNKNOWN_VERSION                  -0x2580  /**< CRT/CRL/CSR has an unsupported version number. */
#define DLC_ERR_X509_UNKNOWN_SIG_ALG                  -0x2600  /**< Signature algorithm (oid) is unsupported. */
#define DLC_ERR_X509_SIG_MISMATCH                     -0x2680  /**< Signature algorithms do not match. (see \c ::dlc_x509_crt sig_oid) */
#define DLC_ERR_X509_CERT_VERIFY_FAILED               -0x2700  /**< Certificate verification failed, e.g. CRL, CA or signature check failed. */
#define DLC_ERR_X509_CERT_UNKNOWN_FORMAT              -0x2780  /**< Format not recognized as DER or PEM. */
#define DLC_ERR_X509_BAD_INPUT_DATA                   -0x2800  /**< Input invalid. */
#define DLC_ERR_X509_ALLOC_FAILED                     -0x2880  /**< Allocation of memory failed. */
#define DLC_ERR_X509_FILE_IO_ERROR                    -0x2900  /**< Read/write of file failed. */
#define DLC_ERR_X509_BUFFER_TOO_SMALL                 -0x2980  /**< Destination buffer is too small. */
#define DLC_ERR_X509_FATAL_ERROR                      -0x3000  /**< A fatal error occured, eg the chain is too long or the vrfy callback failed. */
/* \} name */

/**
 * \name X509 Verify codes
 * \{
 */
/* Reminder: update x509_crt_verify_strings[] in library/x509_crt.c */
#define DLC_X509_BADCERT_EXPIRED             0x01  /**< The certificate validity has expired. */
#define DLC_X509_BADCERT_REVOKED             0x02  /**< The certificate has been revoked (is on a CRL). */
#define DLC_X509_BADCERT_CN_MISMATCH         0x04  /**< The certificate Common Name (CN) does not match with the expected CN. */
#define DLC_X509_BADCERT_NOT_TRUSTED         0x08  /**< The certificate is not correctly signed by the trusted CA. */
#define DLC_X509_BADCRL_NOT_TRUSTED          0x10  /**< The CRL is not correctly signed by the trusted CA. */
#define DLC_X509_BADCRL_EXPIRED              0x20  /**< The CRL is expired. */
#define DLC_X509_BADCERT_MISSING             0x40  /**< Certificate was missing. */
#define DLC_X509_BADCERT_SKIP_VERIFY         0x80  /**< Certificate verification was skipped. */
#define DLC_X509_BADCERT_OTHER             0x0100  /**< Other reason (can be used by verify callback) */
#define DLC_X509_BADCERT_FUTURE            0x0200  /**< The certificate validity starts in the future. */
#define DLC_X509_BADCRL_FUTURE             0x0400  /**< The CRL is from the future */
#define DLC_X509_BADCERT_KEY_USAGE         0x0800  /**< Usage does not match the keyUsage extension. */
#define DLC_X509_BADCERT_EXT_KEY_USAGE     0x1000  /**< Usage does not match the extendedKeyUsage extension. */
#define DLC_X509_BADCERT_NS_CERT_TYPE      0x2000  /**< Usage does not match the nsCertType extension. */
#define DLC_X509_BADCERT_BAD_MD            0x4000  /**< The certificate is signed with an unacceptable hash. */
#define DLC_X509_BADCERT_BAD_PK            0x8000  /**< The certificate is signed with an unacceptable PK alg (eg RSA vs ECDSA). */
#define DLC_X509_BADCERT_BAD_KEY         0x010000  /**< The certificate is signed with an unacceptable key (eg bad curve, RSA too short). */
#define DLC_X509_BADCRL_BAD_MD           0x020000  /**< The CRL is signed with an unacceptable hash. */
#define DLC_X509_BADCRL_BAD_PK           0x040000  /**< The CRL is signed with an unacceptable PK alg (eg RSA vs ECDSA). */
#define DLC_X509_BADCRL_BAD_KEY          0x080000  /**< The CRL is signed with an unacceptable key (eg bad curve, RSA too short). */

/* \} name */
/* \} addtogroup x509_module */

/*
 * X.509 v3 Key Usage Extension flags
 * Reminder: update x509_info_key_usage() when adding new flags.
 */
#define DLC_X509_KU_DIGITAL_SIGNATURE            (0x80)  /* bit 0 */
#define DLC_X509_KU_NON_REPUDIATION              (0x40)  /* bit 1 */
#define DLC_X509_KU_KEY_ENCIPHERMENT             (0x20)  /* bit 2 */
#define DLC_X509_KU_DATA_ENCIPHERMENT            (0x10)  /* bit 3 */
#define DLC_X509_KU_KEY_AGREEMENT                (0x08)  /* bit 4 */
#define DLC_X509_KU_KEY_CERT_SIGN                (0x04)  /* bit 5 */
#define DLC_X509_KU_CRL_SIGN                     (0x02)  /* bit 6 */
#define DLC_X509_KU_ENCIPHER_ONLY                (0x01)  /* bit 7 */
#define DLC_X509_KU_DECIPHER_ONLY              (0x8000)  /* bit 8 */

/*
 * Netscape certificate types
 * (http://www.mozilla.org/projects/security/pki/nss/tech-notes/tn3.html)
 */

#define DLC_X509_NS_CERT_TYPE_SSL_CLIENT         (0x80)  /* bit 0 */
#define DLC_X509_NS_CERT_TYPE_SSL_SERVER         (0x40)  /* bit 1 */
#define DLC_X509_NS_CERT_TYPE_EMAIL              (0x20)  /* bit 2 */
#define DLC_X509_NS_CERT_TYPE_OBJECT_SIGNING     (0x10)  /* bit 3 */
#define DLC_X509_NS_CERT_TYPE_RESERVED           (0x08)  /* bit 4 */
#define DLC_X509_NS_CERT_TYPE_SSL_CA             (0x04)  /* bit 5 */
#define DLC_X509_NS_CERT_TYPE_EMAIL_CA           (0x02)  /* bit 6 */
#define DLC_X509_NS_CERT_TYPE_OBJECT_SIGNING_CA  (0x01)  /* bit 7 */

/*
 * X.509 extension types
 *
 * Comments refer to the status for using certificates. Status can be
 * different for writing certificates or reading CRLs or CSRs.
 */
#define DLC_X509_EXT_AUTHORITY_KEY_IDENTIFIER    (1 << 0)
#define DLC_X509_EXT_SUBJECT_KEY_IDENTIFIER      (1 << 1)
#define DLC_X509_EXT_KEY_USAGE                   (1 << 2)
#define DLC_X509_EXT_CERTIFICATE_POLICIES        (1 << 3)
#define DLC_X509_EXT_POLICY_MAPPINGS             (1 << 4)
#define DLC_X509_EXT_SUBJECT_ALT_NAME            (1 << 5)    /* Supported (DNS) */
#define DLC_X509_EXT_ISSUER_ALT_NAME             (1 << 6)
#define DLC_X509_EXT_SUBJECT_DIRECTORY_ATTRS     (1 << 7)
#define DLC_X509_EXT_BASIC_CONSTRAINTS           (1 << 8)    /* Supported */
#define DLC_X509_EXT_NAME_CONSTRAINTS            (1 << 9)
#define DLC_X509_EXT_POLICY_CONSTRAINTS          (1 << 10)
#define DLC_X509_EXT_EXTENDED_KEY_USAGE          (1 << 11)
#define DLC_X509_EXT_CRL_DISTRIBUTION_POINTS     (1 << 12)
#define DLC_X509_EXT_INIHIBIT_ANYPOLICY          (1 << 13)
#define DLC_X509_EXT_FRESHEST_CRL                (1 << 14)

#define DLC_X509_EXT_NS_CERT_TYPE                (1 << 16)

/*
 * Storage format identifiers
 * Recognized formats: PEM and DER
 */
#define DLC_X509_FORMAT_DER                 1
#define DLC_X509_FORMAT_PEM                 2

#define DLC_X509_MAX_DN_NAME_SIZE         256 /**< Maximum value size of a DN entry */


typedef struct _dlc_x509_time       dlc_x509_time;
typedef struct _dlc_x509_crt        dlc_x509_crt;

struct _dlc_x509_time {
    int year, mon, day;         /**< Date. */
    int hour, min, sec;         /**< Time. */
};


struct _dlc_x509_crt
{
    dlc_asn1_buf raw;               /**< The raw certificate data (DER). */
    dlc_asn1_buf tbs;               /**< The raw certificate body (DER). The part that is To Be Signed. */

    int version;                /**< The X.509 version. (1=v1, 2=v2, 3=v3) */
    dlc_asn1_buf serial;            /**< Unique id for certificate issued by a specific CA. */
    dlc_asn1_buf sig_oid;           /**< Signature algorithm, e.g. sha1RSA */

    dlc_asn1_buf issuer_raw;        /**< The raw issuer data (DER). Used for quick comparison. */
    dlc_asn1_buf subject_raw;       /**< The raw subject data (DER). Used for quick comparison. */

    dlc_asn1_named_data issuer;           /**< The parsed issuer data (named information object). */
    dlc_asn1_named_data subject;          /**< The parsed subject data (named information object). */

    dlc_x509_time valid_from;       /**< Start time of certificate validity. */
    dlc_x509_time valid_to;         /**< End time of certificate validity. */

    dlc_pk_context  pk;              /**< Container for the public key context. */

    dlc_asn1_buf issuer_id;         /**< Optional X.509 v2/v3 issuer unique identifier. */
    dlc_asn1_buf subject_id;        /**< Optional X.509 v2/v3 subject unique identifier. */
    dlc_asn1_buf v3_ext;            /**< Optional X.509 v3 extensions.  */
    dlc_asn1_sequence subject_alt_names;    /**< Optional list of Subject Alternative Names (Only dNSName supported). */

    int ext_types;              /**< Bit string containing detected and parsed extensions */
    int ca_istrue;              /**< Optional Basic Constraint extension value: 1 if this certificate belongs to a CA, 0 otherwise. */
    int max_pathlen;            /**< Optional Basic Constraint extension value: The maximum path length to the root certificate. Path length is 1 higher than RFC 5280 'meaning', so 1+ */

    unsigned int key_usage;     /**< Optional key usage extension value: See the values in x509.h */

    dlc_asn1_sequence ext_key_usage; /**< Optional list of extended key usage OIDs. */

    uchar       ns_cert_type; /**< Optional Netscape certificate type extension value: See the values in x509.h */

    dlc_asn1_buf sig;               /**< Signature: hash of the tbs part signed with the private key. */
    dlc_md_type sig_md;           /**< Internal representation of the MD algorithm of the signature algorithm, e.g. MBEDTLS_MD_SHA256 */
    dlc_pk_type sig_pk;           /**< Internal representation of the Public Key algorithm of the signature algorithm, e.g. MBEDTLS_PK_RSA */
    void *sig_opts;             /**< Signature options to be passed to dlc_pk_verify_ext(), e.g. for RSASSA-PSS */

    dlc_x509_crt *next;     /**< Next certificate in the CA-chain. */
};


int dlc_x509_get_name( unsigned char **p, const unsigned char *end,
                   dlc_asn1_named_data *cur );
int dlc_x509_get_time( unsigned char **p, const unsigned char *end, dlc_x509_time *tm );
int dlc_x509_get_version( uchar **p, const uchar *end, int *ver );
int dlc_x509_get_serial(uchar **p, const uchar *end, dlc_asn1_buf *serial);

int dlc_x509_get_alg( uchar **p, const uchar *end, dlc_asn1_buf *alg, dlc_asn1_buf *params);
int dlc_x509_get_sig_alg( const dlc_asn1_buf *sig_oid, const dlc_asn1_buf *sig_params,
                      dlc_md_type *md_alg, dlc_pk_type *pk_alg,
                      void **sig_opts );
int dlc_pk_parse_subpubkey( uchar **p, const uchar *end, dlc_pk_context *pk );
int dlc_x509_get_ext( uchar **p, const uchar *end, dlc_asn1_buf *ext, int tag );

char *dlc_pem_to_der(char *pem, size_t len, int *slen);
int dlc_x509_crt_parse_der_core( dlc_x509_crt *crt, uchar *buf, size_t buflen );
int dlc_x509_get_sig( unsigned char **p, const unsigned char *end, dlc_asn1_buf *sig );

#endif