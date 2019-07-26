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
#include <sys/time.h>

#include <stddef.h>     //for offsetof
#include <fcntl.h>      //for open

#include <sys/mman.h>
#include <sys/stat.h>

#include <netinet/in.h>

#define DL_INT8_LEN   (sizeof("-128") - 1)
#define DL_INT16_LEN   (sizeof("-32768") - 1)
#define DL_INT32_LEN   (sizeof("-2147483648") - 1)
#define DL_INT64_LEN   (sizeof("-9223372036854775808") - 1)

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;

typedef uintptr_t       dl_uint;
typedef intptr_t        dl_int;

#define LF     '\n'
#define CR     '\r'
#define CRLF   "\r\n"

#define dl_abs(value)       (((value) >= 0) ? (value) : - (value))
#define dl_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))
#define dl_max(a,b) (a > b ? a : b);

#define DL_CUP_CACHE_LINE 64

#define dl_barrier() __asm__ __volatile__("": : :"memory")

#endif
