#ifndef __DL_CONFIG
#define __DL_CONFIG

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>     //for uintptr_t

#include <time.h>

#include <stddef.h>     //for offsetof
#include <fcntl.h>      //for open

#include <sys/mman.h>
#include <sys/stat.h>

#define DL_INT8_LEN   (sizeof("-127") - 1)
#define DL_INT16_LEN   (sizeof("-32768") - 1)
#define DL_INT32_LEN   (sizeof("-2147483648") - 1)
#define DL_INT64_LEN   (sizeof("-9223372036854775808") - 1)

#define DL_INT8_MAX 127
#define DL_INT16_MAX 32767
#define DL_INT32_MAX 2147483647
#define DL_INT64_MAX 9223372036854775807


#define DL_UINT8_MAX 256
#define DL_UINT16_MAX 65535
#define DL_UINT32_MAX 4294967295
#define DL_UINT64_MAX 18446744073709551615

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;

#define dl_max(a,b) (a > b ? a : b);

#define dl_offset_data(q, type, link)               \
    (type *) ((char *) q - offsetof(type, link))

#define DL_CUP_CACHE_LINE 64

#endif
