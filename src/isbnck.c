/* ISBN checksum tester
 * Usage: isbnck <ISBN>
 * History:
 *   ujr/2002-09-25 created
 *   ujr/2020-06-30 rewrite to do ISBN-10 and ISBN-13
 * License: GNU General Public License (GPL)
 *
 * The 10-digit ISBN was published in 1970.
 * Since Jan 2007 the ISBN is 13 digits long.
 *
 * Example ISBN-10:  ISBN 3-519-03402-6
 * Example ISBN-13:  ISBN 978-3-519-03402-5
 *
 * Check 10: check digit x10 must be chosen such that
 * 10 x_1 + 9 x_2 + 8 x_3 + ... + 2 x_9 + x_10 = 0 (mod 11)
 *
 * Check 13: check digit x13 must be chosen such that
 * x_1 + 3 x_2 + x_3 + 3 x_4 + ... + 3 x_12 + x_13 = 0 (mod 10)
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int scanisbn(const char *s, int digits[13]);
static int check10(int const digits[10]);
static int check13(int const digits[10]);

static size_t getline(char *line, size_t max);
static void copyline();

void die(char *msg) { fprintf(stderr, "%s\n", msg); exit(99); }

int main(int argc, char *argv[])
{
  char line[128];
  int digits[13];
  int n, c, i;
  int totalchecked = 0, checkfailed = 0, malformed = 0;

  if (argc > 1) { /* process args */
    for (i = 1; i < argc; i++) {
      totalchecked += 1;
      switch (scanisbn(argv[i], digits)) {
      case 10: c = check10(digits);
        if (c == 0) fprintf(stderr, "checksum passed: %s\n", argv[i]);
        else {
          int cc = c == 11 ? 'X' : c + '0' - 1;
          fprintf(stderr, "checksum failed: %s (should be %c)\n", argv[i], cc);
          checkfailed += 1;
        }
        break;
      case 13: c = check13(digits);
        if (c == 0) fprintf(stderr, "checksum passed: %s\n", argv[i]);
        else {
          int cc = c + '0' - 1;
          fprintf(stderr, "checksum failed: %s (should be %c)\n", argv[i], cc);
          checkfailed += 1;
        }
        break;
      default:
        fprintf(stderr, "malformed ISBN: %s\n", argv[i]);
        malformed += 1;
        break;
      }
    }
  }
  else { /* process stdin */
    while ((n = getline(line, sizeof line)) > 0) {
      totalchecked += 1;
      switch (scanisbn(line, digits)) {
      case 10: c = check10(digits);
        if (c == 0) printf("OK ");
        else { printf("!%c ", c==11 ? 'X' : c+'0'-1); checkfailed += 1; }
        break;
      case 13: c = check13(digits);
        if (c == 0) printf("OK ");
        else { printf("!%c ", c+'0'-1); checkfailed += 1; }
        break;
      default:
        printf("!! "); /* malformed */
        malformed += 1;
        break;
      }
      printf("%s", line);
      if (line[n-1] != '\n') copyline();
    }
  }

  fprintf(stderr, "(%d passed, %d failed, %d malformed)\n",
          totalchecked - checkfailed - malformed, checkfailed, malformed);

  return checkfailed + malformed > 0 ? 1 : 0;
}

/** Compute/verify ISBN-10 check sum;
    return 0 if ok; return 1+cd if nok */
static int
check10(int const digits[10])
{
/* Accumulate check sum without multiplication:
    x1
    x1 + x2
    x1 + x2 + x3
    :    :    :
    x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9
   -------------------------------------------
   9x1 +8x2 +7x3 +6x4 +5x5 +4x6 +3x7 +2x8 +1x9
*/

  int i, acc, sum, c, d;
  for (i = acc = sum = 0; i < 9; i++) { /* eg. 3-519-03402-(6) */
    acc += digits[i];    /* 3  8  9 18 18 21  25  25  27  (33) */
    sum += acc;          /* 3 11 20 38 56 77 102 127 154 (187) */
  }

  d = digits[9];
  sum += acc;            /* sum = 10x1 + 9x2 + 8x3 + ... 2x9 */

  c = (11 - sum % 11) % 11;

  if (c==d) return 0;  /* check digit valid */
  return 1+c;          /* invald; return 1+correct digit */
}

/** Compute/verify ISBN-13 check sum;
    return 0 if ok; return 1+cd if nok */
static int
check13(int const digits[13])
{
  int i, s, c, d;

  for (i=s=0; i<12; i+=2) {
    s += digits[i];
    s += 3*digits[i+1];
  }

  c = 10 - (s % 10); /* 1..10 */
  if (c==10) c = 0;  /* 0..9, required check digit */

  d = digits[12];    /* actual check digit */

  return c==d ? 0 : 1+c;
}

/** Scan an ISBN, return 10 or 13 if ISBN-10 or -13, 0 if malformed */
static int
scanisbn(const char *s, int digits[13])
{
  const char *p = s;
  int i, c;

  if (!s) return 0;

  /* Skip leading blank and optional "ISBN" */
  while (*p == ' ' || *p == '\t') ++p;
  if (strncmp("ISBN", p, 4) == 0) p += 4;
  while (*p == ' ' || *p == '\t') ++p;

  /* Get the digits, optionally separated by blank or dash */
  for (i=0; i<13; i++, p++) {
    if (*p == ' ' || *p == '-') ++p;
    if ((c = (unsigned char) (*p - '0')) > 9) break;
    digits[i] = c;
  }

  /* An ISBN-10 may end in 'X' */
  if (i == 9 && (*p == 'X' || *p == 'x')) {
    digits[i++] = 10; p++;
  }

  /* Valid if we got exactly 10 or 13 digits */
  if ((i==10 || i==13) && !isalnum(*p)) return i;

  return 0; /* malformed ISBN */
}

static size_t
getline(char *line, size_t max)
{
  return fgets(line, max, stdin) ? strlen(line) : 0;
}


static void
copyline()
{
  int c;
  while ((c = getchar()) != EOF && c != '\n')
    putchar(c);
  if (c != EOF)
    putchar(c);
}
