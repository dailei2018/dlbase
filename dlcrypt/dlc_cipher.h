#ifndef __DLC_CIPHER__
#define __DLC_CIPHER__

/**
 * \brief     Supported cipher types.
 *
 * \warning   RC4 and DES are considered weak ciphers and their use
 *            constitutes a security risk. Arm recommends considering stronger
 *            ciphers instead.
 */
typedef enum {
    DLC_CIPHER_ID_NONE = 0,  /**< Placeholder to mark the end of cipher ID lists. */
    DLC_CIPHER_ID_NULL,      /**< The identity cipher, treated as a stream cipher. */
    DLC_CIPHER_ID_AES,       /**< The AES cipher. */
    DLC_CIPHER_ID_DES,       /**< The DES cipher. */
    DLC_CIPHER_ID_3DES,      /**< The Triple DES cipher. */
    DLC_CIPHER_ID_CAMELLIA,  /**< The Camellia cipher. */
    DLC_CIPHER_ID_BLOWFISH,  /**< The Blowfish cipher. */
    DLC_CIPHER_ID_ARC4,      /**< The RC4 cipher. */
    DLC_CIPHER_ID_ARIA,      /**< The Aria cipher. */
    DLC_CIPHER_ID_CHACHA20,  /**< The ChaCha20 cipher. */
} dlc_cipher_id_t;

/**
 * \brief     Supported {cipher type, cipher mode} pairs.
 *
 * \warning   RC4 and DES are considered weak ciphers and their use
 *            constitutes a security risk. Arm recommends considering stronger
 *            ciphers instead.
 */
typedef enum {
    DLC_CIPHER_NONE = 0,             /**< Placeholder to mark the end of cipher-pair lists. */
    DLC_CIPHER_NULL,                 /**< The identity stream cipher. */
    DLC_CIPHER_AES_128_ECB,          /**< AES cipher with 128-bit ECB mode. */
    DLC_CIPHER_AES_192_ECB,          /**< AES cipher with 192-bit ECB mode. */
    DLC_CIPHER_AES_256_ECB,          /**< AES cipher with 256-bit ECB mode. */
    DLC_CIPHER_AES_128_CBC,          /**< AES cipher with 128-bit CBC mode. */
    DLC_CIPHER_AES_192_CBC,          /**< AES cipher with 192-bit CBC mode. */
    DLC_CIPHER_AES_256_CBC,          /**< AES cipher with 256-bit CBC mode. */
    DLC_CIPHER_AES_128_CFB128,       /**< AES cipher with 128-bit CFB128 mode. */
    DLC_CIPHER_AES_192_CFB128,       /**< AES cipher with 192-bit CFB128 mode. */
    DLC_CIPHER_AES_256_CFB128,       /**< AES cipher with 256-bit CFB128 mode. */
    DLC_CIPHER_AES_128_CTR,          /**< AES cipher with 128-bit CTR mode. */
    DLC_CIPHER_AES_192_CTR,          /**< AES cipher with 192-bit CTR mode. */
    DLC_CIPHER_AES_256_CTR,          /**< AES cipher with 256-bit CTR mode. */
    DLC_CIPHER_AES_128_GCM,          /**< AES cipher with 128-bit GCM mode. */
    DLC_CIPHER_AES_192_GCM,          /**< AES cipher with 192-bit GCM mode. */
    DLC_CIPHER_AES_256_GCM,          /**< AES cipher with 256-bit GCM mode. */
    DLC_CIPHER_CAMELLIA_128_ECB,     /**< Camellia cipher with 128-bit ECB mode. */
    DLC_CIPHER_CAMELLIA_192_ECB,     /**< Camellia cipher with 192-bit ECB mode. */
    DLC_CIPHER_CAMELLIA_256_ECB,     /**< Camellia cipher with 256-bit ECB mode. */
    DLC_CIPHER_CAMELLIA_128_CBC,     /**< Camellia cipher with 128-bit CBC mode. */
    DLC_CIPHER_CAMELLIA_192_CBC,     /**< Camellia cipher with 192-bit CBC mode. */
    DLC_CIPHER_CAMELLIA_256_CBC,     /**< Camellia cipher with 256-bit CBC mode. */
    DLC_CIPHER_CAMELLIA_128_CFB128,  /**< Camellia cipher with 128-bit CFB128 mode. */
    DLC_CIPHER_CAMELLIA_192_CFB128,  /**< Camellia cipher with 192-bit CFB128 mode. */
    DLC_CIPHER_CAMELLIA_256_CFB128,  /**< Camellia cipher with 256-bit CFB128 mode. */
    DLC_CIPHER_CAMELLIA_128_CTR,     /**< Camellia cipher with 128-bit CTR mode. */
    DLC_CIPHER_CAMELLIA_192_CTR,     /**< Camellia cipher with 192-bit CTR mode. */
    DLC_CIPHER_CAMELLIA_256_CTR,     /**< Camellia cipher with 256-bit CTR mode. */
    DLC_CIPHER_CAMELLIA_128_GCM,     /**< Camellia cipher with 128-bit GCM mode. */
    DLC_CIPHER_CAMELLIA_192_GCM,     /**< Camellia cipher with 192-bit GCM mode. */
    DLC_CIPHER_CAMELLIA_256_GCM,     /**< Camellia cipher with 256-bit GCM mode. */
    DLC_CIPHER_DES_ECB,              /**< DES cipher with ECB mode. */
    DLC_CIPHER_DES_CBC,              /**< DES cipher with CBC mode. */
    DLC_CIPHER_DES_EDE_ECB,          /**< DES cipher with EDE ECB mode. */
    DLC_CIPHER_DES_EDE_CBC,          /**< DES cipher with EDE CBC mode. */
    DLC_CIPHER_DES_EDE3_ECB,         /**< DES cipher with EDE3 ECB mode. */
    DLC_CIPHER_DES_EDE3_CBC,         /**< DES cipher with EDE3 CBC mode. */
    DLC_CIPHER_BLOWFISH_ECB,         /**< Blowfish cipher with ECB mode. */
    DLC_CIPHER_BLOWFISH_CBC,         /**< Blowfish cipher with CBC mode. */
    DLC_CIPHER_BLOWFISH_CFB64,       /**< Blowfish cipher with CFB64 mode. */
    DLC_CIPHER_BLOWFISH_CTR,         /**< Blowfish cipher with CTR mode. */
    DLC_CIPHER_ARC4_128,             /**< RC4 cipher with 128-bit mode. */
    DLC_CIPHER_AES_128_CCM,          /**< AES cipher with 128-bit CCM mode. */
    DLC_CIPHER_AES_192_CCM,          /**< AES cipher with 192-bit CCM mode. */
    DLC_CIPHER_AES_256_CCM,          /**< AES cipher with 256-bit CCM mode. */
    DLC_CIPHER_CAMELLIA_128_CCM,     /**< Camellia cipher with 128-bit CCM mode. */
    DLC_CIPHER_CAMELLIA_192_CCM,     /**< Camellia cipher with 192-bit CCM mode. */
    DLC_CIPHER_CAMELLIA_256_CCM,     /**< Camellia cipher with 256-bit CCM mode. */
    DLC_CIPHER_ARIA_128_ECB,         /**< Aria cipher with 128-bit key and ECB mode. */
    DLC_CIPHER_ARIA_192_ECB,         /**< Aria cipher with 192-bit key and ECB mode. */
    DLC_CIPHER_ARIA_256_ECB,         /**< Aria cipher with 256-bit key and ECB mode. */
    DLC_CIPHER_ARIA_128_CBC,         /**< Aria cipher with 128-bit key and CBC mode. */
    DLC_CIPHER_ARIA_192_CBC,         /**< Aria cipher with 192-bit key and CBC mode. */
    DLC_CIPHER_ARIA_256_CBC,         /**< Aria cipher with 256-bit key and CBC mode. */
    DLC_CIPHER_ARIA_128_CFB128,      /**< Aria cipher with 128-bit key and CFB-128 mode. */
    DLC_CIPHER_ARIA_192_CFB128,      /**< Aria cipher with 192-bit key and CFB-128 mode. */
    DLC_CIPHER_ARIA_256_CFB128,      /**< Aria cipher with 256-bit key and CFB-128 mode. */
    DLC_CIPHER_ARIA_128_CTR,         /**< Aria cipher with 128-bit key and CTR mode. */
    DLC_CIPHER_ARIA_192_CTR,         /**< Aria cipher with 192-bit key and CTR mode. */
    DLC_CIPHER_ARIA_256_CTR,         /**< Aria cipher with 256-bit key and CTR mode. */
    DLC_CIPHER_ARIA_128_GCM,         /**< Aria cipher with 128-bit key and GCM mode. */
    DLC_CIPHER_ARIA_192_GCM,         /**< Aria cipher with 192-bit key and GCM mode. */
    DLC_CIPHER_ARIA_256_GCM,         /**< Aria cipher with 256-bit key and GCM mode. */
    DLC_CIPHER_ARIA_128_CCM,         /**< Aria cipher with 128-bit key and CCM mode. */
    DLC_CIPHER_ARIA_192_CCM,         /**< Aria cipher with 192-bit key and CCM mode. */
    DLC_CIPHER_ARIA_256_CCM,         /**< Aria cipher with 256-bit key and CCM mode. */
    DLC_CIPHER_AES_128_OFB,          /**< AES 128-bit cipher in OFB mode. */
    DLC_CIPHER_AES_192_OFB,          /**< AES 192-bit cipher in OFB mode. */
    DLC_CIPHER_AES_256_OFB,          /**< AES 256-bit cipher in OFB mode. */
    DLC_CIPHER_AES_128_XTS,          /**< AES 128-bit cipher in XTS block mode. */
    DLC_CIPHER_AES_256_XTS,          /**< AES 256-bit cipher in XTS block mode. */
    DLC_CIPHER_CHACHA20,             /**< ChaCha20 stream cipher. */
    DLC_CIPHER_CHACHA20_POLY1305,    /**< ChaCha20-Poly1305 AEAD cipher. */
} dlc_cipher_type_t;

/** Supported cipher modes. */
typedef enum {
    DLC_MODE_NONE = 0,               /**< None. */
    DLC_MODE_ECB,                    /**< The ECB cipher mode. */
    DLC_MODE_CBC,                    /**< The CBC cipher mode. */
    DLC_MODE_CFB,                    /**< The CFB cipher mode. */
    DLC_MODE_OFB,                    /**< The OFB cipher mode. */
    DLC_MODE_CTR,                    /**< The CTR cipher mode. */
    DLC_MODE_GCM,                    /**< The GCM cipher mode. */
    DLC_MODE_STREAM,                 /**< The stream cipher mode. */
    DLC_MODE_CCM,                    /**< The CCM cipher mode. */
    DLC_MODE_XTS,                    /**< The XTS cipher mode. */
    DLC_MODE_CHACHAPOLY,             /**< The ChaCha-Poly cipher mode. */
} dlc_cipher_mode_t;

/** Supported cipher padding types. */
typedef enum {
    DLC_PADDING_PKCS7 = 0,     /**< PKCS7 padding (default).        */
    DLC_PADDING_ONE_AND_ZEROS, /**< ISO/IEC 7816-4 padding.         */
    DLC_PADDING_ZEROS_AND_LEN, /**< ANSI X.923 padding.             */
    DLC_PADDING_ZEROS,         /**< Zero padding (not reversible). */
    DLC_PADDING_NONE,          /**< Never pad (full blocks only).   */
} dlc_cipher_padding_t;

/** Type of operation. */
typedef enum {
    DLC_OPERATION_NONE = -1,
    DLC_DECRYPT = 0,
    DLC_ENCRYPT,
} dlc_operation_t;

enum {
    /** Undefined key length. */
    DLC_KEY_LENGTH_NONE = 0,
    /** Key length, in bits (including parity), for DES keys. */
    DLC_KEY_LENGTH_DES  = 64,
    /** Key length in bits, including parity, for DES in two-key EDE. */
    DLC_KEY_LENGTH_DES_EDE = 128,
    /** Key length in bits, including parity, for DES in three-key EDE. */
    DLC_KEY_LENGTH_DES_EDE3 = 192,
};









#endif