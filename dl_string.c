#include "dl_base.h"

static void dl_printf_core(const char *fmt, va_list args);
static char *
dl_sprintf_num(char *buf, char *last, uint64_t ui64, char zero,
               uint hexadecimal, uint width);

dl_array * dl_split_by_c(dl_array *arr, dl_str *str, char c)
{
    if(str == NULL || arr == NULL) return NULL;

    char *begin,*end,*cur;
    int len;

    begin = str->data;
    end = str->data + str->len;
    len = str->len;

    dl_str *el;

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
    printf
*/

void exit_msg(int err, const char *fmt, ...){
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

static void dl_printf_core(const char *fmt, va_list args){
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