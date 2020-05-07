/* mklock - create lock file for shell scripts
 * History: ujr/2003-02-22 created
 * Usage: mklock [-hqQV] lockfile [stuff]
 * Caveat: mklock uses O_EXCL, which is unreliable on NFS volumes!
 * License: GNU General Public License (GPL)
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

static char id[] = "mklock by ujr/2003-02-22\n";

int identity(void);
int usage(const char *errmsg);
int cantlock(int code, const char *s);
char *progname = "mklock";  /* default */
int quiet = 0;

int main(int argc, char **argv)
{
  const char *fn;
  int c, fd;
  ssize_t r;

  (void) argc; /* unused */
  if (*argv && **argv) progname = *argv;
  while (*++argv && (**argv == '-')) {
    while ((c = *++argv[0])) switch (c) {
      case 'h': return usage(0);
      case 'q': quiet = 1; break;
      case 'Q': quiet = 0; break;
      case 'V': return identity();
      case '-': argv++; goto endargs;
      default: return usage("invalid option");
    }
  }
endargs:
  fn = *argv;
  if (!fn) return usage("missing lockfile argument");

  if ((fd=open(fn, O_CREAT | O_EXCL | O_WRONLY, 0600)) < 0) {
    switch (errno) {
    case EEXIST: return cantlock(111, fn);
    case EACCES: return cantlock(127, fn);
    default: return cantlock(127, fn);
    }
  }

  /* write remining args to lock file, don't care about errors */
  r = 0;
  while (*++argv && r >= 0)
    r = write(fd, *argv, strlen(*argv));
  (void) close(fd);

  return SUCCESS;
}

int identity(void)
{
  return fputs(id, stdout) >= 0 ? SUCCESS : FAILHARD;
}

int usage(const char *errmsg)
{
  const char *args = "[-hqQV] lockfile [stuff]";

  if (errmsg) {
    fprintf(stderr, "%s: %s\n", progname, errmsg);
    fprintf(stderr, "Usage: %s %s\n", progname, args);
  }
  else {
    fprintf(stdout, "Try create lock file; return 0 iff successful\n");
    fprintf(stdout, "Usage: %s %s\n", progname, args);
    fprintf(stdout, "Options: -q quiet, -Q default, -V version and exit\n");
  }

  return errmsg ? FAILHARD : SUCCESS;
}

int cantlock(int code, const char *fn)
{
  if (!quiet)
  {
    const char *msg = errno == EEXIST ? "already locked" : strerror(errno);
    fprintf(stderr, "cannot acquire lock %s: %s\n", fn, msg);
  }
  return code;
}
