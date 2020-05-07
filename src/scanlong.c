
#include <ctype.h>

/** Scan a long int in decimal notation, return #chars scanned */
int scanlong(const char *s, long *vp)
{
  const char *p;
  int neg;
  long v;

  if (!s) return 0;

  p = s;
  neg = 0;

  switch (*p) {
    case '-': neg=1; /* FALLTHRU */
    case '+': p+=1; break;
  }

  /* compute -v to get LONG_MIN without overflow, but
     input beyond LONG_MIN..LONG_MAX silently overflows */
  for (v = 0; isdigit(*p); p++) {
    v = 10 * v - (*p - '0');
  }

  if (vp) *vp = neg ? v : -v;
  return p - s; /* #chars scanned */
}
