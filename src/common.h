
#include <limits.h>

/* Return status */

#define SUCCESS 0
#define FAILSOFT 111
#define FAILHARD 127

/* Text scanning */

int scanlong(const char *s, long *vp);
int scanuint(const char *s, unsigned int *vp);

/* Additional types */

/* Note: <stdint.h> has uint32_t and the like,
   but <stdint.h> is not part of ANSI C; roll
   our own using <limits.h> maximum values: */

#if UINT_MAX == 0xFFFFFFFF
typedef unsigned int uint32;
#elif ULONG_MAX = 0xFFFFFFFF
typedef unsigned long uint32;
#else
#error "Unsupported word size"
#endif
