/* uxtime - convert Unix time to readable local time */
/* History: ujr/2008-02-06 created */
/* Public domain */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"

void usage(const char *s);

const char *me;

int main(int argc, char **argv)
{
  time_t unixtime;
  struct tm *tmp;
  char tz[128];

  (void) argc; /* unused */
  if (argv && *argv) me = *argv++;
  else return 127; /* no arg0? */

  if (*argv) {
    if (scanlong(*argv++, &unixtime) == 0)
      usage("invalid option");
  } else unixtime = time(0); /* system time */

  if (*argv) usage("too many arguments");

  if ((tmp = localtime(&unixtime)) == 0) {
    fprintf(stderr, "%s: localtime(3) failed: %s\n", me, strerror(errno));
    return FAILSOFT;
  }

  if (!strftime(tz, sizeof(tz), "%Z", tmp)) tz[0] = '\0';

  printf("Unix time %ld is %d-%02d-%02d %02d:%02d:%02d %s\n",
         unixtime, 1900+tmp->tm_year, 1+tmp->tm_mon, tmp->tm_mday,
         tmp->tm_hour, tmp->tm_min, tmp->tm_sec, tz);

  return SUCCESS;
}

void usage(const char *s)
{
  if (s) fprintf(stderr, "%s: %s\n", me, s);
  fprintf(stderr, "Usage: %s [unixtime]\n", me);
  exit(FAILHARD);
}

