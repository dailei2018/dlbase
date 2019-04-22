#include "dlc_base.h"
#include "gmp-impl.h"

struct bases mp_bases[257];

size_t
mpz_out_str (FILE *stream, int base, mpz_srcptr x)
{
  mp_ptr xp;
  mp_size_t x_size = SIZ (x);
  unsigned char *str;
  size_t str_size;
  size_t i;
  size_t written;
  const char *num_to_text;

  if (stream == 0)
    stream = stdout;

  if (base >= 0)
    {
      num_to_text = "0123456789abcdefghijklmnopqrstuvwxyz";
      if (base <= 1)
	base = 10;
      else if (base > 36)
	{
	  num_to_text = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	  if (base > 62)
	    return 0;
	}
    }
  else
    {
      base = -base;
      if (base <= 1)
	base = 10;
      else if (base > 36)
	return 0;
      num_to_text = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    }

  written = 0;

  if (x_size < 0)
    {
      fputc ('-', stream);
      x_size = -x_size;
      written = 1;
    }


  DIGITS_IN_BASE_PER_LIMB (str_size, x_size, base);
  str_size += 3;
  str = (unsigned char *) alloca(str_size);

  xp = PTR (x);
  if (! POW2_P (base))
    {
      xp = alloca( (x_size | 1) * sizeof(mp_limb_t));  /* |1 in case x_size==0 */
      mpn_copyi(xp, PTR (x), x_size);
    }

  str_size = mpn_get_str (str, base, xp, x_size);

  /* Convert result to printable chars.  */
  for (i = 0; i < str_size; i++)
    str[i] = num_to_text[str[i]];
  str[str_size] = 0;

  {
    size_t fwret;
    fwret = fwrite ((char *) str, 1, str_size, stream);
    written += fwret;
  }

  return ferror (stream) ? 0 : written;
}
