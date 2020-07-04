/* Make and dissect IEEE 754 binary64 floating-point numbers
 * Usage: ieee754 <real>                # print mantissa and exponent
 *    or: ieee754 <mantissa> <exponent> # print mantissa*2^exponent
 * Examples:
 *   ieee754 4.0     => 4 0     (because 4*2^0 == 4.0)
 *   ieee754 4 0     => 4.0
 *   ieee754 2 1     => 4.0     (because 2*2^1 == 4.0)
 *   ieee754 3.25    => 13 -2   (because 13/2^2 == 3.25)
 * Binary64 (double precision) format is:
 * Sign(1), Exponent(11), Significand(52)
 */

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double makedbl(int64_t m, int e);
void splitdbl(double r, int64_t *pm, int *pe);
double makenan();

int
main(int argc, char **argv)
{
  const char *me;
  double r;
  int64_t m;
  int e;

  me = argc > 0 ? argv[0] : "ieee754";

  if (argc == 2) {
    r = atof(argv[1]);
    splitdbl(r, &m, &e);
    printf("%ld %d\n", m, e);
    return 0;
  }

  if (argc == 3) {
    m = atol(argv[1]);
    e = atoi(argv[2]);
    r = makedbl(m, e);
    printf("%.15g\n", r);
    return 0;
  }

  fprintf(stderr, "Make and split IEEE 754 binary64 floating-point numbers.\n");
  fprintf(stderr, "The equation is: double real = mantissa * 2 ^ exponent.\n");
  fprintf(stderr, "Usage: %s <real>\n", me);
  fprintf(stderr, "   or: %s <mantissa> <exponent>\n", me);

  /* Running some tests */

  assert(4.0 == makedbl(4, 0));
  assert(4.0 == makedbl(2, 1));
  assert(4.0 == makedbl(1, 2));
  assert(-3.25 == makedbl(-13, -2));

  splitdbl(4.0, &m, &e);
  assert(m == 4 && e == 0);
  splitdbl(-3.25, &m, &e);
  assert(m == -13 && e == -2);

  return 127;
}

double makedbl(int64_t m, int e)
{
  double r;
  int neg = 0;

  if (m < 0) {
    m = -m; neg = 1;
    /* beware 2's complement: int i = INT_MIN; i = -i; now still INT_MIN */
    if (m < 0) return makenan();
  }
  else if (m == 0) return 0.0; /* always zero, ignore e */

  /* keep only the 53 most significant bits (make room for exponent) */
  while ((m >> 32) & 0xFFE00000) {
    m >>= 1; e += 1;
  }

  /* normalize: shift out leading 0 bits */
  if (m != 0) while (((m >> 32) & 0xFFF00000) == 0) {
    m <<= 1; e -= 1;
  }

  /* add bias and adjust for decimal point left (not right) of m */
  e += 1023 + 52;

  /* now e in 0..2047 or the magnitude is out of range for double */
  if (e > 2047) { m = 0; e = 2047; } /* positive infty */
  else if (e < 0) m = e = 0;         /* underflow to zero TODO subnorm */

  m &= (((int64_t) 1) << 52) - 1;    /* mask low 52 bits */
  m |= ((int64_t) e) << 52;          /* add the exponent */
  if (neg) m |= ((int64_t) 1) << 63; /* add the sign bit */

  assert(sizeof(r) == sizeof(m));
  memcpy(&r, &m, sizeof(r));

  return r;
}

void splitdbl(double r, int64_t *pm, int *pe)
{
  int64_t m;
  int e;
  int neg = 0;

  if (r < 0) {
    r = -r;
    neg = 1;
  }

  assert(sizeof(m) == sizeof(r));
  memcpy(&m, &r, sizeof(m));

  if (m == 0) {
    if (pm) *pm = 0;
    if (pe) *pe = 0;
    return;
  }

  /* extract exponent and mantissa */
  e = m >> 52;
  m = m & ((((int64_t) 1) << 52) - 1);
  if (e == 0) m <<= 1; /* denormalized */
  else m |= ((int64_t) 1) << 52; /* implicit bit */

  /* de-normalize for readability */
  while (e < 1075 && m > 0 && (m&1) == 0) {
    m >>= 1; e += 1;
  }

  /* remove bias and move decimal point right of mantissa */
  e -= 1023 + 52;

  if (neg) m = -m;

  if (pm) *pm = m;
  if (pe) *pe = e;
}

double makenan()
{
  double r;
  int64_t a;
  a = 1 | (((int64_t) 2047) << 52);
  memcpy(&r, &a, sizeof(r));
  return r;
}

