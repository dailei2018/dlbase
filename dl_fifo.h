#ifndef __DL_FIFO
#define __DL_FIFO

#include "dl_base.h"

typedef struct _dl_fifo dl_fifo;
struct _dl_fifo {
    unsigned int	in;
    unsigned int	out;
    unsigned int	esize;
    unsigned int	mask;
    void		    *data;
    dl_pool         *p;
};


dl_fifo *dl_fifo_init(unsigned int size, unsigned int esize, dl_pool *p);
void dl_fifo_destroy(dl_fifo *fifo);
unsigned int dl_fifo_in(dl_fifo *fifo, void *buf, unsigned int len);
unsigned int dl_fifo_out(dl_fifo *fifo, void *buf, unsigned int len);

#endif