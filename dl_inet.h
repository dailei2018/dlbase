#ifndef __DL_INET
#define __DL_INET

#include "dl_base.h"

typedef struct {
    in_addr_t                 addr;
    in_addr_t                 mask;
} dl_in_cidr;

typedef struct {
    struct in6_addr           addr;
    struct in6_addr           mask;
} dl_in6_cidr;

typedef struct {
    uint                family;
    union {
        dl_in_cidr         in;
        dl_in6_cidr        in6;
    } u;
} dl_cidr;


in_addr_t dl_inet_addr(uchar *text, size_t len);
int dl_ptocidr(dl_str *text, dl_cidr *cidr);


#endif