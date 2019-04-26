#ifndef __DLC_PK__
#define __DLC_PK__

#define DLC_ERR_PK_ALLOC_FAILED        -0x3F80  /**< Memory allocation failed. */
#define DLC_ERR_PK_TYPE_MISMATCH       -0x3F00  /**< Type mismatch, eg attempt to encrypt with an ECDSA key */
#define DLC_ERR_PK_BAD_INPUT_DATA      -0x3E80  /**< Bad input parameters to function. */
#define DLC_ERR_PK_FILE_IO_ERROR       -0x3E00  /**< Read/write of file failed. */
#define DLC_ERR_PK_KEY_INVALID_VERSION -0x3D80  /**< Unsupported key version */
#define DLC_ERR_PK_KEY_INVALID_FORMAT  -0x3D00  /**< Invalid key tag or value. */
#define DLC_ERR_PK_UNKNOWN_PK_ALG      -0x3C80  /**< Key algorithm is unsupported (only RSA and EC are supported). */
#define DLC_ERR_PK_PASSWORD_REQUIRED   -0x3C00  /**< Private key password can't be empty. */
#define DLC_ERR_PK_PASSWORD_MISMATCH   -0x3B80  /**< Given private key password does not allow for correct decryption. */
#define DLC_ERR_PK_INVALID_PUBKEY      -0x3B00  /**< The pubkey tag or value is invalid (only RSA and EC are supported). */
#define DLC_ERR_PK_INVALID_ALG         -0x3A80  /**< The algorithm tag or value is invalid. */
#define DLC_ERR_PK_UNKNOWN_NAMED_CURVE -0x3A00  /**< Elliptic curve is unsupported (only NIST curves are supported). */
#define DLC_ERR_PK_FEATURE_UNAVAILABLE -0x3980  /**< Unavailable feature, e.g. RSA disabled for RSA key. */
#define DLC_ERR_PK_SIG_LEN_MISMATCH    -0x3900  /**< The buffer contains a valid signature followed by more data. */

typedef struct _dlc_pk_context  dlc_pk_context;
typedef struct _dlc_pk_info     dlc_pk_info;

typedef struct _dlc_rsa_context dlc_rsa_context;

typedef enum {
    DLC_PK_NONE=0,
    DLC_PK_RSA,
    DLC_PK_ECKEY,
    DLC_PK_ECKEY_DH,
    DLC_PK_ECDSA,
    DLC_PK_RSA_ALT,
    DLC_PK_RSASSA_PSS,
} dlc_pk_type;

typedef enum {
    DLC_MD_NONE=0,    /**< None. */
    DLC_MD_MD2,       /**< The MD2 message digest. */
    DLC_MD_MD4,       /**< The MD4 message digest. */
    DLC_MD_MD5,       /**< The MD5 message digest. */
    DLC_MD_SHA1,      /**< The SHA-1 message digest. */
    DLC_MD_SHA224,    /**< The SHA-224 message digest. */
    DLC_MD_SHA256,    /**< The SHA-256 message digest. */
    DLC_MD_SHA384,    /**< The SHA-384 message digest. */
    DLC_MD_SHA512,    /**< The SHA-512 message digest. */
    DLC_MD_RIPEMD160, /**< The RIPEMD-160 message digest. */
} dlc_md_type;


struct _dlc_pk_info {
    dlc_pk_type     type;
    char            *name;
};

struct _dlc_pk_context
{
    dlc_pk_info     *pk_info; /**< Public key information         */
    void *           pk_ctx;  /**< Underlying public key context  */
};

struct _dlc_rsa_context{
    rsa_public_key      pub;
};

dlc_pk_info * dlc_pk_info_from_type(dlc_pk_type pk_type);
int dlc_pk_setup( dlc_pk_context *ctx, dlc_pk_info *info );
int dlc_pk_get_rsapubkey( uchar **p, const uchar *end, dlc_rsa_context *rsa );
int dlc_pk_get_pk_alg( uchar **p, const uchar *end, dlc_pk_type *pk_alg, dlc_asn1_buf *params );

#endif