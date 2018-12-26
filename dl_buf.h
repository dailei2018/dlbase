#ifndef __DL_BUF
#define __DL_BUF

#include "dl_base.h"


typedef struct _dl_buf dl_buf;
struct _dl_buf{
    char          *pos;
    char          *last;

    char          *start;         /* start of buffer */
    char          *end;           /* end of buffer */
};

#endif