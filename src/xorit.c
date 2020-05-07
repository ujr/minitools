/* xorit - xor input against a string
 * Usage: xorit [-hvV] [-f file ] [string]
 * History: ujr/2003-06-02 created
 * License: GNU General Public License (GPL)
 */

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "common.h"

static char id[] = "xorit by ujr/2003-06-02\n";

int identity(void);
int usage(const char *errmsg);
void xorit(char *buf, size_t len, const char *x, size_t xlen);
const char *xload(const char *fn, size_t *lenp);
int logup(int code, const char *fmt, ...);

char *progname = "xorit";
int verbose = 0;

int main(int argc, char **argv)
{
  int c;
  const char *fn = 0;
  const char *x = 0;
  size_t xlen;
  ssize_t r;
  unsigned long n;
  char buf[BUFSIZ];

  (void) argc; /* unused */
  if (*argv && **argv) progname = *argv;

args: while (*++argv && (**argv == '-')) {
    while ((c = *++argv[0])) switch (c) {
      case 'f': if ((fn = *++argv)) goto args;
                return usage("missing argument");
      case 'v': verbose = 1; break;
      case 'V': return identity();
      case 'h': return usage(0);
      case '-': argv++; goto endargs;
      default: return usage("invalid option");
    }
  }
endargs:
  if (*argv) x = *argv++, xlen = strlen(x);
  if (*argv) return usage("too many arguments");

  if (!x) {
    if (fn) {
      x = xload(fn, &xlen); /* load from xor file */
      if (!x)
        return logup(FAILSOFT, "cannot load xor file %s: %s", fn, strerror(errno));
      if (verbose)
        logup(0, "%ld bytes read from xor file %s", xlen, fn);
    }
    else x = "\377", xlen = 1; /* default: hex FF */
  }
  if (xlen < 1) return logup(FAILHARD, "xor file/string length must be at least 1");

  r = 1; n = 0;
  while (r > 0) {
    if ((r = read(0, buf, sizeof buf)) < 0)
      return logup(FAILSOFT, "error reading stdin: %s", strerror(errno));
    xorit(buf, r, x, xlen);  n += r;
    if (write(1, buf, r) != r)
      return logup(FAILSOFT, "error writing stdout: %s", strerror(errno));
  }
  if (verbose) logup(0, "processed %ld bytes", n);

  return SUCCESS;
}

int identity(void)
{
  return fputs(id, stdout) >= 0 ? SUCCESS : FAILSOFT;
}

int usage(const char *errmsg)
{
  const char *args = "[-hvV] [-f file] [string]";
  if (errmsg) {
    fprintf(stderr, "%s: %s\n", progname, errmsg);
    fprintf(stderr, "Usage: %s %s\n", progname, args);
  }
  else {
    fprintf(stdout, "Xor standard input against a string\n");
    fprintf(stdout, "Usage: %s %s\n", progname ,args);
  }
  return errmsg ? FAILHARD : SUCCESS;
}

/** xor s in-place against x (repeated if necessary) */
void xorit(char *s, size_t slen, const char *x, size_t xlen)
{
  register char *sp;
  register const char *xp = x;
  register const char *xend = x + xlen;

  for (sp = s; slen > 0; slen--) {
    *sp++ ^= *xp++;
    if (xp >= xend) xp = x;
  }
}

/** alloc xbuf and load file fn into it, return #bytes, -1 on error*/
const char *xload(const char *fn, size_t *lenp)
{
  struct stat stbuf;
  char *xbuf;
  size_t xlen;
  ssize_t r;
  int fd;

  *lenp = 0;

  if (stat(fn, &stbuf) != 0) return NULL;

  xlen = /*(stbuf.st_size > 32767) ? 32767 :*/ stbuf.st_size;
  if (!(xbuf = malloc(xlen))) return NULL;

  if ((fd = open(fn, O_RDONLY)) < 0) {
    free(xbuf);
    return NULL;
  }

  if ((r = read(fd, xbuf, xlen)) < 0 || (size_t) r < xlen) {
    free(xbuf);
    return NULL;
  }

  (void) close(fd);

  *lenp = xlen;
  return xbuf;
}

/** format and write message to stderr, return given code */
int logup(int code, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "%s: ", progname);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  return code;
}
