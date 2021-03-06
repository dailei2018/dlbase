#ifndef __DL_BASE
#define __DL_BASE

#include "dl_config.h"

typedef struct _dl_pool dl_pool;
typedef struct _dl_array dl_array;
typedef struct _dl_list dl_list;

#define  DL_OK          0
#define  DL_ERROR      -1
#define  DL_AGAIN      -2
#define  DL_BUSY       -3
#define  DL_DONE       -4
#define  DL_DECLINED   -5
#define  DL_ABORT      -6


#include "dl_string.h"
#include "dl_array.h"
#include "dl_list.h"
#include "dl_log.h"
#include "dl_pool.h"
#include "dl_bstree.h"
#include "dl_hash.h"
#include "dl_phash.h"
#include "dl_queue.h"
#include "dl_fifo.h"
#include "dl_rbtree.h"
#include "dl_table.h"
#include "dl_time.h"
#include "dl_shm.h"

#endif
