/* errno - describe errno values */
/* History: ujr/2004-10-03 created */
/* Public domain */

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
    int code = atoi(*argv);
    printf("%s\n", strerror(code));
    return SUCCESS;
  }

  printf("Describe errno values.\n");
  printf("Usage: errno <code>\n");
  return FAILHARD;
}
