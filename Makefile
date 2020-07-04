# Request POSIX compliance
.POSIX:

CC = cc
CFLAGS = -Wall -Wextra -Os -g3 -std=c89
LDFLAGS = -s
LDLIBS = # -lm
PREFIX = /usr/local

BINDIR=$(DESTDIR)$(PREFIX)/bin
MANDIR=$(DESTDIR)$(PREFIX)/man

PROGS = eol errno float ipinfo isbnck legick mklock mkpwd signo uxtime xorit

all: $(PROGS)

install: all
	install -d $(BINDIR)
	install -d $(MANDIR)
	for p in $(PROGS); do install -m 755 bin/$$p $(BINDIR)/$$p; done
	for p in $(PROGS); do install -m 644 man/$$p.1 $(MANDIR)/man1/$$p.1; done

eol: bin/eol
errno: bin/errno
float: bin/float
ipinfo: bin/ipinfo
isbnck: bin/isbnck
legick: bin/legick
mklock: bin/mklock
mkpwd: bin/mkpwd
signo: bin/signo
uxtime: bin/uxtime
xorit: bin/xorit

bin/eol: src/eol.o
	$(CC) $(LDFLAGS) -o $@ src/eol.o $(LDLIBS)
bin/errno: src/errno.o
	$(CC) $(LDFLAGS) -o $@ src/errno.o $(LDLIBS)
bin/float: src/float.o
	$(CC) $(LDFLAGS) -o $@ src/float.o $(LDLIBS)
bin/ipinfo: src/ipinfo.o src/scanuint.o
	$(CC) $(LDFLAGS) -o $@ src/ipinfo.o src/scanuint.o $(LDLIBS)
bin/isbnck: src/isbnck.o
	$(CC) $(LDFLAGS) -o $@ src/isbnck.o $(LDLIBS)
bin/legick: src/legick.o
	$(CC) $(LDFLAGS) -o $@ src/legick.o $(LDLIBS)
bin/mklock: src/mklock.o
	$(CC) $(LDFLAGS) -o $@ src/mklock.o $(LDLIBS)
bin/mkpwd: src/mkpwd.o
	$(CC) $(LDFLAGS) -o $@ src/mkpwd.o $(LDLIBS)
bin/signo: src/signo.o
	$(CC) $(LDFLAGS) -o $@ src/signo.o $(LDLIBS)
bin/uxtime: src/uxtime.o src/scanlong.o
	$(CC) $(LDFLAGS) -o $@ src/uxtime.o src/scanlong.o $(LDLIBS)
bin/xorit: src/xorit.o
	$(CC) $(LDFLAGS) -o $@ src/xorit.o $(LDLIBS)

src/eol.o: src/eol.c src/common.h
src/errno.o: src/errno.c src/common.h
src/ipinfo.o: src/ipinfo.c src/common.h
src/isbnck.o: src/isbnck.c src/common.h
src/legick.o: src/legick.c src/common.h
src/mklock.o: src/mklock.c src/common.h
src/mkpwd.o: src/mkpwd.c src/common.h
src/signo.o: src/signo.c src/common.h
src/uxtime.o: src/uxtime.c src/common.h
src/xorit.o: src/xorit.c src/common.h

src/scanlong.o: src/scanlong.c src/common.h
src/scanuint.o: src/scanuint.c src/common.h

# Like the built-in inference rule, but write
# output to same dir as input, not to current dir.
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f bin/* src/*.o

tgz: clean
	(cd ..; tar chzvf minitools-`date +%Y%m%d`.tgz minitools)

.PHONY: all install clean tgz
