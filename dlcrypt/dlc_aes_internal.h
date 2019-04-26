#ifndef __DLC_INTERNAL__
#define __DLC_INTERNAL__


/* Define to use only small tables. */
#ifndef AES_SMALL
# define AES_SMALL 0
#endif

#if AES_SMALL
# define AES_TABLE_SIZE 1
#else
# define AES_TABLE_SIZE 4
#endif

struct aes_table
{
  uint8_t sbox[0x100];
  uint32_t table[AES_TABLE_SIZE][0x100];
};

void
_aes_encrypt(const aes_ctx *ctx,
	     const struct aes_table *T,
	     unsigned length, uint8_t *dst,
	     const uint8_t *src);

void
_aes_decrypt(const aes_ctx *ctx,
	     const struct aes_table *T,
	     unsigned length, uint8_t *dst,
	     const uint8_t *src);

/* Macros */
#define ROTBYTE(x) (((x) >> 8) | (((x) & 0xff) << 24))
#define ROTRBYTE(x) (((x) << 8) | (((x) >> 24) & 0xff))
#define SUBBYTE(x, box) (((box)[((x) & 0xff)]) | \
                        ((box)[(((x) >> 8) & 0xff)] << 8) | \
                        ((box)[(((x) >> 16) & 0xff)] << 16) | \
                        ((box)[(((x) >> 24) & 0xff)] << 24))

/* Get the byte with index 0, 1, 2 and 3 */
#define B0(x) ((x) & 0xff)
#define B1(x) (((x) >> 8) & 0xff)
#define B2(x) (((x) >> 16) & 0xff)
#define B3(x) (((x) >> 24) & 0xff)

#define AES_ROUND(T, w0, w1, w2, w3, k)		\
((  T->table[0][ B0(w0) ]			\
  ^ T->table[1][ B1(w1) ]			\
  ^ T->table[2][ B2(w2) ]			\
  ^ T->table[3][ B3(w3) ]) ^ (k))

#define AES_FINAL_ROUND(T, w0, w1, w2, w3, k)		\
((   (uint32_t) T->sbox[ B0(w0) ]			\
  | ((uint32_t) T->sbox[ B1(w1) ] << 8)			\
  | ((uint32_t) T->sbox[ B2(w2) ] << 16)		\
  | ((uint32_t) T->sbox[ B3(w3) ] << 24)) ^ (k))
     
/* Globally visible so that the same sbox table can be used by aes_set_encrypt_key */

extern const struct aes_table _aes_encrypt_table;
#define aes_sbox (_aes_encrypt_table.sbox)

void dlc_bxor(char *dst, char *s1, char *s2, int len);


#endif