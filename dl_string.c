#include "dl_base.h"

void
dl_strlow(char *dst, char *src, size_t n)
{
    while (n) {
        *dst = dl_tolower(*src);
        dst++;
        src++;
        n--;
    }
}

static char *
dl_sprintf_num(char *buf, char *last, uint64_t ui64, char zero,
               uint hexadecimal, uint width);

dl_array * dl_split_by_c(dl_pool *p, dl_str *str, char c)
{
    dl_array    *arr;
    char *begin,*end,*cur;
    int len;

    begin = str->data;
    end = str->data + str->len;
    len = str->len;

    dl_str *el;

    arr = dl_array_create(p, 1, sizeof(dl_str));
    if(arr == NULL) return NULL;

    for(;;){
        cur = dl_strnchr(begin, len, c);
        if(cur == NULL) cur = end;

        el = dl_array_push(arr);
        if(el == NULL) return NULL;
        
        el->len = cur - begin;
        el->data = begin;

        if(cur == end) break;

        begin = cur + 1;
        
        len = end - begin;
    }

    return arr;
}

char *dl_strnchr(char *str, int len, char c){
    char *cur;

    cur = str;

    while(len--){
        if(*cur == c) return cur;

        cur++;
    }

    return NULL;
}


char *
dl_pstrdup(dl_pool *pool, char *data, size_t len)
{
    char  *dst;

    dst = dl_pnalloc(pool, len);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, data, len);

    return dst;
}

char *
dl_pstrdup_nt(dl_pool *pool, char *data, size_t len)
{
    char  *dst;

    dst = dl_pnalloc(pool, len + 1);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, data, len);
    dst[len] = '\0';

    return dst;
}

char *
dl_strdup_nt(char *data, size_t len)
{
    char  *dst;

    dst = dl_alloc(len + 1, NULL);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, data, len);
    dst[len] = '\0';

    return dst;
}

int
dl_atoi(char *line, size_t n)
{
    int  value, cutoff, cutlim;

    if (n == 0) {
        return DL_ERROR;
    }

    cutoff = DL_INT32_MAX / 10;
    cutlim = DL_INT32_MAX % 10;

    for (value = 0; n--; line++) {
        if (*line < '0' || *line > '9') {
            return DL_ERROR;
        }

        /* check whether exceed the maximum value of int */
        if (value >= cutoff && (value > cutoff || *line - '0' > cutlim)) {
            return DL_ERROR;
        }

        value = value * 10 + (*line - '0');
    }

    return value;
}


/*
    substring
*/

char *
dl_strnstr(char *s1, size_t len,  char *s2, size_t len_1)
{
    if(len_1 == 0){
        return NULL;
    }
    
    char  c1, c2;
    size_t  n;

    c2 = *s2++;

    n = len_1 - 1;

    do {
        do {
            if (len-- == 0) {
                return NULL;
            }

            c1 = *s1++;

            if (c1 == 0) {
                return NULL;
            }

        } while (c1 != c2);

        if (n > len) {
            return NULL;
        }

    } while (memcmp(s1, s2, n) != 0);

    return --s1;
}

int
dl_substr_num(char *s1, size_t len,  char *s2, size_t len_1)
{
    int     n = 0;
    char    *res;
    
    while((res = dl_strnstr(s1, len, s2, len_1)) != NULL){
        n++;
        
        len -= res - s1 + len_1;
        s1 = res + len_1;
        
        if(len < len_1) break;
    }
    
    return n;
}

dl_array * dl_split_by_s(dl_pool *p, char *s1, size_t len, char *s2, size_t len_1)
{
    dl_array *arr;

    char    *res;
    dl_str  *str;

    arr = dl_array_create(p, 1, sizeof(dl_str));
    if(arr == NULL) return NULL;

    while((res = dl_strnstr(s1, len, s2, len_1)) != NULL){
        str = dl_array_push(arr);
        if(str == NULL) return NULL;
        
        str->data = s1;
        str->len = res - s1;
        
        len -= res - s1 + len_1;
        s1 = res + len_1;
        
    }
    
    str = dl_array_push(arr);
    str->data = s1;
    str->len = len;

    return arr;
}

dl_list * dl_split_by_s_l(dl_pool *p, char *s1, size_t len, char *s2, size_t len_1)
{
    dl_list *list;

    char    *res;
    dl_str  *str;

    list = dl_list_create(p, 1, sizeof(dl_str));
    if(list == NULL) return NULL;

    while((res = dl_strnstr(s1, len, s2, len_1)) != NULL){
        str = dl_list_push(list);
        if(str == NULL) return NULL;
        
        str->data = s1;
        str->len = res - s1;
        
        len -= res - s1 + len_1;
        s1 = res + len_1;
        
    }
    
    str = dl_list_push(list);
    str->data = s1;
    str->len = len;

    return list;
}

/*
    string replace
*/

dl_str *
dl_str_replace(dl_pool *p, char *s1, size_t len, char *s2, size_t len_2, char *s3, size_t len_3){
    dl_pool     *tmp_p;
    dl_str      *str, *res_s;
    char        *res, *cur;
    dl_list     *list;
    dl_list_part    *part;
    
    int         total = 0;
    
    tmp_p = dl_create_pool(1024, NULL);
    if(tmp_p == NULL) return NULL;
    
    list = dl_list_create(tmp_p, 1, sizeof(dl_str));
    if(list == NULL) return NULL;
    
    while((res = dl_strnstr(s1, len, s2, len_2)) != NULL){
        str = dl_list_push(list);
        str->data = s1;
        str->len = res - s1;
        
        len -= res - s1 + len_2;
        s1 = res + len_2;
        
        total += str->len + len_3;
    }
    
    str = dl_list_push(list);
    str->data = s1;
    str->len = len;
    
    total += str->len;
    
    res_s = dl_palloc(p, sizeof(dl_str));
    res_s->data = dl_palloc(p, total);
    res_s->len = total;
    cur = res_s->data;
    
    for(part = &list->part; part; part = part->next){
        str = part->elts;
        
        cur = dl_cpymem(cur, str->data, str->len);
        
        if(part != list->last){
            cur = dl_cpymem(cur, s3, len_3);
        }
    }
    
    dl_destroy_pool(tmp_p);
    
    return res_s;
}

/*
    printf
*/

void dl_exit_msg(int err, const char *fmt, ...){
    va_list     args;
    
    va_start(args, fmt);
    dl_printf_core(fmt, args);
    
    exit(err);
}

void dl_printf(const char *fmt, ...)
{
    va_list     args;
    
    va_start(args, fmt);
    dl_printf_core(fmt, args);
}

void dl_printf_core(const char *fmt, va_list args){
    char        *p;
    char        buf[1024];
    
    p = dl_vslprintf(buf, buf+1023, fmt, args);
    va_end(args);
    
    *p = '\0';
    
    printf("%s", buf);
}

char *
dl_sprintf(char *buf, const char *fmt, ...)
{
    char   *p;
    va_list   args;

    va_start(args, fmt);
    p = dl_vslprintf(buf, (void *) -1, fmt, args);
    va_end(args);

    return p;
}

char *
dl_snprintf(char *buf, size_t max, const char *fmt, ...)
{
    char   *p;
    va_list   args;

    va_start(args, fmt);
    p = dl_vslprintf(buf, buf + max, fmt, args);
    va_end(args);

    return p;
}

char *
dl_slprintf(char *buf, char *last, const char *fmt, ...)
{
    char   *p;
    va_list   args;

    va_start(args, fmt);
    p = dl_vslprintf(buf, last, fmt, args);
    va_end(args);

    return p;
}

char *
dl_vslprintf(char *buf, char *last, const char *fmt, va_list args)
{
    char                    *p, zero;
    int                     d;
    double                  f;
    size_t                  len, slen;
    int64_t                 i64;
    uint64_t                ui64, frac;
    uint                    width, sign, hex, max_width, frac_width, scale, n;
    dl_str                  *v;

    while (*fmt && buf < last) {

        /*
         * "buf < last" means that we could copy at least one character:
         * the plain character, "%%", "%c", and minus without the checking
         */

        if (*fmt == '%') {

            i64 = 0;
            ui64 = 0;

            zero = (char) ((*++fmt == '0') ? '0' : ' ');
            width = 0;
            sign = 1;
            hex = 0;
            max_width = 0;
            frac_width = 0;
            slen = (size_t) -1;

            while (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + *fmt++ - '0';
            }


            for ( ;; ) {
                switch (*fmt) {

                case 'u':
                    sign = 0;
                    fmt++;
                    continue;

                case 'm':
                    max_width = 1;
                    fmt++;
                    continue;

                case 'X':
                    hex = 2;
                    sign = 0;
                    fmt++;
                    continue;

                case 'x':
                    hex = 1;
                    sign = 0;
                    fmt++;
                    continue;

                case '.':
                    fmt++;

                    while (*fmt >= '0' && *fmt <= '9') {
                        frac_width = frac_width * 10 + *fmt++ - '0';
                    }

                    break;

                case '*':
                    slen = va_arg(args, size_t);
                    fmt++;
                    continue;

                default:
                    break;
                }

                break;
            }


            switch (*fmt) {

            case 'V':
                v = va_arg(args, dl_str *);

                len = dl_min(((size_t) (last - buf)), v->len);
                buf = dl_cpymem(buf, v->data, len);
                fmt++;

                continue;

            case 's':
                p = va_arg(args, char *);

                if (slen == (size_t) -1) {
                    while (*p && buf < last) {
                        *buf++ = *p++;
                    }

                } else {
                    len = dl_min(((size_t) (last - buf)), slen);
                    buf = dl_cpymem(buf, p, len);
                }

                fmt++;

                continue;

            case 'd':
                if (sign) {
                    i64 = (int64_t) va_arg(args, int);
                } else {
                    ui64 = (uint64_t) va_arg(args, u_int);
                }
                break;

            case 'l':
                if (sign) {
                    i64 = (int64_t) va_arg(args, long);
                } else {
                    ui64 = (uint64_t) va_arg(args, ulong);
                }
                break;

            case 'D':
                if (sign) {
                    i64 = (int64_t) va_arg(args, int32_t);
                } else {
                    ui64 = (uint64_t) va_arg(args, uint32_t);
                }
                break;

            case 'L':
                if (sign) {
                    i64 = va_arg(args, int64_t);
                } else {
                    ui64 = va_arg(args, uint64_t);
                }
                break;

            case 'f':
                f = va_arg(args, double);

                if (f < 0) {
                    *buf++ = '-';
                    f = -f;
                }

                ui64 = (int64_t) f;
                frac = 0;

                if (frac_width) {

                    scale = 1;
                    for (n = frac_width; n; n--) {
                        scale *= 10;
                    }

                    frac = (uint64_t) ((f - (double) ui64) * scale + 0.5);

                    if (frac == scale) {
                        ui64++;
                        frac = 0;
                    }
                }

                buf = dl_sprintf_num(buf, last, ui64, zero, 0, width);

                if (frac_width) {
                    if (buf < last) {
                        *buf++ = '.';
                    }

                    buf = dl_sprintf_num(buf, last, frac, '0', 0, frac_width);
                }

                fmt++;

                continue;

            case 'p':
                ui64 = (uintptr_t) va_arg(args, void *);
                hex = 2;
                sign = 0;
                zero = '0';
                width = 2 * sizeof(void *);
                break;

            case 'c':
                d = va_arg(args, int);
                *buf++ = (char) (d & 0xff);
                fmt++;

                continue;

            case 'Z':
                *buf++ = '\0';
                fmt++;

                continue;

            case 'N':
                *buf++ = LF;

                fmt++;

                continue;

            case '%':
                *buf++ = '%';
                fmt++;

                continue;

            default:
                *buf++ = *fmt++;

                continue;
            }

            if (sign) {
                if (i64 < 0) {
                    *buf++ = '-';
                    ui64 = (uint64_t) -i64;

                } else {
                    ui64 = (uint64_t) i64;
                }
            }

            buf = dl_sprintf_num(buf, last, ui64, zero, hex, width);

            fmt++;

        } else {
            *buf++ = *fmt++;
        }
    }
    
    return buf;
}

static char *
dl_sprintf_num(char *buf, char *last, uint64_t ui64, char zero,
    uint hexadecimal, uint width)
{
    char            *p, temp[DL_INT64_LEN + 1];
    size_t          len;
    uint32_t        ui32;
    static char     hex[] = "0123456789abcdef";
    static char     HEX[] = "0123456789ABCDEF";

    p = temp + DL_INT64_LEN;

    if (hexadecimal == 0) {

        if (ui64 <= (uint64_t) DL_UINT32_MAX) {

            ui32 = (uint32_t) ui64;

            do {
                *--p = (char) (ui32 % 10 + '0');
            } while (ui32 /= 10);

        } else {
            do {
                *--p = (char) (ui64 % 10 + '0');
            } while (ui64 /= 10);
        }

    } else if (hexadecimal == 1) {

        do {

            /* the "(uint32_t)" cast disables the BCC's warning */
            *--p = hex[(uint32_t) (ui64 & 0xf)];

        } while (ui64 >>= 4);

    } else { /* hexadecimal == 2 */

        do {

            /* the "(uint32_t)" cast disables the BCC's warning */
            *--p = HEX[(uint32_t) (ui64 & 0xf)];

        } while (ui64 >>= 4);
    }

    /* zero or space padding */

    len = (temp + DL_INT64_LEN) - p;

    while (len++ < width && buf < last) {
        *buf++ = zero;
    }

    /* number safe copy */

    len = (temp + DL_INT64_LEN) - p;

    if (buf + len > last) {
        len = last - buf;
    }

    return dl_cpymem(buf, p, len);
}


/*
    base64
*/
static void dl_encode_base64_internal(dl_str *dst, dl_str *src, const uchar *basis, int padding);
static int dl_decode_base64_internal(dl_str *dst, dl_str *src, const uchar *basis);


void
dl_encode_base64(dl_str *dst, dl_str *src, int pad)
{
    static uchar   basis64[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    dl_encode_base64_internal(dst, src, basis64, pad);
}

void
dl_encode_base64url(dl_str *dst, dl_str *src)
{
    static uchar   basis64[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

    dl_encode_base64_internal(dst, src, basis64, 0);
}

/*
    3 * 8 = 4 * 6;
    2^6 = 64;
*/
static void
dl_encode_base64_internal(dl_str *dst, dl_str *src, const uchar *basis, int padding)
{
    uchar           *s;
    char            *d;
    size_t          len;

    len = src->len;
    s = (uchar *)src->data;
    d = dst->data;
    /*
        3 bytes represented by 4 chars
    */
    while (len > 2) {
        *d++ = basis[s[0] >> 2];
        *d++ = basis[((s[0] & 3) << 4) | (s[1] >> 4)];
        *d++ = basis[((s[1] & 0x0f) << 2) | (s[2] >> 6)];
        *d++ = basis[s[2] & 0x3f];

        s += 3;
        len -= 3;
    }

    if (len) {
        *d++ = basis[s[0] >> 2];

        if (len == 1) {
            *d++ = basis[(s[0] & 3) << 4];
            if (padding) {
                *d++ = '=';
            }

        } else {
            *d++ = basis[((s[0] & 3) << 4) | (s[1] >> 4)];
            *d++ = basis[(s[1] & 0x0f) << 2];
        }

        if (padding) {
            *d++ = '=';
        }
    }

    dst->len = d - dst->data;
}


/*
    0 -> 65(A), 1 -> 66(B)
    
    basis64[65(A)] = 0;
*/

int
dl_decode_base64(dl_str *dst, dl_str *src)
{
    static uchar   basis64[] = {
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77, 77, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
        77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 77,
        77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,

        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
    };

    return dl_decode_base64_internal(dst, src, basis64);
}

int
dl_decode_base64url(dl_str *dst, dl_str *src)
{
    static uchar   basis64[] = {
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
        77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 63,
        77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,

        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
    };

    return dl_decode_base64_internal(dst, src, basis64);
}

static int
dl_decode_base64_internal(dl_str *dst, dl_str *src, const uchar *basis)
{
    size_t          len;
    char            *d;
    uchar           *s;

    for (len = 0; len < src->len; len++) {
        if (src->data[len] == '=') {
            break;
        }
        
        // illegal base64 character
        if (basis[src->data[len]] == 77) {
            return DL_ERROR;
        }
    }
    
    /*
        xxxx
        xxxx xx==
        xxxx xxx=
        so (len % 4) could be 0,2,3 but 1
    */
    if (len % 4 == 1) {
        return DL_ERROR;
    }

    s = (uchar *)src->data;
    d = dst->data;

    /* convert 4 bytes to 3 bytes */
    while (len > 3) {
        *d++ = (uchar) (basis[s[0]] << 2 | basis[s[1]] >> 4);
        *d++ = (uchar) (basis[s[1]] << 4 | basis[s[2]] >> 2);
        *d++ = (uchar) (basis[s[2]] << 6 | basis[s[3]]);

        s += 4;
        len -= 4;
    }

    if (len > 1) {
        *d++ = (uchar) (basis[s[0]] << 2 | basis[s[1]] >> 4);
    }

    if (len > 2) {
        *d++ = (uchar) (basis[s[1]] << 4 | basis[s[2]] >> 2);
    }

    dst->len = d - dst->data;

    return DL_OK;
}


/*
    utf8
       
        bits    begin      end
        
    1	7	    U+0000	  U+007F	    0xxxxxxx			
    2	11	    U+0080	  U+07FF	    110xxxxx	10xxxxxx		
    3	16	    U+0800	  U+FFFF	    1110xxxx	10xxxxxx	10xxxxxx	
    4	21	    U+10000	  U+10FFFF	    11110xxx	10xxxxxx	10xxxxxx	10xxxxxx
    
*/

/*
 * decodes two and more bytes UTF sequences only
 * the return values:
 *    0x80 - 0x10ffff         valid character
 *    0x110000 - 0xfffffffd   invalid sequence
 *    0xfffffffe              incomplete sequence
 *    0xffffffff              error
 */

uint32_t
dl_utf8_decode(char **pp, size_t n)
{
    size_t    len;
    uint32_t  u, i, valid;
    uchar       **p;
    
    p = (uchar **)pp;
    
    u = **p;

    if (u >= 0xf0) {
        /* 11110xxx */
        
        u &= 0x07;      // get the last 3 valid bits
        valid = 0xffff; // should greater than this value
        len = 3;

    } else if (u >= 0xe0) {
        /* 1110xxxx */
        
        u &= 0x0f;      // get the last 4 valid bits
        valid = 0x7ff;
        len = 2;

    } else if (u >= 0xc2) {
        /* 110xxxxx */
        u &= 0x1f;
        valid = 0x7f;
        len = 1;

    } else {
        (*p)++;
        return 0xffffffff;
    }

    if (n - 1 < len) {
        return 0xfffffffe;
    }

    (*p)++;

    while (len) {
        i = *(*p)++;

        if (i < 0x80) {
            return 0xffffffff;
        }
        
        u = (u << 6) | (i & 0x3f);

        len--;
    }

    if (u > valid) {
        return u;
    }

    return 0xffffffff;
}

size_t
dl_utf8_length(char *p, size_t n)
{
    uchar   c;
    char    *last;
    size_t  len;

    last = p + n;

    for (len = 0; p < last; len++) {

        c = *p;
        
        // 127 ascii characters
        if (c < 0x80) {
            p++;
            continue;
        }

        if (dl_utf8_decode(&p, n) > 0x10ffff) {
            /* invalid UTF-8 */
            return n;
        }
    }

    return len;
}


/* uri escape -------------------------------------*/

uintptr_t
dl_escape_uri(uchar *dst, uchar *src, size_t size, int type)
{
    uintptr_t           n;
    uint32_t            *escape;
    static char         hex[] = "0123456789ABCDEF";

                    /* " ", "#", "%", "?", %00-%1F, %7F-%FF */
    
    static uint32_t   uri[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */      // 0-31

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x80000029, /* 1000 0000 0000 0000  0000 0000 0010 1001 */      // 63-32

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */      // 63-32

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */      // 126-96

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };

                    /* " ", "#", "%", "&", "+", "?", %00-%1F, %7F-%FF */

    static uint32_t   args[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x88000869, /* 1000 1000 0000 0000  0000 1000 0110 1001 */

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };

                    /* not ALPHA, DIGIT, "-", ".", "_", "~" */

    static uint32_t   uri_component[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */      // 0-31

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0xfc009fff, /* 1111 1100 0000 0000  1001 1111 1111 1111 */      // 63-32

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x78000001, /* 0111 1000 0000 0000  0000 0000 0000 0001 */      // 95-64

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0xb8000001, /* 1011 1000 0000 0000  0000 0000 0000 0001 */      // 126-96

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };
    
        static uint32_t   html[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x000000ad, /* 0000 0000 0000 0000  0000 0000 1010 1101 */

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };

                    /* " ", """, "%", "'", %00-%1F, %7F-%FF */

    static uint32_t   refresh[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x00000085, /* 0000 0000 0000 0000  0000 0000 1000 0101 */

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };

    static uint32_t  *map[] = { uri, args, uri_component, html, refresh};


    escape = map[type];

    if (dst == NULL) {

        /* find the number of the characters to be escaped */

        n = 0;
        
        /* char >> 5 = 3 bits = [0-7] */
        while (size) {
            if (escape[*src >> 5] & (1U << (*src & 0x1f))) {
                n++;
            }
            src++;
            size--;
        }

        return (uintptr_t) n;
    }

    while (size) {
        if (escape[*src >> 5] & (1U << (*src & 0x1f))) {
            *dst++ = '%';
            *dst++ = hex[*src >> 4];
            *dst++ = hex[*src & 0xf];
            src++;

        } else {
            *dst++ = *src++;
        }
        size--;
    }

    return (uintptr_t) dst;
}


/*
    DL_UNESCAPE_URI | DL_UNESCAPE_REDIRECT    return whem meet ?
    DL_UNESCAPE_REDIRECT   only unescape char (38-126)
*/

void
dl_unescape_uri(uchar **dst, uchar **src, size_t size, int type)
{
    uchar  *d, *s, ch, c, decoded;
    enum {
        sw_usual = 0,
        sw_quoted,
        sw_quoted_second
    } state;

    d = *dst;
    s = *src;

    state = 0;
    decoded = 0;

    while (size--) {

        ch = *s++;

        switch (state) {
        case sw_usual:
            if (ch == '?'
                && (type & (DL_UNESCAPE_URI|DL_UNESCAPE_REDIRECT)))
            {
                *d++ = ch;
                goto done;
            }

            if (ch == '%') {
                state = sw_quoted;
                break;
            }

            *d++ = ch;
            break;

        case sw_quoted:

            if (ch >= '0' && ch <= '9') {
                decoded = (uchar) (ch - '0');
                state = sw_quoted_second;
                break;
            }
            
            /* c >= 32 */
            c = (uchar) (ch | 0x20);
            if (c >= 'a' && c <= 'f') {
                decoded = (uchar) (c - 'a' + 10);
                state = sw_quoted_second;
                break;
            }

            /* the invalid quoted character */

            state = sw_usual;

            *d++ = ch;

            break;

        case sw_quoted_second:

            state = sw_usual;

            if (ch >= '0' && ch <= '9') {
                ch = (uchar) ((decoded << 4) + (ch - '0'));

                if (type & DL_UNESCAPE_REDIRECT) {
                    if (ch > '%' && ch < 0x7f) {
                        *d++ = ch;
                        break;
                    }

                    *d++ = '%'; *d++ = *(s - 2); *d++ = *(s - 1);

                    break;
                }

                *d++ = ch;

                break;
            }
            
            // c >= 32
            c = (uchar) (ch | 0x20);
            if (c >= 'a' && c <= 'f') {
                ch = (uchar) ((decoded << 4) + (c - 'a') + 10);

                if (type & DL_UNESCAPE_URI) {
                    if (ch == '?') {
                        *d++ = ch;
                        goto done;
                    }

                    *d++ = ch;
                    break;
                }

                if (type & DL_UNESCAPE_REDIRECT) {
                    if (ch == '?') {
                        *d++ = ch;
                        goto done;
                    }
                    
                    if (ch > '%' && ch < 0x7f) {
                        *d++ = ch;
                        break;
                    }
                    
                    /* do not unescape */
                    *d++ = '%'; *d++ = *(s - 2); *d++ = *(s - 1);
                    break;
                }

                *d++ = ch;

                break;
            }

            /* the invalid quoted character */

            break;
        }
    }

done:

    *dst = d;
    *src = s;
}


uintptr_t
dl_escape_html(uchar *dst, uchar *src, size_t size)
{
    uchar       ch;
    uint         len;

    /* get extra memory space */
    if (dst == NULL) {

        len = 0;

        while (size) {
            switch (*src++) {

            case '<':
                len += sizeof("&lt;") - 2;
                break;

            case '>':
                len += sizeof("&gt;") - 2;
                break;

            case '&':
                len += sizeof("&amp;") - 2;
                break;

            case '"':
                len += sizeof("&quot;") - 2;
                break;

            default:
                break;
            }
            size--;
        }

        return (uintptr_t) len;
    }

    while (size) {
        ch = *src++;

        switch (ch) {

        case '<':
            *dst++ = '&'; *dst++ = 'l'; *dst++ = 't'; *dst++ = ';';
            break;

        case '>':
            *dst++ = '&'; *dst++ = 'g'; *dst++ = 't'; *dst++ = ';';
            break;

        case '&':
            *dst++ = '&'; *dst++ = 'a'; *dst++ = 'm'; *dst++ = 'p';
            *dst++ = ';';
            break;

        case '"':
            *dst++ = '&'; *dst++ = 'q'; *dst++ = 'u'; *dst++ = 'o';
            *dst++ = 't'; *dst++ = ';';
            break;

        default:
            *dst++ = ch;
            break;
        }
        size--;
    }

    return (uintptr_t) dst;
}


/*
    escape json content between souble quotes("")
    printable characters only escape \ and "
*/

uintptr_t
dl_escape_json(uchar *dst, uchar *src, size_t size)
{
    uchar       ch;
    uint        len;

    if (dst == NULL) {
        len = 0;

        while (size) {
            ch = *src++;

            if (ch == '\\' || ch == '"') {
                len++;

            } else if (ch <= 0x1f) {

                switch (ch) {
                case '\n':
                case '\r':
                case '\t':
                case '\b':
                case '\f':
                    len++;
                    break;

                default:
                    len += sizeof("\\u001F") - 2;
                }
            }

            size--;
        }

        return (uintptr_t) len;
    }

    while (size) {
        ch = *src++;
        
        if (ch > 0x1f) {
            if (ch == '\\' || ch == '"') {
                *dst++ = '\\';
            }

            *dst++ = ch;

        } else {
            *dst++ = '\\';

            switch (ch) {
            case '\n':
                *dst++ = 'n';
                break;

            case '\r':
                *dst++ = 'r';
                break;

            case '\t':
                *dst++ = 't';
                break;

            case '\b':
                *dst++ = 'b';
                break;

            case '\f':
                *dst++ = 'f';
                break;

            default:
                *dst++ = 'u'; *dst++ = '0'; *dst++ = '0';
                *dst++ = '0' + (ch >> 4);

                ch &= 0xf;

                *dst++ = (ch < 10) ? ('0' + ch) : ('A' + ch - 10);
            }
        }

        size--;
    }

    return (uintptr_t) dst;
}

uintptr_t
dl_hextoi(uchar *line, size_t n)
{
    uchar     c, ch;
    uintptr_t  value, cutoff;

    if (n == 0) {
        return DL_ERROR;
    }

    cutoff = UINTPTR_MAX / 16;

    for (value = 0; n--; line++) {
        if (value > cutoff) {
            return DL_ERROR;
        }

        ch = *line;

        if (ch >= '0' && ch <= '9') {
            value = value * 16 + (ch - '0');
            continue;
        }

        c = (uchar) (ch | 0x20);

        if (c >= 'a' && c <= 'f') {
            value = value * 16 + (c - 'a' + 10);
            continue;
        }

        return DL_ERROR;
    }

    return value;
}

uchar *
dl_hex2bc(uchar *dst, uchar *src, size_t slen)
{
    intptr_t         value;
    uchar *cur;
    cur = src;
    
    if(slen > 0 && slen % 2 == 0){
        
        while(slen){
            value = dl_hextoi(cur, 2);
            if(value == DL_ERROR) return NULL;
            *dst++ = (uchar)value;
            
            cur += 2;
            slen -= 2;
        }
        
    }else{
        return NULL;
    }

    return dst;
}

static inline uchar dl_c2hex(uchar d){
    if(d >= 0 && d <= 9){
        d = d + '0';
    }else{
        d = d - 10 + 'a';
    }
    
    return d;
}

void dl_dump_bin2hex(char *buf, size_t len)
{
    int     i;
    uchar   d1, d2;
    
    uchar *cur = buf;
    
    for(i = 0; i < len; i++){
        d1 = *cur >> 4;
        d2 = *cur++ & 0xf;
        
        printf("%c%c", dl_c2hex(d1), dl_c2hex(d2));
        //printf(",");
    }
    
    printf("\n");
}

void dl_memcpy_rev(char *dst, char *src, size_t len)
{
    char *cur = src + len - 1;
    
    while(src <= cur){
        *dst++ = *cur--;
    }
}


/*
    checksum------------------------
*/
#include "dl_md5.h"

char *dl_md5sum(char *dst, char *str, size_t len)
{
    uchar       digest[16];
    md5_ctx     context;
    
    md5Init(&context);
    md5Update(&context, str, len);
    md5Final(digest, &context);
    
    int i;
    for(i = 0; i < 16; i++){
        sprintf(dst, "%02x", digest[i]);
        dst += 2;
    }
    
    return dst;
}

#include "dl_sha.h"

static char *dl_sha(char *dst, char *str, size_t len, enum SHAversion whichSha)
{
    USHAContext     ctx;
    uchar           digest[USHAHashSize(whichSha)];
    int             s,i;
    
    s = USHAReset(&ctx, whichSha);
    if(s) return NULL;
    
    s = USHAInput(&ctx, str, len);
    if(s) return NULL;
    
    s = USHAResult(&ctx, digest);
    if(s) return NULL;
    
    for(i = 0; i < USHAHashSize(whichSha) ; ++i){
        sprintf(dst, "%02x", digest[i]);
        dst += 2;
    }
    
    return dst;
}

char *dl_sha1sum(char *dst, char *str, size_t len){
    return dl_sha(dst, str, len, SHA1);
}
char *dl_sha224sum(char *dst, char *str, size_t len){
    return dl_sha(dst, str, len, SHA224);
}
char *dl_sha256sum(char *dst, char *str, size_t len){
    return dl_sha(dst, str, len, SHA256);
}
char *dl_sha384sum(char *dst, char *str, size_t len){
    return dl_sha(dst, str, len, SHA384);
}
char *dl_sha512sum(char *dst, char *str, size_t len){
    return dl_sha(dst, str, len, SHA512);
}

char *dl_sha_hmac(char *dst, char *str, size_t len, char *key, size_t key_len,
                  int whichSha)
{
    int     i,s;
    uint8_t digest[USHAMaxHashSize];
    
    s = dl_hmac(whichSha, str, len, key, key_len, digest);
    if(s) return NULL;
    
    for(i = 0; i < USHAHashSize(whichSha); i++){
        sprintf(dst, "%02x", digest[i]);
        dst += 2;
    }
    
    return dst;
}