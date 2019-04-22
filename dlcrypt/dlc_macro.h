#ifndef __DLC_BASE__
#define __DLC_BASE__

/* Reads a 32-bit integer, in network, big-endian, byte order */
#define READ_UINT32(p)				\
(  (((uint32_t) (p)[0]) << 24)			\
 | (((uint32_t) (p)[1]) << 16)			\
 | (((uint32_t) (p)[2]) << 8)			\
 |  ((uint32_t) (p)[3]))

#define WRITE_UINT32(p, i)			\
do {						\
  (p)[0] = ((i) >> 24) & 0xff;			\
  (p)[1] = ((i) >> 16) & 0xff;			\
  (p)[2] = ((i) >> 8) & 0xff;			\
  (p)[3] = (i) & 0xff;				\
} while(0)

/* Analogous macros, for 24 and 16 bit numbers */
#define READ_UINT24(p)				\
(  (((uint32_t) (p)[0]) << 16)			\
 | (((uint32_t) (p)[1]) << 8)			\
 |  ((uint32_t) (p)[2]))

#define WRITE_UINT24(p, i)			\
do {						\
  (p)[0] = ((i) >> 16) & 0xff;			\
  (p)[1] = ((i) >> 8) & 0xff;			\
  (p)[2] = (i) & 0xff;				\
} while(0)

#define READ_UINT16(p)				\
(  (((uint32_t) (p)[0]) << 8)			\
 |  ((uint32_t) (p)[1]))

#define WRITE_UINT16(p, i)			\
do {						\
  (p)[0] = ((i) >> 8) & 0xff;			\
  (p)[1] = (i) & 0xff;				\
} while(0)

/* And the other, little-endian, byteorder */
#define LE_READ_UINT32(p)			\
(  (((uint32_t) (p)[3]) << 24)			\
 | (((uint32_t) (p)[2]) << 16)			\
 | (((uint32_t) (p)[1]) << 8)			\
 |  ((uint32_t) (p)[0]))

#define LE_WRITE_UINT32(p, i)			\
do {						\
  (p)[3] = ((i) >> 24) & 0xff;			\
  (p)[2] = ((i) >> 16) & 0xff;			\
  (p)[1] = ((i) >> 8) & 0xff;			\
  (p)[0] = (i) & 0xff;				\
} while(0)

/* Analogous macros, for 16 bit numbers */
#define LE_READ_UINT16(p)			\
  (  (((uint32_t) (p)[1]) << 8)			\
     |  ((uint32_t) (p)[0]))

#define LE_WRITE_UINT16(p, i)			\
  do {						\
    (p)[1] = ((i) >> 8) & 0xff;			\
    (p)[0] = (i) & 0xff;			\
  } while(0)

/* Macro to make it easier to loop over several blocks. */
#define FOR_BLOCKS(length, dst, src, blocksize)	\
  assert( !((length) % (blocksize)));           \
  for (; (length); ((length) -= (blocksize),	\
		  (dst) += (blocksize),		\
		  (src) += (blocksize)) )


#endif