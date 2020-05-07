
# Technical Notes

[Makefiles](#makefiles) •
[Compiler Flags](#compiler-flags) •
[Standard Headers](#standard-headers) •
[Manual Pages](#manual-pages) •
[Pitfalls](#pitfalls)

## Makefiles

Put `.POSIX:` on the first non-comment line to request
POSIX behaviour. You loose features but gain compatibility.

Conventional phony targets:

- all (build all; place first to make it the default target)
- clean (delete all generated files)
- install (install built artifacts)
- distclean (delete even more than *clean*)
- check or test (run the test suite)
- dist (create a package for distribution)

The *install* target, by convention, should use PREFIX and DESTDIR:

- PREFIX: should default to */usr/local*
- DESTDIR: for staged builds (install in a fake root)

Remarks

- Avoid recursive Makefiles (calling make on a subdirectory)
- Inference rules (like .c.o:) do not work with out-of-source
  builds (building .o files in another directory than the .c
  files). You would have to use explicit target rules like
  `obj/foo.o: src/foo.c` or pattern rules, but the latter
  are not part of the POSIX standard.
- POSIX make has only five internal macros:
  `$@`, `$%`, `$?`, `$<`, `$*`, where `$<` is defined
  only for inference rules, and `$?` is the list of only
  those prerequisites that are newer than the target.
  This means that with POSIX make a program's object files
  have to be listed twice: as prerequisites and in the
  build command.
- POSIX make does not scale well to large projects.
  Maintaining dependencies is error-prone.
  Makefiles for multi-directory source trees are cumbersome.
- .PHONY is a special target to declare “phony” targets
  (those that do not correspond with an actual file, such
  as `all` or `clean`). It is not part of POSIX, but
  seems to do no harm.

References

- A Tutorial on Portable Makefiles:
  <https://nullprogram.com/blog/2017/08/20/>
- Make in the POSIX.1 standard:
  <https://pubs.opengroup.org/onlinepubs/9699919799/utilities/make.html>


## Compiler Flags

- `-Wall` enable all warnings
- `-Wextra` enable some extra warnings (not included in `-Wall`)
- `-g3` include lots of debug information
- `-g0` include no debug information at all
- `-Os` optimize for code size (implies `-O2` but
  disables optimizations that could increase code size)
- `-std=c89` turn off GCC features incompatible with ISO C90
  (this is the same as `-ansi` and `-std=c90`)

Generally, options and arguments can be mixed. Sometimes, however,
parameter ordering is relevant:

The linker searches object files (.o) and libraries (libX.a or -lX)
in the order they are specified. For example, if *foo.c* references
*sin* from the math library, `gcc -lm foo.c` will fail, whereas
`gcc foo.c -lm` will work because gcc will find the unresolved
symbol *sin* from *foo.c* in the following library *libm.a*.
Note that this behaviour may be used to override functionality.

Also, the search paths for libraries and include files are built
in the order specified by the `-L` and `-I` options.


## Standard Headers

The functionality of the ANSI C Standard Library is declared
in the following headers. Other headers, like `<stdint.h>`
are not part of the ANSI C standard.

|Header|Remarks|
|------|-------|
|`<assert.h>` | assertions: the `assert()` macro|
|`<ctype.h>`  | character classification: `isalpha()` etc.|
|`<errno.h>`  | the `errno` expression and the `EXXX` constants|
|`<float.h>`  | various float and double constants|
|`<limits.h>` | size of integral types: `XXX_MIN` and `XXX_MAX`|
|`<locale.h>` | localisation: `setlocale()` and `localeconv()`|
|`<math.h>`   | math functions: `sin()`, `atan2()`, `sqrt()`, etc.|
|`<setjmp.h>` | non-local jumps: `setjmp()` and `longjmp()`|
|`<signal.h>` | signal processing: `raise()` and `signal()`|
|`<stdarg.h>` | access to variable argument lists: `va_list` etc.|
|`<stddef.h>` | `NULL`, `size_t`, `ptrdiff_t`, `wchar_t`, `offsetof()`|
|`<stdio.h>`  | input and output streams: `fopen()`, `printf()`, etc.|
|`<stdlib.h>` | memory allocation, random numbers, and varia|
|`<string.h>` | string and memory functions: `strxxx()` and `memxxx()`|
|`<time.h>`   | getting and formatting time: `time()`, `strftime()`, etc.|


## Manual Pages

Naming: *item*.*section*, e.g. tool.1 or func.3  
Optionally compress with GZip and name it foo.1.gz

Start with `.TH title section date source manual` where

- title: the program, e.g. `mklock`
- section: the manual section, here always `1` (User Commands)
- date: date of last non-trivial update, e.g. `April 2003`
- source: here always `minitools`
- manual: omit

Note that the date must be in double quotes, or blanks must be
escaped with a backslash.

Example: `.TH mklock 1 "April 2003" minitools`

The first section must be NAME and formatted exactly as follows:

```troff
.SH NAME
program \- one-line description
```

Note the backslash before the dash. This is required by mandb(8)
to create the database for apropos(1) and other commands.

Keep to the following section headings, in the given order:

- NAME (strictly required, see above)
- SYNOPSIS (required)
- DESCRIPTION (understandable description of the program)
- OPTIONS (may be skipped if easily visible from DESCRIPTION)
- EXIT STATUS (may be skipped if easily visible from DESCRIPTION)
- ENVIRONMENT (may be skipped if easily visible from DESCRIPTION)
- REMARKS (optional)
- BUGS (optional)
- AUTHOR (optional)
- SEE ALSO (optional)

On Linux, see **man**(7) and **man-pages**(7) in the manual.


## Pitfalls

It is tempting to name your logging function `log`, but this is
also the name of the logarithm function from the standard library
and the compiler may issue a warning even if you did not include
`<math.h>`.
