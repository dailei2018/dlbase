#ifndef __DLC_OID__
#define __DLC_OID__

#include "dlc_cipher.h"

#define DLC_ERR_OID_NOT_FOUND                         -0x002E  /**< OID is not found. */
#define DLC_ERR_OID_BUF_TOO_SMALL                     -0x000B  /**< output buffer is too small */

/*
 * Top level OID tuples
 */
#define DLC_OID_ISO_MEMBER_BODIES           "\x2a"          /* {iso(1) member-body(2)} */
#define DLC_OID_ISO_IDENTIFIED_ORG          "\x2b"          /* {iso(1) identified-organization(3)} */
#define DLC_OID_ISO_CCITT_DS                "\x55"          /* {joint-iso-ccitt(2) ds(5)} */
#define DLC_OID_ISO_ITU_COUNTRY             "\x60"          /* {joint-iso-itu-t(2) country(16)} */

/*
 * ISO Member bodies OID parts
 */
#define DLC_OID_COUNTRY_US                  "\x86\x48"      /* {us(840)} */
#define DLC_OID_ORG_RSA_DATA_SECURITY       "\x86\xf7\x0d"  /* {rsadsi(113549)} */
#define DLC_OID_RSA_COMPANY                 DLC_OID_ISO_MEMBER_BODIES DLC_OID_COUNTRY_US \
                                        DLC_OID_ORG_RSA_DATA_SECURITY /* {iso(1) member-body(2) us(840) rsadsi(113549)} */
#define DLC_OID_ORG_ANSI_X9_62              "\xce\x3d" /* ansi-X9-62(10045) */
#define DLC_OID_ANSI_X9_62                  DLC_OID_ISO_MEMBER_BODIES DLC_OID_COUNTRY_US \
                                        DLC_OID_ORG_ANSI_X9_62

/*
 * ISO Identified organization OID parts
 */
#define DLC_OID_ORG_DOD                     "\x06"          /* {dod(6)} */
#define DLC_OID_ORG_OIW                     "\x0e"
#define DLC_OID_OIW_SECSIG                  DLC_OID_ORG_OIW "\x03"
#define DLC_OID_OIW_SECSIG_ALG              DLC_OID_OIW_SECSIG "\x02"
#define DLC_OID_OIW_SECSIG_SHA1             DLC_OID_OIW_SECSIG_ALG "\x1a"
#define DLC_OID_ORG_CERTICOM                "\x81\x04"  /* certicom(132) */
#define DLC_OID_CERTICOM                    DLC_OID_ISO_IDENTIFIED_ORG DLC_OID_ORG_CERTICOM
#define DLC_OID_ORG_TELETRUST               "\x24" /* teletrust(36) */
#define DLC_OID_TELETRUST                   DLC_OID_ISO_IDENTIFIED_ORG DLC_OID_ORG_TELETRUST

/*
 * ISO ITU OID parts
 */
#define DLC_OID_ORGANIZATION                "\x01"          /* {organization(1)} */
#define DLC_OID_ISO_ITU_US_ORG              DLC_OID_ISO_ITU_COUNTRY DLC_OID_COUNTRY_US DLC_OID_ORGANIZATION /* {joint-iso-itu-t(2) country(16) us(840) organization(1)} */

#define DLC_OID_ORG_GOV                     "\x65"          /* {gov(101)} */
#define DLC_OID_GOV                         DLC_OID_ISO_ITU_US_ORG DLC_OID_ORG_GOV /* {joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101)} */

#define DLC_OID_ORG_NETSCAPE                "\x86\xF8\x42"  /* {netscape(113730)} */
#define DLC_OID_NETSCAPE                    DLC_OID_ISO_ITU_US_ORG DLC_OID_ORG_NETSCAPE /* Netscape OID {joint-iso-itu-t(2) country(16) us(840) organization(1) netscape(113730)} */

/* ISO arc for standard certificate and CRL extensions */
#define DLC_OID_ID_CE                       DLC_OID_ISO_CCITT_DS "\x1D" /**< id-ce OBJECT IDENTIFIER  ::=  {joint-iso-ccitt(2) ds(5) 29} */

#define DLC_OID_NIST_ALG                    DLC_OID_GOV "\x03\x04" /** { joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101) csor(3) nistAlgorithm(4) */

/**
 * Private Internet Extensions
 * { iso(1) identified-organization(3) dod(6) internet(1)
 *                      security(5) mechanisms(5) pkix(7) }
 */
#define DLC_OID_PKIX                        DLC_OID_ISO_IDENTIFIED_ORG DLC_OID_ORG_DOD "\x01\x05\x05\x07"

/*
 * Arc for standard naming attributes
 */
#define DLC_OID_AT                          DLC_OID_ISO_CCITT_DS "\x04" /**< id-at OBJECT IDENTIFIER ::= {joint-iso-ccitt(2) ds(5) 4} */
#define DLC_OID_AT_CN                       DLC_OID_AT "\x03" /**< id-at-commonName AttributeType:= {id-at 3} */
#define DLC_OID_AT_SUR_NAME                 DLC_OID_AT "\x04" /**< id-at-surName AttributeType:= {id-at 4} */
#define DLC_OID_AT_SERIAL_NUMBER            DLC_OID_AT "\x05" /**< id-at-serialNumber AttributeType:= {id-at 5} */
#define DLC_OID_AT_COUNTRY                  DLC_OID_AT "\x06" /**< id-at-countryName AttributeType:= {id-at 6} */
#define DLC_OID_AT_LOCALITY                 DLC_OID_AT "\x07" /**< id-at-locality AttributeType:= {id-at 7} */
#define DLC_OID_AT_STATE                    DLC_OID_AT "\x08" /**< id-at-state AttributeType:= {id-at 8} */
#define DLC_OID_AT_ORGANIZATION             DLC_OID_AT "\x0A" /**< id-at-organizationName AttributeType:= {id-at 10} */
#define DLC_OID_AT_ORG_UNIT                 DLC_OID_AT "\x0B" /**< id-at-organizationalUnitName AttributeType:= {id-at 11} */
#define DLC_OID_AT_TITLE                    DLC_OID_AT "\x0C" /**< id-at-title AttributeType:= {id-at 12} */
#define DLC_OID_AT_POSTAL_ADDRESS           DLC_OID_AT "\x10" /**< id-at-postalAddress AttributeType:= {id-at 16} */
#define DLC_OID_AT_POSTAL_CODE              DLC_OID_AT "\x11" /**< id-at-postalCode AttributeType:= {id-at 17} */
#define DLC_OID_AT_GIVEN_NAME               DLC_OID_AT "\x2A" /**< id-at-givenName AttributeType:= {id-at 42} */
#define DLC_OID_AT_INITIALS                 DLC_OID_AT "\x2B" /**< id-at-initials AttributeType:= {id-at 43} */
#define DLC_OID_AT_GENERATION_QUALIFIER     DLC_OID_AT "\x2C" /**< id-at-generationQualifier AttributeType:= {id-at 44} */
#define DLC_OID_AT_UNIQUE_IDENTIFIER        DLC_OID_AT "\x2D" /**< id-at-uniqueIdentifier AttributType:= {id-at 45} */
#define DLC_OID_AT_DN_QUALIFIER             DLC_OID_AT "\x2E" /**< id-at-dnQualifier AttributeType:= {id-at 46} */
#define DLC_OID_AT_PSEUDONYM                DLC_OID_AT "\x41" /**< id-at-pseudonym AttributeType:= {id-at 65} */

#define DLC_OID_DOMAIN_COMPONENT            "\x09\x92\x26\x89\x93\xF2\x2C\x64\x01\x19" /** id-domainComponent AttributeType:= {itu-t(0) data(9) pss(2342) ucl(19200300) pilot(100) pilotAttributeType(1) domainComponent(25)} */

/*
 * OIDs for standard certificate extensions
 */
#define DLC_OID_AUTHORITY_KEY_IDENTIFIER    DLC_OID_ID_CE "\x23" /**< id-ce-authorityKeyIdentifier OBJECT IDENTIFIER ::=  { id-ce 35 } */
#define DLC_OID_SUBJECT_KEY_IDENTIFIER      DLC_OID_ID_CE "\x0E" /**< id-ce-subjectKeyIdentifier OBJECT IDENTIFIER ::=  { id-ce 14 } */
#define DLC_OID_KEY_USAGE                   DLC_OID_ID_CE "\x0F" /**< id-ce-keyUsage OBJECT IDENTIFIER ::=  { id-ce 15 } */
#define DLC_OID_CERTIFICATE_POLICIES        DLC_OID_ID_CE "\x20" /**< id-ce-certificatePolicies OBJECT IDENTIFIER ::=  { id-ce 32 } */
#define DLC_OID_POLICY_MAPPINGS             DLC_OID_ID_CE "\x21" /**< id-ce-policyMappings OBJECT IDENTIFIER ::=  { id-ce 33 } */
#define DLC_OID_SUBJECT_ALT_NAME            DLC_OID_ID_CE "\x11" /**< id-ce-subjectAltName OBJECT IDENTIFIER ::=  { id-ce 17 } */
#define DLC_OID_ISSUER_ALT_NAME             DLC_OID_ID_CE "\x12" /**< id-ce-issuerAltName OBJECT IDENTIFIER ::=  { id-ce 18 } */
#define DLC_OID_SUBJECT_DIRECTORY_ATTRS     DLC_OID_ID_CE "\x09" /**< id-ce-subjectDirectoryAttributes OBJECT IDENTIFIER ::=  { id-ce 9 } */
#define DLC_OID_BASIC_CONSTRAINTS           DLC_OID_ID_CE "\x13" /**< id-ce-basicConstraints OBJECT IDENTIFIER ::=  { id-ce 19 } */
#define DLC_OID_NAME_CONSTRAINTS            DLC_OID_ID_CE "\x1E" /**< id-ce-nameConstraints OBJECT IDENTIFIER ::=  { id-ce 30 } */
#define DLC_OID_POLICY_CONSTRAINTS          DLC_OID_ID_CE "\x24" /**< id-ce-policyConstraints OBJECT IDENTIFIER ::=  { id-ce 36 } */
#define DLC_OID_EXTENDED_KEY_USAGE          DLC_OID_ID_CE "\x25" /**< id-ce-extKeyUsage OBJECT IDENTIFIER ::= { id-ce 37 } */
#define DLC_OID_CRL_DISTRIBUTION_POINTS     DLC_OID_ID_CE "\x1F" /**< id-ce-cRLDistributionPoints OBJECT IDENTIFIER ::=  { id-ce 31 } */
#define DLC_OID_INIHIBIT_ANYPOLICY          DLC_OID_ID_CE "\x36" /**< id-ce-inhibitAnyPolicy OBJECT IDENTIFIER ::=  { id-ce 54 } */
#define DLC_OID_FRESHEST_CRL                DLC_OID_ID_CE "\x2E" /**< id-ce-freshestCRL OBJECT IDENTIFIER ::=  { id-ce 46 } */

/*
 * Netscape certificate extensions
 */
#define DLC_OID_NS_CERT                 DLC_OID_NETSCAPE "\x01"
#define DLC_OID_NS_CERT_TYPE            DLC_OID_NS_CERT  "\x01"
#define DLC_OID_NS_BASE_URL             DLC_OID_NS_CERT  "\x02"
#define DLC_OID_NS_REVOCATION_URL       DLC_OID_NS_CERT  "\x03"
#define DLC_OID_NS_CA_REVOCATION_URL    DLC_OID_NS_CERT  "\x04"
#define DLC_OID_NS_RENEWAL_URL          DLC_OID_NS_CERT  "\x07"
#define DLC_OID_NS_CA_POLICY_URL        DLC_OID_NS_CERT  "\x08"
#define DLC_OID_NS_SSL_SERVER_NAME      DLC_OID_NS_CERT  "\x0C"
#define DLC_OID_NS_COMMENT              DLC_OID_NS_CERT  "\x0D"
#define DLC_OID_NS_DATA_TYPE            DLC_OID_NETSCAPE "\x02"
#define DLC_OID_NS_CERT_SEQUENCE        DLC_OID_NS_DATA_TYPE "\x05"

/*
 * OIDs for CRL extensions
 */
#define DLC_OID_PRIVATE_KEY_USAGE_PERIOD    DLC_OID_ID_CE "\x10"
#define DLC_OID_CRL_NUMBER                  DLC_OID_ID_CE "\x14" /**< id-ce-cRLNumber OBJECT IDENTIFIER ::= { id-ce 20 } */

/*
 * X.509 v3 Extended key usage OIDs
 */
#define DLC_OID_ANY_EXTENDED_KEY_USAGE      DLC_OID_EXTENDED_KEY_USAGE "\x00" /**< anyExtendedKeyUsage OBJECT IDENTIFIER ::= { id-ce-extKeyUsage 0 } */

#define DLC_OID_KP                          DLC_OID_PKIX "\x03" /**< id-kp OBJECT IDENTIFIER ::= { id-pkix 3 } */
#define DLC_OID_SERVER_AUTH                 DLC_OID_KP "\x01" /**< id-kp-serverAuth OBJECT IDENTIFIER ::= { id-kp 1 } */
#define DLC_OID_CLIENT_AUTH                 DLC_OID_KP "\x02" /**< id-kp-clientAuth OBJECT IDENTIFIER ::= { id-kp 2 } */
#define DLC_OID_CODE_SIGNING                DLC_OID_KP "\x03" /**< id-kp-codeSigning OBJECT IDENTIFIER ::= { id-kp 3 } */
#define DLC_OID_EMAIL_PROTECTION            DLC_OID_KP "\x04" /**< id-kp-emailProtection OBJECT IDENTIFIER ::= { id-kp 4 } */
#define DLC_OID_TIME_STAMPING               DLC_OID_KP "\x08" /**< id-kp-timeStamping OBJECT IDENTIFIER ::= { id-kp 8 } */
#define DLC_OID_OCSP_SIGNING                DLC_OID_KP "\x09" /**< id-kp-OCSPSigning OBJECT IDENTIFIER ::= { id-kp 9 } */

/*
 * PKCS definition OIDs
 */

#define DLC_OID_PKCS                DLC_OID_RSA_COMPANY "\x01" /**< pkcs OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) 1 } */
#define DLC_OID_PKCS1               DLC_OID_PKCS "\x01" /**< pkcs-1 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) pkcs(1) 1 } */
#define DLC_OID_PKCS5               DLC_OID_PKCS "\x05" /**< pkcs-5 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) pkcs(1) 5 } */
#define DLC_OID_PKCS9               DLC_OID_PKCS "\x09" /**< pkcs-9 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) pkcs(1) 9 } */
#define DLC_OID_PKCS12              DLC_OID_PKCS "\x0c" /**< pkcs-12 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) pkcs(1) 12 } */

/*
 * PKCS#1 OIDs
 */
#define DLC_OID_PKCS1_RSA           DLC_OID_PKCS1 "\x01" /**< rsaEncryption OBJECT IDENTIFIER ::= { pkcs-1 1 } */
#define DLC_OID_PKCS1_MD2           DLC_OID_PKCS1 "\x02" /**< md2WithRSAEncryption ::= { pkcs-1 2 } */
#define DLC_OID_PKCS1_MD4           DLC_OID_PKCS1 "\x03" /**< md4WithRSAEncryption ::= { pkcs-1 3 } */
#define DLC_OID_PKCS1_MD5           DLC_OID_PKCS1 "\x04" /**< md5WithRSAEncryption ::= { pkcs-1 4 } */
#define DLC_OID_PKCS1_SHA1          DLC_OID_PKCS1 "\x05" /**< sha1WithRSAEncryption ::= { pkcs-1 5 } */
#define DLC_OID_PKCS1_SHA224        DLC_OID_PKCS1 "\x0e" /**< sha224WithRSAEncryption ::= { pkcs-1 14 } */
#define DLC_OID_PKCS1_SHA256        DLC_OID_PKCS1 "\x0b" /**< sha256WithRSAEncryption ::= { pkcs-1 11 } */
#define DLC_OID_PKCS1_SHA384        DLC_OID_PKCS1 "\x0c" /**< sha384WithRSAEncryption ::= { pkcs-1 12 } */
#define DLC_OID_PKCS1_SHA512        DLC_OID_PKCS1 "\x0d" /**< sha512WithRSAEncryption ::= { pkcs-1 13 } */

#define DLC_OID_RSA_SHA_OBS         "\x2B\x0E\x03\x02\x1D"

#define DLC_OID_PKCS9_EMAIL         DLC_OID_PKCS9 "\x01" /**< emailAddress AttributeType ::= { pkcs-9 1 } */

/* RFC 4055 */
#define DLC_OID_RSASSA_PSS          DLC_OID_PKCS1 "\x0a" /**< id-RSASSA-PSS ::= { pkcs-1 10 } */
#define DLC_OID_MGF1                DLC_OID_PKCS1 "\x08" /**< id-mgf1 ::= { pkcs-1 8 } */

/*
 * Digest algorithms
 */
#define DLC_OID_DIGEST_ALG_MD2              DLC_OID_RSA_COMPANY "\x02\x02" /**< id-dlc_md2 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) digestAlgorithm(2) 2 } */
#define DLC_OID_DIGEST_ALG_MD4              DLC_OID_RSA_COMPANY "\x02\x04" /**< id-dlc_md4 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) digestAlgorithm(2) 4 } */
#define DLC_OID_DIGEST_ALG_MD5              DLC_OID_RSA_COMPANY "\x02\x05" /**< id-dlc_md5 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) digestAlgorithm(2) 5 } */
#define DLC_OID_DIGEST_ALG_SHA1             DLC_OID_ISO_IDENTIFIED_ORG DLC_OID_OIW_SECSIG_SHA1 /**< id-dlc_sha1 OBJECT IDENTIFIER ::= { iso(1) identified-organization(3) oiw(14) secsig(3) algorithms(2) 26 } */
#define DLC_OID_DIGEST_ALG_SHA224           DLC_OID_NIST_ALG "\x02\x04" /**< id-sha224 OBJECT IDENTIFIER ::= { joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101) csor(3) nistalgorithm(4) hashalgs(2) 4 } */
#define DLC_OID_DIGEST_ALG_SHA256           DLC_OID_NIST_ALG "\x02\x01" /**< id-dlc_sha256 OBJECT IDENTIFIER ::= { joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101) csor(3) nistalgorithm(4) hashalgs(2) 1 } */

#define DLC_OID_DIGEST_ALG_SHA384           DLC_OID_NIST_ALG "\x02\x02" /**< id-sha384 OBJECT IDENTIFIER ::= { joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101) csor(3) nistalgorithm(4) hashalgs(2) 2 } */

#define DLC_OID_DIGEST_ALG_SHA512           DLC_OID_NIST_ALG "\x02\x03" /**< id-dlc_sha512 OBJECT IDENTIFIER ::= { joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101) csor(3) nistalgorithm(4) hashalgs(2) 3 } */

#define DLC_OID_HMAC_SHA1                   DLC_OID_RSA_COMPANY "\x02\x07" /**< id-hmacWithSHA1 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) digestAlgorithm(2) 7 } */

#define DLC_OID_HMAC_SHA224                 DLC_OID_RSA_COMPANY "\x02\x08" /**< id-hmacWithSHA224 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) digestAlgorithm(2) 8 } */

#define DLC_OID_HMAC_SHA256                 DLC_OID_RSA_COMPANY "\x02\x09" /**< id-hmacWithSHA256 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) digestAlgorithm(2) 9 } */

#define DLC_OID_HMAC_SHA384                 DLC_OID_RSA_COMPANY "\x02\x0A" /**< id-hmacWithSHA384 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) digestAlgorithm(2) 10 } */

#define DLC_OID_HMAC_SHA512                 DLC_OID_RSA_COMPANY "\x02\x0B" /**< id-hmacWithSHA512 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) rsadsi(113549) digestAlgorithm(2) 11 } */

/*
 * Encryption algorithms
 */
#define DLC_OID_DES_CBC                     DLC_OID_ISO_IDENTIFIED_ORG DLC_OID_OIW_SECSIG_ALG "\x07" /**< desCBC OBJECT IDENTIFIER ::= { iso(1) identified-organization(3) oiw(14) secsig(3) algorithms(2) 7 } */
#define DLC_OID_DES_EDE3_CBC                DLC_OID_RSA_COMPANY "\x03\x07" /**< des-ede3-cbc OBJECT IDENTIFIER ::= { iso(1) member-body(2) -- us(840) rsadsi(113549) encryptionAlgorithm(3) 7 } */
#define DLC_OID_AES                         DLC_OID_NIST_ALG "\x01" /** aes OBJECT IDENTIFIER ::= { joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101) csor(3) nistAlgorithm(4) 1 } */

/*
 * Key Wrapping algorithms
 */
/*
 * RFC 5649
 */
#define DLC_OID_AES128_KW                   DLC_OID_AES "\x05" /** id-aes128-wrap     OBJECT IDENTIFIER ::= { aes 5 } */
#define DLC_OID_AES128_KWP                  DLC_OID_AES "\x08" /** id-aes128-wrap-pad OBJECT IDENTIFIER ::= { aes 8 } */
#define DLC_OID_AES192_KW                   DLC_OID_AES "\x19" /** id-aes192-wrap     OBJECT IDENTIFIER ::= { aes 25 } */
#define DLC_OID_AES192_KWP                  DLC_OID_AES "\x1c" /** id-aes192-wrap-pad OBJECT IDENTIFIER ::= { aes 28 } */
#define DLC_OID_AES256_KW                   DLC_OID_AES "\x2d" /** id-aes256-wrap     OBJECT IDENTIFIER ::= { aes 45 } */
#define DLC_OID_AES256_KWP                  DLC_OID_AES "\x30" /** id-aes256-wrap-pad OBJECT IDENTIFIER ::= { aes 48 } */
/*
 * PKCS#5 OIDs
 */
#define DLC_OID_PKCS5_PBKDF2                DLC_OID_PKCS5 "\x0c" /**< id-PBKDF2 OBJECT IDENTIFIER ::= {pkcs-5 12} */
#define DLC_OID_PKCS5_PBES2                 DLC_OID_PKCS5 "\x0d" /**< id-PBES2 OBJECT IDENTIFIER ::= {pkcs-5 13} */
#define DLC_OID_PKCS5_PBMAC1                DLC_OID_PKCS5 "\x0e" /**< id-PBMAC1 OBJECT IDENTIFIER ::= {pkcs-5 14} */

/*
 * PKCS#5 PBES1 algorithms
 */
#define DLC_OID_PKCS5_PBE_MD2_DES_CBC       DLC_OID_PKCS5 "\x01" /**< pbeWithMD2AndDES-CBC OBJECT IDENTIFIER ::= {pkcs-5 1} */
#define DLC_OID_PKCS5_PBE_MD2_RC2_CBC       DLC_OID_PKCS5 "\x04" /**< pbeWithMD2AndRC2-CBC OBJECT IDENTIFIER ::= {pkcs-5 4} */
#define DLC_OID_PKCS5_PBE_MD5_DES_CBC       DLC_OID_PKCS5 "\x03" /**< pbeWithMD5AndDES-CBC OBJECT IDENTIFIER ::= {pkcs-5 3} */
#define DLC_OID_PKCS5_PBE_MD5_RC2_CBC       DLC_OID_PKCS5 "\x06" /**< pbeWithMD5AndRC2-CBC OBJECT IDENTIFIER ::= {pkcs-5 6} */
#define DLC_OID_PKCS5_PBE_SHA1_DES_CBC      DLC_OID_PKCS5 "\x0a" /**< pbeWithSHA1AndDES-CBC OBJECT IDENTIFIER ::= {pkcs-5 10} */
#define DLC_OID_PKCS5_PBE_SHA1_RC2_CBC      DLC_OID_PKCS5 "\x0b" /**< pbeWithSHA1AndRC2-CBC OBJECT IDENTIFIER ::= {pkcs-5 11} */

/*
 * PKCS#8 OIDs
 */
#define DLC_OID_PKCS9_CSR_EXT_REQ           DLC_OID_PKCS9 "\x0e" /**< extensionRequest OBJECT IDENTIFIER ::= {pkcs-9 14} */

/*
 * PKCS#12 PBE OIDs
 */
#define DLC_OID_PKCS12_PBE                      DLC_OID_PKCS12 "\x01" /**< pkcs-12PbeIds OBJECT IDENTIFIER ::= {pkcs-12 1} */

#define DLC_OID_PKCS12_PBE_SHA1_RC4_128         DLC_OID_PKCS12_PBE "\x01" /**< pbeWithSHAAnd128BitRC4 OBJECT IDENTIFIER ::= {pkcs-12PbeIds 1} */
#define DLC_OID_PKCS12_PBE_SHA1_RC4_40          DLC_OID_PKCS12_PBE "\x02" /**< pbeWithSHAAnd40BitRC4 OBJECT IDENTIFIER ::= {pkcs-12PbeIds 2} */
#define DLC_OID_PKCS12_PBE_SHA1_DES3_EDE_CBC    DLC_OID_PKCS12_PBE "\x03" /**< pbeWithSHAAnd3-KeyTripleDES-CBC OBJECT IDENTIFIER ::= {pkcs-12PbeIds 3} */
#define DLC_OID_PKCS12_PBE_SHA1_DES2_EDE_CBC    DLC_OID_PKCS12_PBE "\x04" /**< pbeWithSHAAnd2-KeyTripleDES-CBC OBJECT IDENTIFIER ::= {pkcs-12PbeIds 4} */
#define DLC_OID_PKCS12_PBE_SHA1_RC2_128_CBC     DLC_OID_PKCS12_PBE "\x05" /**< pbeWithSHAAnd128BitRC2-CBC OBJECT IDENTIFIER ::= {pkcs-12PbeIds 5} */
#define DLC_OID_PKCS12_PBE_SHA1_RC2_40_CBC      DLC_OID_PKCS12_PBE "\x06" /**< pbeWithSHAAnd40BitRC2-CBC OBJECT IDENTIFIER ::= {pkcs-12PbeIds 6} */

/*
 * EC key algorithms from RFC 5480
 */

/* id-ecPublicKey OBJECT IDENTIFIER ::= {
 *       iso(1) member-body(2) us(840) ansi-X9-62(10045) keyType(2) 1 } */
#define DLC_OID_EC_ALG_UNRESTRICTED         DLC_OID_ANSI_X9_62 "\x02\01"

/*   id-ecDH OBJECT IDENTIFIER ::= {
 *     iso(1) identified-organization(3) certicom(132)
 *     schemes(1) ecdh(12) } */
#define DLC_OID_EC_ALG_ECDH                 DLC_OID_CERTICOM "\x01\x0c"

/*
 * ECParameters namedCurve identifiers, from RFC 5480, RFC 5639, and SEC2
 */

/* secp192r1 OBJECT IDENTIFIER ::= {
 *   iso(1) member-body(2) us(840) ansi-X9-62(10045) curves(3) prime(1) 1 } */
#define DLC_OID_EC_GRP_SECP192R1        DLC_OID_ANSI_X9_62 "\x03\x01\x01"

/* secp224r1 OBJECT IDENTIFIER ::= {
 *   iso(1) identified-organization(3) certicom(132) curve(0) 33 } */
#define DLC_OID_EC_GRP_SECP224R1        DLC_OID_CERTICOM "\x00\x21"

/* secp256r1 OBJECT IDENTIFIER ::= {
 *   iso(1) member-body(2) us(840) ansi-X9-62(10045) curves(3) prime(1) 7 } */
#define DLC_OID_EC_GRP_SECP256R1        DLC_OID_ANSI_X9_62 "\x03\x01\x07"

/* secp384r1 OBJECT IDENTIFIER ::= {
 *   iso(1) identified-organization(3) certicom(132) curve(0) 34 } */
#define DLC_OID_EC_GRP_SECP384R1        DLC_OID_CERTICOM "\x00\x22"

/* secp521r1 OBJECT IDENTIFIER ::= {
 *   iso(1) identified-organization(3) certicom(132) curve(0) 35 } */
#define DLC_OID_EC_GRP_SECP521R1        DLC_OID_CERTICOM "\x00\x23"

/* secp192k1 OBJECT IDENTIFIER ::= {
 *   iso(1) identified-organization(3) certicom(132) curve(0) 31 } */
#define DLC_OID_EC_GRP_SECP192K1        DLC_OID_CERTICOM "\x00\x1f"

/* secp224k1 OBJECT IDENTIFIER ::= {
 *   iso(1) identified-organization(3) certicom(132) curve(0) 32 } */
#define DLC_OID_EC_GRP_SECP224K1        DLC_OID_CERTICOM "\x00\x20"

/* secp256k1 OBJECT IDENTIFIER ::= {
 *   iso(1) identified-organization(3) certicom(132) curve(0) 10 } */
#define DLC_OID_EC_GRP_SECP256K1        DLC_OID_CERTICOM "\x00\x0a"

/* RFC 5639 4.1
 * ecStdCurvesAndGeneration OBJECT IDENTIFIER::= {iso(1)
 * identified-organization(3) teletrust(36) algorithm(3) signature-
 * algorithm(3) ecSign(2) 8}
 * ellipticCurve OBJECT IDENTIFIER ::= {ecStdCurvesAndGeneration 1}
 * versionOne OBJECT IDENTIFIER ::= {ellipticCurve 1} */
#define DLC_OID_EC_BRAINPOOL_V1         DLC_OID_TELETRUST "\x03\x03\x02\x08\x01\x01"

/* brainpoolP256r1 OBJECT IDENTIFIER ::= {versionOne 7} */
#define DLC_OID_EC_GRP_BP256R1          DLC_OID_EC_BRAINPOOL_V1 "\x07"

/* brainpoolP384r1 OBJECT IDENTIFIER ::= {versionOne 11} */
#define DLC_OID_EC_GRP_BP384R1          DLC_OID_EC_BRAINPOOL_V1 "\x0B"

/* brainpoolP512r1 OBJECT IDENTIFIER ::= {versionOne 13} */
#define DLC_OID_EC_GRP_BP512R1          DLC_OID_EC_BRAINPOOL_V1 "\x0D"

/*
 * SEC1 C.1
 *
 * prime-field OBJECT IDENTIFIER ::= { id-fieldType 1 }
 * id-fieldType OBJECT IDENTIFIER ::= { ansi-X9-62 fieldType(1)}
 */
#define DLC_OID_ANSI_X9_62_FIELD_TYPE   DLC_OID_ANSI_X9_62 "\x01"
#define DLC_OID_ANSI_X9_62_PRIME_FIELD  DLC_OID_ANSI_X9_62_FIELD_TYPE "\x01"

/*
 * ECDSA signature identifiers, from RFC 5480
 */
#define DLC_OID_ANSI_X9_62_SIG          DLC_OID_ANSI_X9_62 "\x04" /* signatures(4) */
#define DLC_OID_ANSI_X9_62_SIG_SHA2     DLC_OID_ANSI_X9_62_SIG "\x03" /* ecdsa-with-SHA2(3) */

/* ecdsa-with-SHA1 OBJECT IDENTIFIER ::= {
 *   iso(1) member-body(2) us(840) ansi-X9-62(10045) signatures(4) 1 } */
#define DLC_OID_ECDSA_SHA1              DLC_OID_ANSI_X9_62_SIG "\x01"

/* ecdsa-with-SHA224 OBJECT IDENTIFIER ::= {
 *   iso(1) member-body(2) us(840) ansi-X9-62(10045) signatures(4)
 *   ecdsa-with-SHA2(3) 1 } */
#define DLC_OID_ECDSA_SHA224            DLC_OID_ANSI_X9_62_SIG_SHA2 "\x01"

/* ecdsa-with-SHA256 OBJECT IDENTIFIER ::= {
 *   iso(1) member-body(2) us(840) ansi-X9-62(10045) signatures(4)
 *   ecdsa-with-SHA2(3) 2 } */
#define DLC_OID_ECDSA_SHA256            DLC_OID_ANSI_X9_62_SIG_SHA2 "\x02"

/* ecdsa-with-SHA384 OBJECT IDENTIFIER ::= {
 *   iso(1) member-body(2) us(840) ansi-X9-62(10045) signatures(4)
 *   ecdsa-with-SHA2(3) 3 } */
#define DLC_OID_ECDSA_SHA384            DLC_OID_ANSI_X9_62_SIG_SHA2 "\x03"

/* ecdsa-with-SHA512 OBJECT IDENTIFIER ::= {
 *   iso(1) member-body(2) us(840) ansi-X9-62(10045) signatures(4)
 *   ecdsa-with-SHA2(3) 4 } */
#define DLC_OID_ECDSA_SHA512            DLC_OID_ANSI_X9_62_SIG_SHA2 "\x04"






/**
 * \brief Base OID descriptor structure
 */
typedef struct dlc_oid_descriptor_t
{
    const char *asn1;               /*!< OID ASN.1 representation       */
    size_t asn1_len;                /*!< length of asn1                 */
    const char *name;               /*!< official name (e.g. from RFC)  */
    const char *description;        /*!< human friendly description     */
} dlc_oid_descriptor_t;

/**
 * \brief           Translate an ASN.1 OID into its numeric representation
 *                  (e.g. "\x2A\x86\x48\x86\xF7\x0D" into "1.2.840.113549")
 *
 * \param buf       buffer to put representation in
 * \param size      size of the buffer
 * \param oid       OID to translate
 *
 * \return          Length of the string written (excluding final NULL) or
 *                  DLC_ERR_OID_BUF_TOO_SMALL in case of error
 */
int dlc_oid_get_numeric_string( char *buf, size_t size, const dlc_asn1_buf *oid );

/**
 * \brief          Translate an X.509 extension OID into local values
 *
 * \param oid      OID to use
 * \param ext_type place to store the extension type
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_x509_ext_type( const dlc_asn1_buf *oid, int *ext_type );

/**
 * \brief          Translate an X.509 attribute type OID into the short name
 *                 (e.g. the OID for an X520 Common Name into "CN")
 *
 * \param oid      OID to use
 * \param short_name    place to store the string pointer
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_attr_short_name( const dlc_asn1_buf *oid, const char **short_name );

/**
 * \brief          Translate PublicKeyAlgorithm OID into pk_type
 *
 * \param oid      OID to use
 * \param pk_alg   place to store public key algorithm
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_pk_alg( const dlc_asn1_buf *oid, dlc_pk_type *pk_alg );

/**
 * \brief          Translate pk_type into PublicKeyAlgorithm OID
 *
 * \param pk_alg   Public key type to look for
 * \param oid      place to store ASN.1 OID string pointer
 * \param olen     length of the OID
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_oid_by_pk_alg( dlc_pk_type pk_alg,
                           const char **oid, size_t *olen );






/**
 * \brief          Translate SignatureAlgorithm OID into md_type and pk_type
 *
 * \param oid      OID to use
 * \param md_alg   place to store message digest algorithm
 * \param pk_alg   place to store public key algorithm
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_sig_alg( const dlc_asn1_buf *oid,
                     dlc_md_type *md_alg, dlc_pk_type *pk_alg );

/**
 * \brief          Translate SignatureAlgorithm OID into description
 *
 * \param oid      OID to use
 * \param desc     place to store string pointer
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_sig_alg_desc( const dlc_asn1_buf *oid, const char **desc );

/**
 * \brief          Translate md_type and pk_type into SignatureAlgorithm OID
 *
 * \param md_alg   message digest algorithm
 * \param pk_alg   public key algorithm
 * \param oid      place to store ASN.1 OID string pointer
 * \param olen     length of the OID
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_oid_by_sig_alg( dlc_pk_type pk_alg, dlc_md_type md_alg,
                            const char **oid, size_t *olen );

/**
 * \brief          Translate hash algorithm OID into md_type
 *
 * \param oid      OID to use
 * \param md_alg   place to store message digest algorithm
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_md_alg( const dlc_asn1_buf *oid, dlc_md_type *md_alg );

/**
 * \brief          Translate hmac algorithm OID into md_type
 *
 * \param oid      OID to use
 * \param md_hmac  place to store message hmac algorithm
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_md_hmac( const dlc_asn1_buf *oid, dlc_md_type *md_hmac );

/**
 * \brief          Translate Extended Key Usage OID into description
 *
 * \param oid      OID to use
 * \param desc     place to store string pointer
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_extended_key_usage( const dlc_asn1_buf *oid, const char **desc );

/**
 * \brief          Translate md_type into hash algorithm OID
 *
 * \param md_alg   message digest algorithm
 * \param oid      place to store ASN.1 OID string pointer
 * \param olen     length of the OID
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_oid_by_md( dlc_md_type md_alg, const char **oid, size_t *olen );


/**
 * \brief          Translate encryption algorithm OID into cipher_type
 *
 * \param oid           OID to use
 * \param cipher_alg    place to store cipher algorithm
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_cipher_alg( const dlc_asn1_buf *oid, dlc_cipher_type_t *cipher_alg );


/**
 * \brief          Translate PKCS#12 PBE algorithm OID into md_type and
 *                 cipher_type
 *
 * \param oid           OID to use
 * \param md_alg        place to store message digest algorithm
 * \param cipher_alg    place to store cipher algorithm
 *
 * \return         0 if successful, or DLC_ERR_OID_NOT_FOUND
 */
int dlc_oid_get_pkcs12_pbe_alg( const dlc_asn1_buf *oid, dlc_md_type *md_alg,
                            dlc_cipher_type_t *cipher_alg );









#endif