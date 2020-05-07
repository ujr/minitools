/* Schweizerische Matrikelnummer ("Leginummer")
 * Usage: legick <matrikel>
 * History: ujr/2003-04-02 created
 * License: GNU General Public License (GPL)
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static int scan(const char *s, char *buf);
static int check(const char *buf, int *c);
static void die(const char *msg) { fprintf(stderr, "%s\n", msg); exit(99); }

int main(int argc, char *argv[])
{
  char buf[10];
  int c;

  (void) argc; /* unused */
  argv++; /* shift */
  if (!*argv) die("missing argument");
  switch (scan(*argv, buf)) {
    case 7: buf[7] = '\0'; check(buf, &c);
      fprintf(stdout, "%s%c check\n", buf, c);
      return 0;
    case 8: buf[8] = '\0'; c = check(buf, NULL);
      fprintf(stdout, "%s %s\n", buf, (c) ? "ok" : "wrong");
      return c == 0;
    default: die("malformed argument");
  }
  return 127; /* not reached */
}

static int scan(const char *s, char *buf)
{
  if (isdigit(*s)) *buf++ = *s++; else return 0;
  if (isdigit(*s)) *buf++ = *s++; else return 0;
  if (*s == '-') s++; /* skip optional dash */
  if (isdigit(*s)) *buf++ = *s++; else return 0;
  if (isdigit(*s)) *buf++ = *s++; else return 0;
  if (isdigit(*s)) *buf++ = *s++; else return 0;
  if (*s == '-') s++; /* skip optional dash */
  if (isdigit(*s)) *buf++ = *s++; else return 0;
  if (isdigit(*s)) *buf++ = *s++; else return 0;
  if (isdigit(*s)) *buf++ = *s++; else return 7;
  return isdigit(*s) ? 0 : 8;
}

static int check(const char *buf, int *c)
{
  int i, t, u;

  for (i = 0, u = 0; i < 7; i++) {
    t = *buf++;
    assert(isdigit(t));
    t -= '0';
    t *= 2 - (i % 2);
    u += t;
    if (t > 9) u -= 9;
  }
  t = ((10 - (u % 10)) % 10) + '0';
  if (c) *c = t;
  return *buf == t;
}
