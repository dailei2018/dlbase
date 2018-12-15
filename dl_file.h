#ifndef __DL_FILE
#define __DL_FILE

#include "dl_base.h"

typedef struct _dl_file dl_file;

struct _dl_file{
    int fd;
    dl_str name;
};

#endif
