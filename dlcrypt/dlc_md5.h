#ifndef __DL_md5
#define __DL_md5
#include <stdint.h>
#include <string.h>

/* md5 context. */
typedef struct {
  uint32_t  state[4];                                   /* state (ABCD) */
  uint32_t  count[2];        /* number of bits, modulo 2^64 (lsb first) */
  uint8_t   buffer[64];                         /* 512 bits buffer */
} md5_ctx;

void md5Init (md5_ctx *);

void md5Update (md5_ctx *, uint8_t *, uint32_t);

void md5Final (uint8_t [16], md5_ctx *);


#endif