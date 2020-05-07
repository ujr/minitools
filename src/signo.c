/* signo - describe signal values */
/* History: ujr/2005-05-23 created */
/* Public domain */

#define _POSIX_C_SOURCE 200809L  /* for strsignal */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

int main(int argc, char **argv)
{
  (void) argc; /* unused */
  if (argv == 0 || *argv++ == 0)
    return FAILHARD;  /* no arg0? */

  if (*argv && !argv[1]) {
    int num = atoi(*argv);
    printf("%s\n", strsignal(num));
    return SUCCESS;
  }

  printf("Describe signal numbers\n");
  printf("Usage: signo <number>\n");
  return FAILHARD;
}
