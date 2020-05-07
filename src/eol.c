/* eol - convert end of lines
 * Usage: eol [-dmu] [files]
 * History:
 *   ujr/1999 created
 *   ujr/2002-07 added input and output buffering
 *   ujr/2006-01-21 revamped
 * License: GNU General Public License (GPL)
 */

#include <errno.h>    /* errno */
#include <stdio.h>
#include <stdlib.h>   /* abort */
#include <string.h>   /* strerror */

#include <fcntl.h>    /* O_RDONLY */
#include <unistd.h>   /* open, read, write, close */

#include "common.h"

/* Finite State Machine                   | A     | B     | C
 *                                   -----+-------+-------+-------
 * states: A (initial), B, C          c   | A/c   | A/c   | A/c
 *                                    CR  | C/EOL | A     | C/EOL
 * input alphabet:  c, CR, LF, EOF    LF  | B/EOL | B/EOL | A
 * output alphabet: c, EOL            EOF | STOP  | STOP  | STOP
 */

#define CR 13   /* carriage return */
#define LF 10   /* line feed */

int getch(int fd);
int putch(int ch);
int puteol(int type);
int convert(int fd, int type);

int getch(int fd)
{ /* return next byte or EOF on end-of-file or error */
  static char buf[BUFSIZ];
  static char *bufp = buf;
  static ssize_t n = 0;

  if (fd < 0) { /* flush buffer */
    n = 0;
    return 0;
  }
  if (n <= 0) { /* buffer empty */
      n = read(fd, buf, sizeof buf);
      if (n <= 0) return EOF;
      bufp = buf;
  }
  n -= 1;
  return (unsigned char) *bufp++;
}

int putch(int ch)
{ /* buffered writing to stdout */
  static char buf[BUFSIZ];
  static int n = 0;

  if (n == sizeof buf || (ch == EOF && n > 0)) {
      if (write(1, buf, n) != n) return EOF;
      n = 0;
      if (ch == EOF) return 0; /* ok */
  }
  buf[n++] = (char) ch;
  return 0; /* ok */
}

int puteol(int style)
{ /* write eol in given style to stdout */
  switch (style) {
    case 'd': putch(CR); /* FALLTHRU */
    case 'u': return putch(LF);
    case 'm': return putch(CR);
    default: abort();
  }
}

int convert(int fd, int style)
{
  int c, erc = 0;
  enum { A, B, C } state = A;

  errno = 0;
  (void) getch(-1);  /* prepare input buffer */
  while (1) {
    switch (c = getch(fd)) {
      case CR: switch (state) {
        case A: state = C;  /* FALLTHRU */
        case C: erc = puteol(style); break;
        case B: state = A; break; } break;
      case LF: switch (state) {
        case A: state = B;  /* FALLTHRU */
        case B: erc = puteol(style); break;
        case C: state = A; break; } break;
      case EOF: if (errno) return EOF;
        return putch(EOF); /* flush and done */
      default: state = A; erc = putch(c); break;
    }
    if (erc == EOF) return EOF;
  }
}

int main(int argc, char *argv[])
{
  const char *me;
  int style;

  if (argv && *argv) me = *argv++;
  else return 127; /* no arg0? */

  if (*argv && ((*argv)[0] == '-') && (*argv)[1]) {
    if ((*argv)[2]) style = '!';
    else style = (*argv)[1];
    argv++; argc--; /* shift */
  } else style = 'u'; /* default style is Unix */

  if ((style != 'u' && style != 'm' && style != 'd') || style == '?') {
    fprintf(stderr, "Usage: %s [-umd] [files]\n", me);
    fputs(" Convert to Unix (u, default), Mac (m), or DOS (d) style\n", stderr);
    fputs(" end-of-lines. Read from stdin (or files). Write to stdout.\n", stderr);
    return FAILHARD;
  }

  if (*argv) while (*argv) {
    int fd;
    if ((fd = open(*argv, O_RDONLY)) < 0) {
      fprintf(stderr, "%s: cannot open %s: %s\n", me, *argv, strerror(errno));
      return FAILSOFT;
    }
    if (convert(fd, style) == EOF) {
      fprintf(stderr, "%s: cannot convert %s: %s\n", me, *argv, strerror(errno));
      return FAILSOFT;
    }
    (void) close(fd);
    argv++; argc--; /* shift */
  }
  else if (convert(0, style) == EOF) {
    fprintf(stderr, "%s: cannot convert stdin: %s\n", me, strerror(errno));
    return FAILSOFT;
  }
  return SUCCESS;
}
