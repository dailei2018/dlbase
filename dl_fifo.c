#include "dl_base.h"

void dl_fifo_destroy(dl_fifo *fifo)
{
    if (fifo->p == NULL) {
        dl_free(fifo->data);
        dl_free(fifo);
    }
}

dl_fifo *dl_fifo_init(unsigned int size, unsigned int esize, dl_pool *p)
{
    dl_fifo     *fifo;
    
    if (p)
        fifo = dl_palloc(p, sizeof(dl_fifo));
    else
        fifo = dl_alloc(sizeof(dl_fifo), NULL);
    
    if (fifo == NULL)
        return NULL;
    
    size = 1 << ceillog2(size);

    fifo->in = 0;
    fifo->out = 0;
    fifo->esize = esize;
    
    if (size < 2)
        return NULL;
    
    if (p)
        fifo->data = dl_palloc(p, size * esize);
    else
        fifo->data = dl_alloc(size * esize, NULL);
    
    if (fifo->data == NULL)
        return NULL;
    
    fifo->mask = size - 1;
    fifo->p = p;
    
    return fifo;
}

static void dl_fifo_copy_in(dl_fifo *fifo, void *src, unsigned int len, unsigned int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = dl_min(len, size - off);

    // 往前拷贝
	memcpy(fifo->data + off, src, l);
    
    // 没拷贝完的，从头开始拷贝
	memcpy(fifo->data, src + l, len - l);
    
    // 保证 fifo->in 在拷贝之后才增加(进出就无需加锁，但是进跟进之间，出跟出之间还是要加锁)
    dl_barrier();
}

static inline int dl_fifo_unused(dl_fifo *fifo)
{
	return (fifo->mask + 1) - (fifo->in - fifo->out);
}

unsigned int dl_fifo_in(dl_fifo *fifo, void *buf, unsigned int len)
{
	unsigned int     l;

	l = dl_fifo_unused(fifo);
	if (len > l)
		len = l;

	dl_fifo_copy_in(fifo, buf, len, fifo->in);
	fifo->in += len;
	return len;
}




static void dl_fifo_copy_out(dl_fifo *fifo, void *dst, unsigned int len, int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = dl_min(len, size - off);
    
    
	memcpy(dst, fifo->data + off, l);
	memcpy(dst + l, fifo->data, len - l);
    
    // 保证 fifo->out 在拷贝之后才增加(进出就无需加锁，但是进跟进之间，出跟出之间还是要加锁)
    dl_barrier();
}

unsigned int dl_fifo_out_peek(dl_fifo *fifo, void *buf, unsigned int len)
{
	unsigned int l;

	l = fifo->in - fifo->out;
	if (len > l)
		len = l;

	dl_fifo_copy_out(fifo, buf, len, fifo->out);
	return len;
}

unsigned int dl_fifo_out(dl_fifo *fifo, void *buf, unsigned int len)
{
	len = dl_fifo_out_peek(fifo, buf, len);
	fifo->out += len;
	return len;
}