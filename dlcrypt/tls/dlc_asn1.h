#ifndef __DLC_ANS1__
#define __DLC_ANS1__

#define DLC_ERR_ASN1_OUT_OF_DATA                      -0x0060  /**< Out of data when parsing an ASN1 data structure. */
#define DLC_ERR_ASN1_UNEXPECTED_TAG                   -0x0062  /**< ASN1 tag was of an unexpected value. */
#define DLC_ERR_ASN1_INVALID_LENGTH                   -0x0064  /**< Error when trying to determine the length or invalid length. */
#define DLC_ERR_ASN1_LENGTH_MISMATCH                  -0x0066  /**< Actual length differs from expected length. */
#define DLC_ERR_ASN1_INVALID_DATA                     -0x0068  /**< Data is invalid. (not used) */
#define DLC_ERR_ASN1_ALLOC_FAILED                     -0x006A  /**< Memory allocation failed */
#define DLC_ERR_ASN1_BUF_TOO_SMALL                    -0x006C  /**< Buffer too small when writing ASN.1 data structure. */

#define DLC_ASN1_BOOLEAN                 0x01
#define DLC_ASN1_INTEGER                 0x02
#define DLC_ASN1_BIT_STRING              0x03
#define DLC_ASN1_OCTET_STRING            0x04
#define DLC_ASN1_NULL                    0x05
#define DLC_ASN1_OID                     0x06
#define DLC_ASN1_UTF8_STRING             0x0C
#define DLC_ASN1_SEQUENCE                0x10
#define DLC_ASN1_SET                     0x11
#define DLC_ASN1_PRINTABLE_STRING        0x13
#define DLC_ASN1_T61_STRING              0x14
#define DLC_ASN1_IA5_STRING              0x16
#define DLC_ASN1_UTC_TIME                0x17
#define DLC_ASN1_GENERALIZED_TIME        0x18
#define DLC_ASN1_UNIVERSAL_STRING        0x1C
#define DLC_ASN1_BMP_STRING              0x1E
#define DLC_ASN1_PRIMITIVE               0x00
#define DLC_ASN1_CONSTRUCTED             0x20
#define DLC_ASN1_CONTEXT_SPECIFIC        0x80

/*
 * Bit masks for each of the components of an ASN.1 tag as specified in
 * ITU X.690 (08/2015), section 8.1 "General rules for encoding",
 * paragraph 8.1.2.2:
 *
 * Bit  8     7   6   5          1
 *     +-------+-----+------------+
 *     | Class | P/C | Tag number |
 *     +-------+-----+------------+
 */
#define DLC_ASN1_TAG_CLASS_MASK          0xC0
#define DLC_ASN1_TAG_PC_MASK             0x20
#define DLC_ASN1_TAG_VALUE_MASK          0x1F

/** Returns the size of the binary string, without the trailing \\0 */
#define DLC_OID_SIZE(x) (sizeof(x) - 1)

/**
 * Compares an mbedtls_asn1_buf structure to a reference OID.
 *
 * Only works for 'defined' oid_str values (DLC_OID_HMAC_SHA1), you cannot use a
 * 'unsigned char *oid' here!
 */
#define DLC_OID_CMP(oid_str, oid_buf)                                   \
        ( ( DLC_OID_SIZE(oid_str) != (oid_buf)->len ) ||                \
          memcmp( (oid_str), (oid_buf)->p, (oid_buf)->len) != 0 )


typedef struct _dlc_asn1_buf            dlc_asn1_buf;
typedef struct _dlc_asn1_bitstring      dlc_asn1_bitstring;
typedef struct _dlc_asn1_sequence       dlc_asn1_sequence;
typedef struct _dlc_asn1_named_data     dlc_asn1_named_data;

struct _dlc_asn1_buf
{
    int         tag;                /**< ASN1 type, e.g. DLC_ASN1_UTF8_STRING. */
    size_t      len;                /**< ASN1 length, in octets. */
    uchar       *p;                 /**< ASN1 data, e.g. in ASCII. */
};

struct _dlc_asn1_bitstring
{
    size_t      len;                /**< ASN1 length, in octets. */
    uchar       unused_bits;        /**< Number of unused bits at the end of the string */
    uchar       *p;                 /**< Raw ASN1 data for the bit string */
};

struct _dlc_asn1_sequence
{
    dlc_asn1_buf        buf;        /**< Buffer containing the given ASN.1 item. */
    dlc_asn1_sequence   *next;      /**< The next entry in the sequence. */
};

struct _dlc_asn1_named_data
{
    dlc_asn1_buf            oid;                   /**< The object identifier. */
    dlc_asn1_buf            val;                   /**< The named value. */
    dlc_asn1_named_data     *next;              /**< The next entry in the sequence. */
    uchar                   next_merged;      /**< Merge next item into the current one? */
};

int dlc_asn1_get_len( uchar **p, const uchar *end, size_t *len);
int dlc_asn1_get_tag( uchar **p, const uchar *end, size_t *len, int tag );
int dlc_asn1_get_int( uchar **p, const uchar *end, int *val );
int dlc_asn1_get_alg( uchar **p, const uchar *end, dlc_asn1_buf *alg, dlc_asn1_buf *params );
int dlc_asn1_get_bitstring_null( uchar **p, const uchar *end, size_t *len );
int dlc_asn1_get_bool( uchar **p, const uchar *end, int *val );
int dlc_asn1_get_bitstring( uchar **p, const uchar *end, dlc_asn1_bitstring *bs);
int dlc_asn1_get_sequence_of( uchar **p, const uchar *end, dlc_asn1_sequence *cur, int tag);


#endif