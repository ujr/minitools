// Taken largely from ext/misc/ieee754.c of the SQLite sources

// Functions for exact display and input of IEEE754
// Binary64 floating-point numbers. Examples:
//
//   ieee754 2.0     =>  2 0
//   ieee754 45.25   =>  181 -2
//   ieee754 2 0     =>  2.0
//   ieee754 181 -2  =>  45.25
//
// See Wikipedia entries for IEEE_754 and Floating_point

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// IEEE Binary64 (double precision) format:
// Sign(1), Exponent(11), Significand(52)

// Note:
// 64bit masks are constructed using bit shifting (eg, a |= 1 << 63)
// instead of being written literally (eg, 0x8000000000000000),
// because there is no portable way to write 64bit literals.

static double
ieee754make(int64_t m, int64_t e)
{
  int64_t a;
  double r;
  int neg = 0;
  assert(sizeof(r) == sizeof(a));
  if (m < 0) {
    neg = 1;
    m = -m;
    // Note that m may still be negative: if m is the least
    // representable int64_t, then -m is not representable,
    // because with 2's complement, there's one more negative
    // integer than there are positive integers. Return NaN.
    if (m < 0) return 0.0/0.0;
  }
  else if (m == 0) {// && e > 1000 && e < 1000) {
    return 0.0;
  }
  while ((m >> 32) & 0xffe00000) {
    m >>= 1; e += 1;
  }
  while (((m >> 32) & 0xfff00000) == 0) {
    m <<= 1; e -= 1;
  }
  e += 1075;
  if (e < 0) e = m = 0;
  if (e > 0x7ff) m = 0;
  a = m & ((((int64_t) 1) << 52) - 1); // mask lower 52 bits
  a |= e << 52; // or in the exponent bits
  if (neg) a |= ((int64_t) 1) << 63; // or in the sign bit
fprintf(stderr, "a=%ld\n", a);
  memcpy(&r, &a, sizeof(r));
  return r;
}

static void
ieee754split(double r, int64_t *pm, int *pe)
{
  int64_t a, m;
  int e, neg;
  assert(sizeof(a) == sizeof(r));
  if (r < 0.0) { neg = 1; r = -r; }
  else { neg = 0; }
  memcpy(&a, &r, sizeof(a));
  if (a == 0) { m = 0; e = 0; }
  else {
    e = a >> 52; // get exponent bits
    m = a & ((((int64_t) 1) << 52) - 1); // mask mantissa bits
    m |= ((int64_t) 1) << 52; // or-in the 'hidden' bit
    while (e < 1075 && m > 0 && (m&1)==0) {
      m >>= 1; e += 1;
    }
    e -= 1075;
    if (neg) m = -m;
  }
  *pm = m;
  *pe = e;
}

#define OK (0)
#include <limits.h>

static const char *me;

int
main(int argc, char **argv)
{
  char c;
  int i;

  c = CHAR_MIN;
  c = -c;

  i = INT_MIN;
  i = -i;

  printf("c = %d  -c = %d  i = %d  -i = %d\n", CHAR_MIN, c, INT_MIN, i);

  me = argc > 0 ? argv[0] : "ieee754";

  if (argc == 2) {
    int64_t m;
    int e;
    double r = atof(argv[1]);
    ieee754split(r, &m, &e);
    printf("%lg = %ld * 2 ^ %d\n", r, m, e);
    return OK;
  }

  if (argc == 3) {
    int64_t m = atoll(argv[1]);
    int e = atoi(argv[2]);
    double r = ieee754make(m, e);
    printf("%lg = %ld * 2 ^ %d\n", r, m, e);
    return OK;
  }


  fprintf(stderr, "Usage: %s <real>\n", me);
  fprintf(stderr, "   or: %s <mantissa> <exponent>\n", me);
  return 127;
}
