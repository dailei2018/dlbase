#ifndef __DLC_ASN1__
#define __DLC_ASN1__


/* enum asn1_type keeps the class number and the constructive in bits
   13-14, and the constructive flag in bit 12. The remaining 14 bits
   are the tag (although currently, only tags in the range 0-30 are
   supported). */

enum
  {
    ASN1_TYPE_CONSTRUCTED = 1 << 12,

    ASN1_CLASS_UNIVERSAL = 0,
    ASN1_CLASS_APPLICATION = 1 << 13,
    ASN1_CLASS_CONTEXT_SPECIFIC = 2 << 13,
    ASN1_CLASS_PRIVATE = 3 << 13,

    ASN1_CLASS_MASK = 3 << 13,
    ASN1_CLASS_SHIFT = 13,
  };

enum asn1_type
  {
    ASN1_BOOLEAN = 1,
    ASN1_INTEGER = 2,
    ASN1_BITSTRING = 3,
    ASN1_OCTETSTRING = 4,
    ASN1_NULL = 5,
    ASN1_IDENTIFIER = 6,
    ASN1_REAL = 9,
    ASN1_ENUMERATED = 10,
    ASN1_UTF8STRING = 12,
    ASN1_SEQUENCE = 16 | ASN1_TYPE_CONSTRUCTED,
    ASN1_SET = 17 | ASN1_TYPE_CONSTRUCTED,
    ASN1_PRINTABLESTRING = 19,
    ASN1_TELETEXSTRING = 20,
    ASN1_IA5STRING = 22,
    ASN1_UTC = 23,
    ASN1_UNIVERSALSTRING = 28,
    ASN1_BMPSTRING = 30,
  };

enum asn1_iterator_result
  {
    ASN1_ITERATOR_ERROR,
    ASN1_ITERATOR_PRIMITIVE,
    ASN1_ITERATOR_CONSTRUCTED,
    ASN1_ITERATOR_END,
  };

/* Parsing DER objects. */
struct asn1_der_iterator
{
  unsigned buffer_length;
  const uint8_t *buffer;

  /* Next object to parse. */
  unsigned pos;

  enum asn1_type type;

  /* Pointer to the current object */
  unsigned length;
  const uint8_t *data;
};

/* Initializes the iterator. */
enum asn1_iterator_result
asn1_der_iterator_first(struct asn1_der_iterator *iterator,
			unsigned length, const uint8_t *input);

enum asn1_iterator_result
asn1_der_iterator_next(struct asn1_der_iterator *iterator);

/* Starts parsing of a constructed object. */
enum asn1_iterator_result
asn1_der_decode_constructed(struct asn1_der_iterator *i,
			    struct asn1_der_iterator *contents);

/* For the common case that we have a sequence at the end of the
   object. Checks that the current object is the final one, and ten
   reinitialize the iterator to parse its ontents. */
enum asn1_iterator_result
asn1_der_decode_constructed_last(struct asn1_der_iterator *i);

enum asn1_iterator_result
asn1_der_decode_bitstring(struct asn1_der_iterator *i,
			  struct asn1_der_iterator *contents);

enum asn1_iterator_result
asn1_der_decode_bitstring_last(struct asn1_der_iterator *i);

/* All these functions return 1 on success, 0 on failure */
int
asn1_der_get_uint32(struct asn1_der_iterator *i,
		    uint32_t *x);


#endif
