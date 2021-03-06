
# Technical Notes

[Makefiles](#makefiles) •
[Compiler Flags](#compiler-flags) •
[Standard Headers](#standard-headers) •
[Manual Pages](#manual-pages) •
[Pitfalls](#pitfalls)

## Makefiles

The first target is the default target; it is usually
called `all` and is is built when *make* is invoked
without a target. Consider using only POSIX features
and declare so by putting `.POSIX:` on the first
non-comment line.

Terminology

- Rules: `target: prerequisites; commands`
- Target Rules: like the example above
- Inference Rules: e.g. `.c.o: ; $(CC) $(CFLAGS) -c -o $@ $<`
- Pattern Rules: e.g. `obj/%.o: src/%.c` (**not** POSIX)
- Macros: `FOO = bar` (note that `+=` and `?=` is **not** POSIX)
- Internal Macros, aka Automatic Variables: `$@`, `$*`, etc.
- Special Targets: e.g. `.POSIX:` and `.PHONY:` (**not** POSIX)
- Phony Target: nodes in the dependency graph that do not
  correspond to actual files, e.g. `all` or `clean`

Conventional phony targets:

- `all` (build all; place first to make it the default target)
- `clean` (delete all generated files)
- `install` (install built artifacts)
- `check` or test (run the test suite)
- `dist` (create a package for distribution)

By convention, the *install* target, should use PREFIX and DESTDIR,
where PREFIX should default to */usr/local* and DESTDIR is for staged
builds (install in a fake root).

Internal Macros

- `$@` current target (full name)
- `$*` current target (without extension)
- `$?` prerequisites *newer* than target
- `$^` *all* prerequisites (very useful, but **not** in POSIX)
- `$<` file that triggered the inference rule (eg, the .c file in .c.o:)
- `$%` file.o if current target is an archive library member `lib(file.o)`

Remarks

- Avoid recursive Makefiles (calling make on a subdirectory)
- Inference rules (like .c.o:) do not work with out-of-source
  builds (building .o files in another directory than the .c
  files). You would have to use explicit target rules like
  `obj/foo.o: src/foo.c` or pattern rules, but the latter
  are not part of the POSIX standard.
- POSIX make has only five internal macros:
  `$@`, `$%`, `$?`, `$<`, `$*`, where `$<` is defined only
  for inference rules, and `$?` is the list of *only those*
  prerequisites that are newer than the target!
- POSIX make does not scale well to large projects.
  Maintaining dependencies is error-prone.
  Makefiles for multi-directory source trees are cumbersome.
- `.PHONY:` is a special target to declare “phony” targets.
  It is not part of POSIX, but seems to do no harm.
- `.POSIX:` on the first non-comment line requests POSIX
  compliant behaviour. You loose features but gain compatibility.

See also

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

In a Makefile, `$?` represents only those prerequisites that are
newer than the target. For example, `foo: foo.o bar.o; cc -o $@ $?`
will not work as expected, because `$?` may not represent all
prerequisites! Instead, say `cc -o $@ $(OBJS)` after defining
`OBJS = foo.o bar.o` or use `$^` in the command (but beware
that the latter is **not** POSIX).
