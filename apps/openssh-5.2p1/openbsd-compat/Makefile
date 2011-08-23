# $Id: Makefile.in,v 1.43 2008/06/08 17:32:29 dtucker Exp $

sysconfdir=${prefix}/etc
piddir=/var/run
srcdir=.
top_srcdir=..


CC=arm-linux-gcc
LD=arm-linux-gcc
CFLAGS=-Os -frename-registers -march=armv4 -Wall -Wpointer-arith -Wuninitialized -Wsign-compare -Wformat-security -fno-builtin-memset -std=gnu99 
CPPFLAGS=-I. -I.. -I$(srcdir) -I$(srcdir)/.. -I/home/dwmcqueen/Projects/snake-os-8-22/snake-os/apps/openssh/../openssl/include -I~/include -DHAVE_CONFIG_H
LIBS=-lcrypto -lutil -lz  -lcrypt -lresolv -lresolv
AR=/usr/bin/ar
RANLIB=ranlib
INSTALL=/usr/bin/install -c
LDFLAGS=-L. -L/home/dwmcqueen/Projects/snake-os-8-22/snake-os/apps/openssh/../openssl 

OPENBSD=base64.o basename.o bindresvport.o daemon.o dirname.o fmt_scaled.o getcwd.o getgrouplist.o getopt.o getrrsetbyname.o glob.o inet_aton.o inet_ntoa.o inet_ntop.o mktemp.o readpassphrase.o realpath.o rresvport.o setenv.o setproctitle.o sha2.o sigact.o strlcat.o strlcpy.o strmode.o strsep.o strtonum.o strtoll.o strtoul.o vis.o

COMPAT=bsd-arc4random.o bsd-asprintf.o bsd-closefrom.o bsd-cray.o bsd-cygwin_util.o bsd-getpeereid.o bsd-misc.o bsd-nextstep.o bsd-openpty.o bsd-poll.o bsd-snprintf.o bsd-statvfs.o bsd-waitpid.o fake-rfc2553.o openssl-compat.o xmmap.o xcrypt.o

PORTS=port-aix.o port-irix.o port-linux.o port-solaris.o port-tun.o port-uw.o

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

all: libopenbsd-compat.a

$(COMPAT): ../config.h
$(OPENBSD): ../config.h
$(PORTS): ../config.h

libopenbsd-compat.a:  $(COMPAT) $(OPENBSD) $(PORTS)
	$(AR) rv $@ $(COMPAT) $(OPENBSD) $(PORTS)
	$(RANLIB) $@

clean:
	rm -f *.o *.a core 

distclean: clean
	rm -f Makefile *~
