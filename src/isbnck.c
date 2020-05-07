/* ISBN mod 11 checksum tester
 * Usage: isbnck <ISBN>
 * History: ujr/2002-09-25 created
 * License: GNU General Public License (GPL)
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void die(char *msg) { fprintf(stderr, "%s\n", msg); exit(99); }

int main(int argc, char *argv[])
{
  char *p = argv[1];  /* ptr into ISBN arg */
  int c = 0, i = 10;  /* cksum, multiplier */

  (void) argc; /* unused */
  if (!p) die("missing argument");

  while (i > 0 && *p) {
    if (isdigit(*p)) c += i-- * (*p-'0');
    else if (i==1 && (*p=='X' || *p=='x')) c += 10, --i;
    p++;
  }
  if (i > 0) die("malformed ISBN");
  if (*p && !isspace(*p)) die("malformed ISBN");

  c %= 11;  /* now c should be zero or the given ISBN is invalid */

  fprintf(stderr, "%s\n", c ? "invalid" : "ok");
  return c ? 1 : 0;
}
