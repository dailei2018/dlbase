#ifndef __DL_STRING
#define __DL_STRING

#include "dl_base.h"

#define dl_cpymem(dst, src, n)   (((char *) memcpy(dst, src, n)) + (n))

typedef struct _dl_str dl_str;

struct _dl_str{
    size_t len;
    char *data;
};

#define dl_string(str)     { sizeof(str) - 1, (char *) str};

void dl_exit_msg(int err, const char *fmt, ...);
void dl_printf(const char *fmt, ...);
void dl_printf_core(const char *fmt, va_list args);
char * dl_sprintf(char *buf, const char *fmt, ...);
char * dl_snprintf(char *buf, size_t max, const char *fmt, ...);
char * dl_slprintf(char *buf, char *last, const char *fmt, ...);
char * dl_vslprintf(char *buf, char *last, const char *fmt, va_list args);

char * dl_strnchr(char *str, int len, char c);
dl_array * dl_split_by_c(dl_pool *p, dl_str *str, char c);

char * dl_strnstr(char *s1, size_t len,  char *s2, size_t len_1);
int dl_substr_num(char *s1, size_t len,  char *s2, size_t len_1);
dl_array * dl_split_by_s(dl_pool *p, char *s1, size_t len, char *s2, size_t len_1);
dl_list * dl_split_by_s_l(dl_pool *p, char *s1, size_t len, char *s2, size_t len_1);

dl_str *
dl_str_replace(dl_pool *p, char *s1, size_t len, char *s2, size_t len_1, char *s3, size_t len_2);

char *dl_pstrdup(dl_pool *pool, char *data, size_t len);
char *dl_pstrdup_nt(dl_pool *pool, char *data, size_t len);
char *dl_strdup_nt(char *data, size_t len);
int dl_atoi(char *line, size_t n);

/* base64 */
void dl_encode_base64(dl_str *dst, dl_str *src);
void dl_encode_base64url(dl_str *dst, dl_str *src);

int dl_decode_base64(dl_str *dst, dl_str *src);
int dl_decode_base64url(dl_str *dst, dl_str *src);

#define base64_len(n)   ((n*4/3 + 3) & ~3)


/* utf8 */
uint32_t dl_utf8_decode(char **p, size_t n);
size_t dl_utf8_length(char *p, size_t n);

/* escape uri */

#define DL_ESCAPE_URI            0
#define DL_ESCAPE_ARGS           1
#define DL_ESCAPE_URI_COMPONENT  2
#define DL_ESCAPE_HTML           3
#define DL_ESCAPE_REFRESH        4

#define DL_UNESCAPE_URI         1
#define DL_UNESCAPE_REDIRECT    2

uintptr_t dl_escape_uri(char *dst, char *srcc, size_t size, int type);
void dl_unescape_uri(uchar **dst, uchar **src, size_t size, int type);

/*
 * debug
 */


#endif
