/* mkpwd - generate initial random passwords
 * Usage: mkpwd [-VD] [-N num] {-<c><alphabet>} [spec]
 * Want: -R randfile to read random bytes from eg /dev/random
 * History: ujr/2004-10-30 created
 * License: GNU General Public License (GPL)
 */

#include <ctype.h>   /* isdigit */
#include <stdio.h>
#include <stdlib.h>  /* getenv */
#include <string.h>  /* strlen */
#include <time.h>    /* time */
#include <unistd.h>  /* getpid */

#include "common.h"

static char id[] = "mkpwd by ujr/2004-10-30\n";

int identity(void);
int usage(const char *s);
void generate(const char *alph[], const char *spec);
int setalph(int i, char *s);
int getint(const char *s, int *val);
void putr(int n, const char *s);
void putn(const char *s, int n);

void rndseed(unsigned seed);
unsigned rnd(unsigned lo, unsigned hi);

char *me = "mkpwd";
const char *alph[26]; /* the 26 alphabets */

int main(int argc, char **argv)
{
  int c, num = 1, debug = 0;
  const char *spec = 0; /* pwd spec; 0 means "8z" */

  /* Initialise alphabets */
  for (c = 0; c < 26; c++) alph[c] = 0;
  alph[0] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  alph[1] = "abcdefghijklmnopqrstuvwxyz";
  alph[2] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  alph[3] = "0123456789";
  alph[25] = "abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ23456789";

  (void) argc; /* unused */
  if (*argv && **argv) me = *argv;

  /* Homebrewn command line processing */
  args: while (*++argv && (**argv == '-')) {
    while ((c = *++argv[0])) switch (c) {
      case 'N': if (getint(*++argv, &num)) goto args;
                return usage("missing argument");
      case 'V': return identity();
      case 'h': return usage(0);
      case 'D': debug = 1; break;
      case '-': argv++; goto endargs;
      default:  if (setalph(c, *++argv)) goto args;
                return usage("invalid option");
    }
  }
endargs:
  if (*argv) spec = *argv++;
  if (*argv) return usage("too many arguments");

  /* If no spec on cmd line, check environ */
  if (!spec) spec = getenv("MKPWDSPEC");
  if (spec && *spec == '\0') spec = 0;

  rndseed((unsigned) time(NULL) + getpid());

  if (debug) for (c = 0; c < 26; c++) if (alph[c])
    fprintf(stderr, "%c: %s\n", c+'a', alph[c]); /* XXX */

  while (num--) generate(alph, spec);

  return SUCCESS;
}

int identity(void)
{
  return fputs(id, stdout) >= 0 ? SUCCESS : FAILHARD;
}

int usage(const char *errmsg)
{
  const char *args = "[-VD] [-N num] {-<c> alphabet} [spec]";
  if (errmsg) {
    fprintf(stderr, "%s: %s\n", me, errmsg);
    fprintf(stderr, "Usage: %s %s\n", me, args);
  }
  else {
    fprintf(stdout, "Generate passwords according to spec; see manual.\n");
    fprintf(stdout, "Usage: %s %s\n", me, args);
  }
  return errmsg ? FAILHARD : SUCCESS;
}

void generate(const char *alph[], const char *spec)
{
  if (spec) while (*spec) {
    if (isdigit(*spec)) { int c, n;
      c = getint(spec, &n);
      if (c && n && islower(spec[c]))
        putr(n, alph[spec[c]-'a']), spec++;
      else putn(spec, c);
      spec += c;
    }
    else putc(*spec++, stdout);
  }
  else putr(8, alph[25]); /* default spec */

  putc('\n', stdout);
}

int getint(const char *s, int *val)
{
  register int c, i = 0;
  register const char *p = s;

  if (s) while ((c = (unsigned char) (*p - '0')) < 10)
    i = 10 * i + c, p++;
  if (p > s && val) *val = i;
  return p - s; /* #chars scanned */
}

int setalph(int i, char *s)
{
  if (i < 'a' || i > 'z') return 0; /* error */
  alph[i - 'a'] = (s && *s) ? s : 0; /* map empty to null */
  return 1; /* OK */
}

void putr(int n, const char *a)
{
  int alen;
  if (!a || !*a) return;
  alen = strlen(a) - 1;
  while (n-- > 0) putc(a[rnd(0,alen)], stdout);
}

void putn(const char *s, int n)
{
  if (s && n > 0) fwrite(s, 1, n, stdout);
}

/* Quick+Dirty random numbers
 *
 * Based on chapter 7.1 in Numerical Recipes in C (www.nr.com).
 * It is a linear congruential generator with the constants m (modulus),
 * a (multiplier), and c (increment) taken from the table on page 285.
 *
 * TODO Find better constants for 8bit randoms; adjust types.
 */
static unsigned long r = 1;
void rndseed(unsigned seed) { r = seed; }

unsigned rnd(unsigned lo, unsigned hi)
{
  static unsigned a=9301, c=49297, m=233280;
  r = (r * a + c) % m;
  return lo + ((hi-lo+1)*r)/m;
}
