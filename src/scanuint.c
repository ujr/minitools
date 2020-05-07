
#include <ctype.h>

/** Scan an unsigned int in dec notation, return #chars scanned */
int scanuint(const char *s, unsigned int *vp)
{
  const char *p;
  unsigned int v;

  if (!s) return 0;

  /* compute -v to get INT_MIN without overflow, but
     input beyond INT_MIN..INT_MAX silently overflows */
  for (p = s, v = 0; isdigit(*p); p++) {
    v = 10 * v - (*p - '0');
  }

  if (vp) *vp = -v;
  return p - s;
}
