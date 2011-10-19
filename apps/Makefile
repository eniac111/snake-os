#
#	StarSemi BSP GPL source Makefile
# 	SNAKE OS source
#
# Build time:   2009/08/21
# Author:       Douglas Gazineu
#
#
CC?=arm-linux-gcc

APP_DIR=$(shell pwd)
TOOLS_DIR=$(APP_DIR)/../tools
ROOTFS_DIR=$(APP_DIR)/../rootfs
DEF_DIR=${ROOTFS_DIR}/default
TARGET_ROOT=${ROOTFS_DIR}/target
KERNELDIR=$(APP_DIR)/../kernels/linux
INSTALL_PATH?=${ROOTFS_DIR}/target
PARAM?= CC=arm-linux-gcc DESTDIR=$(TARGET_ROOT)

install: build_image to_install star_auto_install

to_install:
	# BUSYBOX
	make PREFIX=$(TARGET_ROOT) -j3 -C busybox install CROSS_COMPILE="arm-linux-"
	# SAMBA
	cp $(APP_DIR)/samba/source/bin/smbd ${TARGET_ROOT}/bin
	cp $(APP_DIR)/samba/source/bin/nmbd ${TARGET_ROOT}/bin
	# E2FSPROGS
	rm ${TARGET_ROOT}/sbin/mke2fs
	cp $(APP_DIR)/e2fsprogs/misc/mke2fs ${TARGET_ROOT}/sbin
	cp $(APP_DIR)/e2fsprogs/e2fsck/e2fsck ${TARGET_ROOT}/sbin
	ln -sf mke2fs ${TARGET_ROOT}/sbin/mkfs.ext2
	ln -sf mke2fs ${TARGET_ROOT}/sbin/mkfs.ext3
	# HASERL
	cp $(APP_DIR)/haserl/src/haserl ${TARGET_ROOT}/bin
	# P910ND
	cp $(APP_DIR)/p910nd/p910nd ${TARGET_ROOT}/bin
	# NTFS-3G
	cp $(APP_DIR)/ntfs-3g/src/.libs/ntfs-3g ${TARGET_ROOT}/bin
	cp -a $(APP_DIR)/ntfs-3g/libntfs-3g/.libs/libntfs-3g.so* ${TARGET_ROOT}/lib
	# INADYN
	cp $(APP_DIR)/inadyn/bin/linux/inadyn ${TARGET_ROOT}/bin
	# HD-IDLE
	#cp $(APP_DIR)/hd-idle/hd-idle ${TARGET_ROOT}/bin
	# NTPCLIENT
	cp $(APP_DIR)/ntpclient/ntpclient ${TARGET_ROOT}/bin
	# SMBENCRYPT
	cp $(APP_DIR)/smbencrypt/smbencrypt ${TARGET_ROOT}/bin
	# DROPBEAR
	cp $(APP_DIR)/dropbear/dropbearmulti ${TARGET_ROOT}/bin
	ln -s dropbearmulti ${TARGET_ROOT}/bin/dropbear
	ln -s dropbearmulti ${TARGET_ROOT}/bin/dropbearkey
	ln -s dropbearmulti ${TARGET_ROOT}/bin/dbclient
	ln -s dbclient ${TARGET_ROOT}/bin/ssh
	ln -s dropbearmulti ${TARGET_ROOT}/bin/scp
	# SFTP-SERVER
	cp $(APP_DIR)/openssh/sftp-server ${TARGET_ROOT}/bin
	# RSYNC
	cp $(APP_DIR)/rsync/rsync ${TARGET_ROOT}/bin
	# CURL
	cp -a $(APP_DIR)/curl/lib/.libs/libcurl.so* ${TARGET_ROOT}/lib
	#LIBEVENT
	cp -a $(APP_DIR)/libevent/.libs/libevent.so* ${TARGET_ROOT}/lib
	# TRANSMISSION
	make -C transmission install
	# VSFTPD
	cp $(APP_DIR)/vsftpd/vsftpd ${TARGET_ROOT}/bin
	# OPKG
	# IPKG
	cp $(APP_DIR)/opkg/src/opkg-cl ${TARGET_ROOT}/bin
	# Clean up SVN
	find ${TARGET_ROOT} -name ".svn" | xargs rm -rf

build_image: to_build_image star_auto_build

to_build_image:
	# BUSYBOX
	if [ ! -e busybox/.config ] ; then cp busybox/busybox_defconfig busybox/.config; make oldconfig -C  busybox; fi
	make PREFIX=$(TARGET_ROOT) CONFIG_PREFIX=$(TARGET_ROOT) -j3 -C busybox CROSS_COMPILE="arm-linux-"

	# SAMBA
	cd samba/source ; \
	ac_cv_sizeof_int=4 ac_cv_sizeof_long=4 ac_cv_sizeof_short=2 \
	samba_cv_FTRUNCATE_NEEDS_ROOT=no samba_cv_HAVE_BROKEN_FCNTL64_LOCKS=no \
	samba_cv_HAVE_BROKEN_GETGROUPS=no samba_cv_HAVE_BROKEN_READDIR=no \
	samba_cv_HAVE_FCNTL_LOCK=yes samba_cv_HAVE_FNMATCH=yes \
	samba_cv_HAVE_FTRUNCATE_EXTEND=no samba_cv_HAVE_IFACE_AIX=no \
	samba_cv_HAVE_IFACE_IFCONF=yes samba_cv_HAVE_IFACE_IFREQ=yes \
	samba_cv_HAVE_INO64_T=yes samba_cv_HAVE_IRIX_SPECIFIC_CAPABILITIES=no \
	samba_cv_HAVE_OFF64_T=yes samba_cv_HAVE_ROOT=yes \
	samba_cv_HAVE_SECURE_MKSTEMP=yes samba_cv_HAVE_SHARED_MMAP=yes \
	samba_cv_HAVE_STRUCT_FLOCK64=yes samba_cv_HAVE_SYSV_IPC=no \
	samba_cv_HAVE_TRUNCATED_SALT=no samba_cv_HAVE_UNION_SEMUN=no \
	samba_cv_HAVE_UNSIGNED_CHAR=yes samba_cv_NEED_SGI_SEMUN_HACK=no \
	samba_cv_REPLACE_INET_NTOA=no samba_cv_SIZEOF_INO_T=4 \
	samba_cv_SIZEOF_OFF_T=4 samba_cv_SYSCONF_SC_NGROUPS_MAX=yes \
	samba_cv_USE_SETRESUID=no samba_cv_USE_SETREUID=yes \
	samba_cv_USE_SETEUID=yes samba_cv_USE_SETUIDX=no \
	samba_cv_have_longlong=yes samba_cv_have_setresgid=no \
	samba_cv_have_setresuid=no samba_cv_HAVE_GETTIMEOFDAY_TZ=yes \
	CPPFLAGS="-D_GNU_SOURCE -DNDEBUG -DUSE_MMAP -DSHMEM_SIZE=524288 -Dfcntl=fcntl64" CFLAGS=" -pipe -Os -march=armv4t " \
	CC=arm-linux-gcc LD=arm-linux-ld AR=arm-linux-ar RANLIB=arm-linux-ranlib ./configure --host=arm-linux \
	--prefix=/bin --bindir=/bin --sbindir=/bin --sharedstatedir=/var --localstatedir=/var --sysconfdir=/etc --with-privatedir=/etc \
	--with-lockdir=/var --with-piddir=/var --libexecdir=/bin --with-logfilebase=/var --libdir=/etc --with-configdir=/etc \
	--without-syslog --disable-cups --with-sendfile-support --without-winbind
	make -j3 -C samba/source CFLAGS="-pipe -Os -march=armv4t"

	# E2FSPROGS
	cd e2fsprogs ; \
	CC=arm-linux-gcc LD=arm-linux-ld RANLIB=arm-linux-ranlib CROSS_COMPILE=arm-linux- ./configure --host=arm-linux --build=i686-pc-linux-gnu --enable-maintainer-mode=no  --enable-verbose-makecmds=no --enable-compression=no --enable-htree --enable-profile=no --enable-checker=no --enable-jbd-debug=no --enable-blkid-debug=no --disable-testio-debug  --disable-nls --without-libintl-prefix --without-libiconv-prefix
	make -j3 -C e2fsprogs CC=arm-linux-gcc CROSS_COMPILE="arm-linux-" CFLAGS="-pipe -Os -march=armv4t"

	# INADYN
	make -j3 -C inadyn CC=arm-linux-gcc CROSS_COMPILE="arm-linux-" CFLAGS="-pipe -Os -march=armv4t"

	# HD-IDLE
	#make -j3 -C hd-idle CC=arm-linux-gcc CROSS_COMPILE="arm-linux-" CFLAGS="-pipe -Os -march=armv4t"

	# HASERL
	cd haserl ; \
	./configure \
		--prefix=$(shell pwd) --build=arm-linux-uclibc --target=arm-linux-uclibc --host=i486-linux-gnu CFLAGS="-Os -frename-registers -march=armv4t" 
	make -j3 -C haserl CROSS_COMPILE="arm-linux-"

	# P910ND
	cd p910nd ; \
	sed -ri 's,#define\t\tLOCKFILE.*,#define\t\tLOCKFILE\t\t"/var/lock-p910%cd",g' p910nd.c ; \
	sed -ri 's,#define\t\tPIDFILE.*,#define\t\tPIDFILE\t\t"/var/run-p910%cd.pid",g' p910nd.c
	make -j3 -C p910nd CROSS="arm-linux-" CONFIGDIR="/etc" BINDIR="/bin"

	# NTFS-3G
	cd ntfs-3g ; \
	./configure \
		--build=arm-linux --target=arm-linux --host=i486-linux-gnu CFLAGS="-Os -frename-registers -march=armv4t"
	make -j3 -C ntfs-3g CROSS_COMPILE="arm-linux-"

	# NTPCLIENT
	make -j3 -C ntpclient CROSS_COMPILE="arm-linux-"

	# LIBDES (Required for smbencrypt) 
	make -j3 -C libdes CFLAGS="-Os -fomit-frame-pointer -march=armv4t"

	# SMBENCRYPT
	rm -f smbencrypt/des.h smbencrypt/libdes.a
	ln -s ../libdes/des.h smbencrypt/des.h
	ln -s ../libdes/libdes.a smbencrypt/libdes.a
	make -j3 -C smbencrypt CFLAGS="-Os -fomit-frame-pointer -march=armv4t"

	# DROPBEAR
	cd dropbear ; \
	CC=arm-linux-gcc LD=arm-linux-ld RANLIB=arm-linux-ranlib ./configure --host=arm-linux \
	--disable-zlib  --disable-largefile --disable-pam --disable-openpty --disable-syslog --disable-shadow --disable-lastlog --disable-utmp --disable-utmpx --disable-wtmp --disable-wtmpx --disable-loginfunc --disable-pututline --disable-pututxline
	make -j3 -C dropbear CC="arm-linux-gcc" PROGRAMS="dropbear dropbearkey dbclient scp" MULTI=1

	# OPENSSL  (Required for curl, transmission, sftp-server)
	cd openssl ; \
	./Configure dist 
	cd openssl ; \
	make CC="arm-linux-gcc" AR="arm-linux-ar r" RANLIB="arm-linux-ranlib"

	# OPENSSH  (sftp-server)
	cd openssh ; \
	./configure CC=arm-linux-gcc CPPFLAGS=-I~/include --build=arm-linux-uclibc --target=arm-linux-uclibc --host=i486-linux-gnu CFLAGS="-Os -frename-registers -march=armv4" --with-ssl-dir=../openssl --disable-etc-default-login --prefix=
	make -j3 -C openssh

	# RSYNC
	cd rsync ; \
	./configure CC=arm-linux-gcc --build=arm-linux-uclibc --target=arm-linux-uclibc --host=i486-linux-gnu CFLAGS="-Os -frename-registers -march=armv4" --disable-debug  --prefix=
	# rsync build often fails the first time
	make -j3 -C rsync || make -j3 -C rsync

	# CURL  (Required for transmission)
	cd curl ;\
	./configure \
		--prefix=$(shell pwd) --build=arm-linux-uclibc --target=arm-linux-uclibc --host=i486-linux-gnu CFLAGS="-Os -frename-registers -march=armv4" \
		--with-ssl=../openssl --with-gnutls \
		--disable-manual --without-random --disable-verbose --disable-debug  \
		--disable-cookies --disable-dict --disable-sspi
	make -j3 -C curl CROSS_COMPILE="arm-linux-"

	# OPKG
	cd opkg ;\
	./configure CC=arm-linux-gcc --build=arm-linux-uclibc --target=arm-linux-uclibc --host=arm-linux CFLAGS="-Os -frename-registers -march=armv4" --prefix=$(shell pwd) --with-opkglibdir=/opt --enable-static --disable-shared --disable-gpg --disable-curl --disable-shave
	make -j3 -C opkg

	#LIBEVENT (Required for transmission)
	cd libevent ; \
	./configure CC=arm-linux-gcc --build=arm-linux-uclibc --target=arm-linux-uclibc --host=i486-linux-gnu CFLAGS="-Os -frename-registers -march=armv4" --prefix=
	make -j3 -C libevent

	make -C transmission
#	# VSFTPD
	cd vsftpd ;\
	make CC=arm-linux-gcc LD=arm-linux-ld AR=arm-linux-ar RANLIB=arm-linux-ranlib CFLAGS="-pipe -Os -march=armv4t"

clean: star_auto_clean
	make -C busybox clean
	make -C samba/source clean
	make -C e2fsprogs clean
	make -C haserl clean
	make -C ntfs-3g clean
	make -C ntpclient clean
	make -C libdes clean
	make -C smbencrypt clean
	make -C dropbear clean
	make -C openssl clean
	make -C curl clean
#	make -C transmission clean
	make -C vsftpd clean
	make -C inadyn clean
	make -C opkg clean
	#make -C hd-idle clean

tags:
	find . -name *."[chS]" > filelist
	ctags -L filelist
	rm -f filelist



include star_auto.make
