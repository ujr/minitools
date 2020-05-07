
# Minitools

Collection of my simple miniature tools, some written
to solve a practical problem, some just out of curiosity.
They were all developed and used on Linux systems a long
time ago. The markdown files I added only recently.

## The Tools

- **eol** - convert end-of-line styles (Unix, DOS, Mac)
- **errno** - describe errno values (wraps `strerror`)
- **ipinfo** - show information about an IPv4 address
- **isbnck** - verify the ISBN check sum
- **legick** - check/compute a Swiss student "Leginummer"
- **mklock** - create exclusive lock file (for use in scripts)
- **mkpwd** - generate random initial passwords (use in scripts)
- **signo** - describe signal numbers (wraps `strsignal`)
- **uxtime** - convert Unix time to human readable local time
- **xorit** - xor standard input against a string

All tools exit with status **0** if successful,
**111** if a temporary error occurred, and
**127** if a permanent error occurred, unless
stated otherwise in the tool's manual page.
All tools come with a manual page.

## Remarks

Technical notes are in the [NOTES.md](./NOTES.md) file.

### About newlines

The **eol** tool converts between the three most common newline
(or end-of-line) conventions: CR+LF (DOS), LF (Unix), CR (Macintosh).
The logical operation “newline” involved two physical motions
in the old teletype machines: return the carriage to the left
margin, and move down to the next line. This found its way into
the ASCII code, which provides those two operations: CR (ASCII 13)
for Carriage Return and LF (ASCII 10) for Line Feed. Apparently,
the Unix community chose just LF and the early Mac designers
decided for CR. By the way, in C, `\r` is CR and `\n` is LF.

### About ISBN

The International Standard Book Number (ISBN) was devised
in the 1960ies and standardised in 1972. Its purpose is to
uniquely identify a book and thus facilitate the logistics
of publishers and book sellers. ISBNs must be purchased.
Its use is optional. The original ISBN was a 9 digit number,
followed by a check digit for error detection. When in the
early 2000s it became difficult for new publishers to acquire
number blocks, the new ISBN-13 system was designed, which
is compatible with the European Article Number (EAN).
The **isbnck** tool only knows the old ISBN-10 system.

### About lock files

The **mklock** tool provides a simple mechanism for mutual
exclusion in scripts: atomically test if a file exists and
create it if not; if it does exist, signal an error. If this
“test-and-set” operation succeeded, you're alone. When done,
remove the lock file, either explicitly or as the result of
a `trap "rm -f my.lock" 0`. For example:

```sh
mklock /var/run/foo.lock
echo "Doing something alone..."
rm -f /var/run/foo.lock
```

### About IPv4 addresses

An IP (Internet Protocol) version 4 address is a 32-bit number.
For routing purposes, the first *n* bits are taken to signify
the network, and the remaining 32-*n* bits are the host within
that network. The least address within a network identifies
the network itself, and the largest address within a network
serves as the broadcast address (send to all hosts in the
network). The number *n* is given either implicitly (derived
from the first few bits of the IP address; this is known
as class-based routing), or explicily by augmenting the
IP address with a netmask or a "slash value". IP addresses
are usually written in "dotted decimal notation" like
192.168.15.3; if a "slash value" is provided, it looks
like 192.168.15.3/24 (which means that 192.168.15.0 is
the network and 3 is the host within this network).
The **ipinfo** tool shows all these values; for example,
given 192.168.25.108/27, the tool reports:

```text
CIDR 27 host address, private
Address:    192.168.25.108   11000000.10101000.00011001.011/01100
Netmask:    255.255.255.224  11111111.11111111.11111111.111/00000  27
Hostmask:   0.0.0.31         00000000.00000000.00000000.000/11111   5
MaxHosts:   30               --------+--------+--------+---/-----
Network:    192.168.25.96    11000000.10101000.00011001.011/00000 min
Broadcast:  192.168.25.127   11000000.10101000.00011001.011/11111 max
```

The manual page for **ipinfo** may contain additional information.
Also see [RFC 1519](https://www.ietf.org/rfc/rfc1519.txt) about CIDR
and [RFC 1918](https://www.ietf.org/rfc/rfc1918.txt) about private
internet addresses. For information about an IP address's ownership,
try **whois** or one of the online services, e.g.
[whois.arin.net](https://whois.arin.net/).
Nowadays, IPv6 is widely deployed. An IPv6 address is 128 bits.

### About Unix times

Unix maintains (at least) calendar time and CPU time.
The **uxtime** utility shows calendar time, not CPU time.

Calendar time is the number of seconds since the Epoch,
which is 00:00:00 on January 1, 1970, Universal Time
Coordinated (UTC, formely Greenwich Mean Time, GMT).
It is returned by the function `time()` and stored
in variables of type `time_t`.

CPU time is measured in clock ticks, which occur at the
constant but system-dependent frequency `CLOCKS_PER_SEC`.
There is no epoch, only differences of clock values
are meaningful. Clock values are returned by `clock()`
and have type `clock_t`.

All functions, types, and definitions above are part
of the `<time.h>` ANSI C standard header. Note that
clock ticks may overflow `clock_t` and thus are not
suitable for reliable timings. System functions may
be used, at the price of less portability.


## License

Collection of my simple and miniature tools.  
Copyright (C) 1999-2008 Urs Jakob Ruetschi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

![GitHub](https://img.shields.io/github/license/ujr/minitools)
