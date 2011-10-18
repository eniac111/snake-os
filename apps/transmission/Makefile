
build:
	cd transmission ;\
	./configure \
		--host=arm-linux CC=arm-linux-gcc CXX=arm-linux-g++ --prefix=${INSTALL_PATH} CFLAGS="-pipe -Os -s -march=armv4" CXXFLAGS="-pipe -Os -s -march=armv4" \
        LIBCURL_LIBS="-L${PWD}/apps/curl/lib/.libs -lcurl"  \
        LIBCURL_CFLAGS=-I${PWD}/apps/curl/include \
        OPENSSL_LIBS="-L${PWD}/apps/openssl ${PWD}/apps/openssl/libcrypto.a" \
        OPENSSL_CFLAGS=-I${PWD}/apps/openssl/include \
        LIBEVENT_LIBS="-L${PWD}/apps/libevent/.libs ${PWD}/apps/libevent/.libs/libevent.a"  \
        LIBEVENT_CFLAGS=-I${PWD}/apps/libevent/include \
		--host=i686-linux-gnu --target=arm-linux-gnu --enable-static=yes --enable-shared=no \
		--disable-dependency-tracking  \
		--disable-libtool-lock  \
		--disable-gtk        \
		--disable-libnotify  \
		--disable-nls        \
		--disable-cli        \
		--disable-mac        \
		--enable-daemon      \
		--enable-lightweight   
	make PREFIX=$(TARGET_ROOT) CONFIG_PREFIX=$(TARGET_ROOT) -j3 -C transmission CROSS_COMPILE="arm-linux-"
	
	cp transmission/daemon/transmission-daemon data/bin/
	cp -a transmission/web data/share/transmission/	
	find data/ -name ".svn" | xargs rm -rf

	cd data; tar --exclude-vcs -zcf ../data.tar.gz *
	chown -R root:root data/*
	
	cd control; tar --exclude-vcs -zcf ../control.tar.gz *
	chown -R root:root control/*
	chmod +x control/prerm control/postinst
	
	ar -r transmission.opk data.tar.gz control.tar.gz debian-binary

install: build
	cp -fv transmission.opk ../../output/
	
clean:	
	make -C transmission clean
