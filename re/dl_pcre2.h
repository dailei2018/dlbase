#ifndef __DL_PCRE
#define __DL_PCRE

#include "dl_base.h"

#define DL_CAPTURE_ONE  0
#define DL_CAPTURE_ALL  1
#define dl_free_re(re) pcre2_code_free(re)

pcre2_code *dl_compile_re(char *pattern, size_t len, dl_log *log);
dl_str *
dl_str_replace_re(dl_pool *p, char *s1, size_t len, char *pattern,
                   char *s3, size_t len_3, pcre2_code *rep);
int dl_str_match_re(char *s1, size_t len, char *pattern, pcre2_code *rep, dl_log *log);

dl_list * dl_str_capture_re(dl_pool *p, char *s1, size_t len, char *pattern, pcre2_code *rep, int all);
#endif