#ifndef __DL_sha1
#define __DL_sha1

#include <stdint.h>

enum
{
    shaSuccess = 0,
    shaNull,            /* Null pointer parameter */
    shaInputTooLong,    /* input data too long */
    shaStateError       /* called Input after Result */
};

#define sha1HashSize 20


typedef struct sha1Context
{
    uint32_t Intermediate_Hash[sha1HashSize/4]; /* Message Digest  */

    uint32_t Length_Low;            /* Message length in bits      */
    uint32_t Length_High;           /* Message length in bits      */

                               /* Index into message block array   */
    int_least16_t Message_Block_Index;
    uint8_t Message_Block[64];      /* 512-bit message blocks      */

    int Computed;               /* Is the digest computed?         */
    int Corrupted;             /* Is the message digest corrupted? */
} sha1_ctx;

/*
 *  Function Prototypes
 */

int sha1Reset(sha1_ctx *);
int sha1Input(sha1_ctx *,const uint8_t *,unsigned int);
int sha1Result(sha1_ctx *,uint8_t Message_Digest[sha1HashSize]);

#endif