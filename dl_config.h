#ifndef __DL_CONFIG
#define __DL_CONFIG

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>     //for uintptr_t

#include <stddef.h>     //for offsetof

#define DL_INT8_LEN   (sizeof("-127") - 1)
#define DL_INT16_LEN   (sizeof("-32768") - 1)
#define DL_INT32_LEN   (sizeof("-2147483648") - 1)
#define DL_INT64_LEN   (sizeof("-9223372036854775808") - 1)

#define DL_UINT8_MAX 256
#define DL_UINT16_MAX 65535
#define DL_UINT32_MAX 4294967295
#define DL_UINT64_MAX 18446744073709551615

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;


#endif
