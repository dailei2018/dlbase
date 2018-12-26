#include "dl_base.h"


in_addr_t
dl_inet_addr(uchar *text, size_t len)
{
    uchar       *p, c;
    in_addr_t    addr;
    uint         octet, n;

    addr = 0;
    octet = 0;
    n = 0;

    for (p = text; p < text + len; p++) {
        c = *p;

        if (c >= '0' && c <= '9') {
            octet = octet * 10 + (c - '0');

            if (octet > 255) {
                return INADDR_NONE;
            }

            continue;
        }

        if (c == '.') {
            addr = (addr << 8) + octet;
            octet = 0;
            n++;
            continue;
        }

        return INADDR_NONE;
    }

    if (n == 3) {
        addr = (addr << 8) + octet;
        return htonl(addr);
    }

    return INADDR_NONE;
}


int
dl_ptocidr(dl_str *text, dl_cidr *cidr)
{
    uchar      *addr, *mask, *last;
    size_t      len;
    int         shift;

    addr = text->data;
    last = addr + text->len;

    mask = (uchar *)dl_strnchr(addr, text->len, '/');
    len = (mask ? mask : last) - addr;

    cidr->u.in.addr = dl_inet_addr(addr, len);

    if (cidr->u.in.addr != INADDR_NONE) {
        cidr->family = AF_INET;

        if (mask == NULL) {
            cidr->u.in.mask = 0xffffffff;
            return DL_OK;
        }

    } else {
        return DL_ERROR;
    }

    mask++;

    shift = dl_atoi(mask, last - mask);
    if (shift == DL_ERROR) {
        return DL_ERROR;
    }

    switch (cidr->family) {

    default: /* AF_INET */
        if (shift > 32) {
            return DL_ERROR;
        }

        if (shift) {
            cidr->u.in.mask = htonl((uint32_t) (0xffffffffu << (32 - shift)));

        } else {
            /* x86 compilers use a shl instruction that shifts by modulo 32 */
            cidr->u.in.mask = 0;
        }

        if (cidr->u.in.addr == (cidr->u.in.addr & cidr->u.in.mask)) {
            return DL_OK;
        }

        cidr->u.in.addr &= cidr->u.in.mask;

        return DL_DONE;
    }
}