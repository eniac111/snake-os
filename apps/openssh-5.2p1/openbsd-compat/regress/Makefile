# $Id: Makefile.in,v 1.4 2006/08/19 09:12:14 dtucker Exp $

sysconfdir=${prefix}/etc
piddir=/var/run
srcdir=.
top_srcdir=../..


CC=arm-linux-gcc
LD=arm-linux-gcc
CFLAGS=-Os -frename-registers -march=armv4 -Wall -Wpointer-arith -Wuninitialized -Wsign-compare -Wformat-security -fno-builtin-memset -std=gnu99 
CPPFLAGS=-I. -I.. -I$(srcdir) -I$(srcdir)/.. -I/home/dwmcqueen/Projects/snake-os-8-22/snake-os/apps/openssh/../openssl/include -I~/include -DHAVE_CONFIG_H
EXEEXT=
LIBCOMPAT=../libopenbsd-compat.a
LIBS=-lcrypto -lutil -lz  -lcrypt -lresolv -lresolv
LDFLAGS=-L/home/dwmcqueen/Projects/snake-os-8-22/snake-os/apps/openssh/../openssl  $(LIBCOMPAT)

TESTPROGS=closefromtest$(EXEEXT) snprintftest$(EXEEXT) strduptest$(EXEEXT) \
	strtonumtest$(EXEEXT)

all:	t-exec ${OTHERTESTS}

%$(EXEEXT):	%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $< $(LIBCOMPAT) $(LIBS)

t-exec:	$(TESTPROGS)
	@echo running compat regress tests
	@for TEST in ""$?; do \
		echo "run test $${TEST}" ... 1>&2; \
		./$${TEST}$(EXEEXT) || exit $$? ; \
	done
	@echo finished compat regress tests

clean:
	rm -f *.o *.a core $(TESTPROGS) valid.out

distclean: clean
	rm -f Makefile *~
