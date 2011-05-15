#!/bin/sh

if [ "x$1" = "x" ] ; then echo Invalid param... ; exit 1 ; fi
if [ "x$2" = "x" ] ; then echo Invalid param... ; exit 1 ; fi


#TOOLCHAIN=/prj/tools/arm-linux-uclibc-3.4.6.swfp;
TOOLCHAIN=$1;
TARGET=$2;

echo TOOLCHAIN=$TOOLCHAIN
echo TARGET=$TARGET


find rootfs/target -type f | egrep -v "\.htm[~]{0,1}|target/dev|\.(o|ko|gif|script|css|xml|sh|js)|target/usr/etc" | xargs readelf -a 2>/dev/null | grep NEEDED|sed -n 's/.*Shared library: \[\(.*\)\]/\1/p'|sort|uniq > log;

rm log.1 -fv;for x in `cat log` ; do if [ -f $TOOLCHAIN/lib/$x ] ;then echo $TOOLCHAIN/lib/$x >> log.1;else echo $TOOLCHAIN/lib/$x not found;fi;done;


rm log.2 -fv;for link in `cat log.1` ; do readlink $link -f 1>>log.2;done;


rm log.3 -fv; cp log.2 log.3 -fv;for link in `find $TOOLCHAIN/lib/ -maxdepth 1 -mindepth 1 -type l` ; do grep "`basename \`readlink $link -f\``" log.2 1>/dev/null 2>&1&& echo $link >> log.3;done;


mkdir $TARGET/lib -p;cp `cat log.3` $TARGET/lib -afv;

rm log log.* -fv;
