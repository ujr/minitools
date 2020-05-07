/* Show IPv4 address information.
 * Usage: ipinfo [-hV] address[/n] [mask]
 * History:
 *   ujr/2001-10-25 started
 *   ujr/2002-09-03 added command line switches for selective output
 *   ujr/2005-04-10 changed -v to -V and my related standard behaviour
 *   ujr/2007-11-30 major rewrite, renamed ipcalc to ipinfo.
 * Note: works internally with a hostmask (not netmask).
 * License: GNU General Public License (GPL).
 */

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

static char id[] = "This is ipinfo, version 1.0\n"
                   "Copyright (c) 2001-2007 by UJR\n";

static int identity(void);
static int usage(const char *errmsg);

static char getclass(uint32 ip);
static uint32 getmask(char class);
static int ispriv(uint32 ip);
static int mask2bits(uint32 mask);

static int scanip4(const char *s, uint32 *ip);
static const char *fmtip2(uint32 ip, int slash);
static const char *fmtsep(int slash);

static char *me = "ipinfo";

int main(int argc, char *argv[])
{
  const char *addrstr;
  const char *maskstr;
  uint32 addr, mask;
  int c, i, j, slash = -1; /* no slash */
  char ipclass = 0; /* classless */
  uint32 nwaddr, bcaddr, count;

  (void) argc; /* unused */
  if (argv && *argv) me = *argv;
  else return FAILHARD; /* no arg0? */

  while (*++argv && (**argv == '-')) {
    while ((c = *++argv[0])) switch (c) {
      case 'h': return usage(0);
      case 'V': return identity();
      case '-': argv++; goto endargs;
      default: return usage("invalid option");
    }
  }
endargs:

  if (*argv) addrstr = *argv++;
  else return usage("no address specified");

  if (*argv) maskstr = *argv++;
  else maskstr = 0; /* not specified */

  if (*argv) return usage("too many arguments");

  /* Parse address and opt slash value */

  i = scanip4(addrstr, &addr);
  if (i && addrstr[i] == '/') {
    i += 1; /* skip slash */
    j = scanuint(addrstr+i, (unsigned*)&slash);
    if (j) i += j; else i = 0;
  }
  else slash = -1; /* not specified */

  if (i == 0 || addrstr[i] != '\0')
    return usage("invalid address");
  if (slash > 32)
    return usage("slash value out of range 0..32");

  if (slash < 0) { /* negative means classes */
    ipclass = getclass(addr);
    mask = getmask(ipclass);
  }
  else {
    ipclass = 0; /* classless address (CIDR) */
    mask = (1 << (32-slash))-1;
  }

  /* Parse optional net/host mask or assume class */

  if (maskstr) {
    if (slash >= 0)
      return usage("too many arguments");
    i = scanip4(maskstr, &mask);
    if ((i == 0) || (maskstr[i] != '\0'))
      return usage("invalid mask");
    ipclass = 0; /* explicit mask means no class (CIDR) */

    /* If the given mask appears to be a netmask, convert
     * to hostmask (Cisco wildcard) by bitwise negation.
     */
    if (mask & 0x80000000) mask = ~mask;
  }

  /* Compute the "slash-value" corresponding to mask;
   * check if valid mask (ie, mask+1 is power of two).
   */
  if (slash < 0) /* compute netbits from mask */
    if ((slash = 32 - mask2bits(mask)) > 32)
      return usage("invalid mask");

  /* Compute other interesting values and output */

  bcaddr = addr | mask;
  mask = ~mask; /* convert to netmask */
  nwaddr = addr & mask;
  count = (1 << (32 - slash)) - 2;

  if (ipclass) printf("Class %c", ipclass);
  else printf("CIDR %d", slash);
  printf(" %s address", (nwaddr == addr) ? "network"
      : (bcaddr == addr) ? "broadcast" : "host");
  if (ispriv(addr)) printf(", private");
  printf("\n");

  printf("Address:    %s\n", fmtip2(addr, slash));
  printf("Netmask:    %s %3d\n", fmtip2(mask, slash), slash);
  printf("Hostmask:   %s %3d\n", fmtip2(~mask, slash), 32-slash);
  printf("MaxHosts:   %-17ld%s\n", (long) count, fmtsep(slash));
  printf("Network:    %s min\n", fmtip2(nwaddr, slash));
  printf("Broadcast:  %s max\n\n", fmtip2(bcaddr, slash));

  return SUCCESS;
}

static int identity(void)
{
  return fputs(id, stdout) >= 0 ? SUCCESS : FAILSOFT;
}

static int usage(const char *errmsg)
{
  FILE *fp = errmsg ? stderr : stdout;
  if (errmsg) fprintf(fp, "%s: %s\n", me, errmsg);
  else fprintf(fp, "Show IPv4 address information\n");
  fprintf(fp, "Usage: %s [-V] address/n\n", me);
  fprintf(fp, "   or: %s [-V] address [mask]\n", me);
  return errmsg ? FAILHARD : SUCCESS;
}

/** Return the class for an IPv4 address */
static char getclass(uint32 ip)
{
  int first = (ip >> 24) & 255; /* first octet */
  if ((first & 0x80) == 0x00) return 'A';
  if ((first & 0xc0) == 0x80) return 'B';
  if ((first & 0xe0) == 0xc0) return 'C';
  if ((first & 0xf0) == 0xe0) return 'D';
  return 'E'; /* class E, reserved */
}

/** Return the hostmask for a class */
static uint32 getmask(char class)
{
  if (class == 'A') return 0x00ffffff;
  if (class == 'B') return 0x0000ffff;
  if (class == 'C') return 0x000000ff;
  return 0; /* no hostmask for other classes */
}

/** Return true iff the given IP address is a private address,
 * ie, in one of the networks 10/8 or 172.16/12 or 192.168/16.
 */
static int ispriv(uint32 ip)
{
  int x = ip >> 16; /* first two octets decide */
  return ((x & 0xff00) == (uint32) 10*256) ||
         ((x & 0xfff0) == (uint32) 172*256+16) ||
         ((x & 0xffff) == (uint32) 192*256+168);
}

/** Count trailing one bits (mask to bit count).
 * By the way, the inverse is mask=(1<<bits)-1. */
static int mask2bits(uint32 mask)
{
  int n;

  if (mask == 0) return 32; /* 2^32 = 0 (mod wordsize) */
  if (mask & (mask+1)) return -1; /* mask is not 2^n-1 */

  /* count 1-bits from the right */
  for (n=0; mask & 1; n++) mask >>= 1;

  return n;
}

/** Scan an IPv4 addr in dotted decimal notation, return #chars scanned */
static int scanip4(const char *s, uint32 *ip)
{
  const char *p = s;
  uint32 value;
  unsigned octet;
  int n;

  if ((n = scanuint(p, &octet)) == 0 || (octet > 255)) return 0;
  value = (uint32) octet;  p += n;

  if (*p++ != '.') return 0;

  if ((n = scanuint(p, &octet)) == 0 || (octet > 255)) return 0;
  value = 256*value + octet;  p += n;

  if (*p++ != '.') return 0;

  if ((n = scanuint(p, &octet)) == 0 || (octet > 255)) return 0;
  value = 256*value + octet;  p += n;

  if (*p++ != '.') return 0;

  if ((n = scanuint(p, &octet)) == 0 || (octet > 255)) return 0;
  value = 256*value + octet;  p += n;

  if (ip) *ip = value;
  return p - s; /* #chars scanned */
}

static const char *fmtip2(uint32 ip, int slash)
{
  int i, j;
  static char buf[80];

  i = sprintf(buf, "%d.%d.%d.%d", /* dotted decimal */
    (int) (ip>>24)&255, (int) (ip>>16)&255, (int) (ip>>8)&255, (int) ip&255);
  for (; i < 17; i++) buf[i] = ' ';
  slash = 32 - slash;
  for (j=31; j>=0; j--) { /* binary representation */
    buf[i++] = (ip & (1<<j)) ? '1' : '0';
    if (j == slash) buf[i++] = '/';
    else if (j == 24 || j == 16 || j == 8) buf[i++] = '.';
  }
  buf[i] = '\0';

  return (const char *) buf;
}

static const char *fmtsep(int slash)
{
  int i, j = 0;
  static char buf[40];

  for (i = 1; i <= 32; i++) {
    buf[j++] = '-';
    if (i == slash) buf[j++] = '/';
    else if (i == 24 || i == 16 || i == 8) buf[j++] = '+';
  }
  buf[j] = '\0';

  return (const char *) buf;
}
