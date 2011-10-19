########################################################################
#
# This script is for generating Snake OS image, include linux kernel image and armboot.
#
########################################################################
#!/bin/sh

TOP_DIR=$PWD
KER_DIR=$TOP_DIR/kernels/linux
PREVIOUS_OWNER=$(ls -ld rootfs/default | awk '{ print $3":"$4}')
CURRDATE=$(date +%Y%m%d)
CURRVER=V1.3.2
CURRENTVER="SNAKE OS ${CURRVER}"
CURRENTRELEASE="${CURRENTVER} (${CURRDATE})"

echo ${CURRENTVER} > $TOP_DIR/etcfs/etc/version
echo ${CURRDATE} > $TOP_DIR/etcfs/etc/build

echo ' ' > $TOP_DIR/etcfs/etc/motd
echo '                          __' >> $TOP_DIR/etcfs/etc/motd
echo '        (\   .-.   .-.   /_")' >> $TOP_DIR/etcfs/etc/motd
echo '         \\_//^\\_//^\\_//' >> $TOP_DIR/etcfs/etc/motd
echo '    jgs   `"`   `"`   `"` ' >> $TOP_DIR/etcfs/etc/motd
echo ' ' >> $TOP_DIR/etcfs/etc/motd
echo '----------------------------------' >> $TOP_DIR/etcfs/etc/motd
echo 'Star Nas Altered Killer Edition OS' >> $TOP_DIR/etcfs/etc/motd
echo ' ' >> $TOP_DIR/etcfs/etc/motd
echo ${CURRENTRELEASE} >> $TOP_DIR/etcfs/etc/motd
echo '----------------------------------' >> $TOP_DIR/etcfs/etc/motd


chown -R root:root rootfs/default
chown -R root:root etcfs

find . -name "*~" | xargs rm


cd $TOP_DIR/etcfs/etc
tar --exclude-vcs -cvzf $TOP_DIR/rootfs/default/usr/share/snake/default.tar.gz *
cd $TOP_DIR


mk_kn(){
	echo =================================================
	echo ===================   $1 JFFS2   ===================
	echo =================================================
	cp configs/str$1_defconfig_jffs2 $KER_DIR/.config -fv
	#make -C $KER_DIR/ clean 
	make oldconfig_kernel || exit 1
	echo =================================================
	echo ===================   $1 JFFS2   ===================
	echo =================================================
	make kernel || exit 1
	mv output/jffs2Image output/jffs2Image.$1
	mv output/zImage output/zImage.$1
	echo =================================================
	echo ===================   $1 bootpImage   ===================
	echo =================================================
	cp configs/str$1_defconfig_ramimage $KER_DIR/.config -fv
	#make -C $KER_DIR/ clean
	make oldconfig_kernel || exit 1
	echo =================================================
	echo ===================   $1 bootpImage   ===================
	echo =================================================
	make kernel || exit 1
	mv output/bootpImage output/bootpImage.$1
}
mk_fs(){
	make root_fs || exit 1

}
#copy an arbitrary config and build kernel for latter mk_fs
cp configs/str8132_defconfig_jffs2 $KER_DIR/.config -fv
make pre-build-kernel
mk_fs

mk_kn 8132

#make bootloader



#Create full compatible firmware image for updates from legacy/vendor firmware
CONFIG_SIZE=0x20000
KERNEL_SIZE=0xF0000
# Used only to check size....
ROOTFS_SIZE=0x2A0000

BOOTIMG=snake_os/default/mtd0_bootloader.bin

CONFIG=snake_os/release/mtd1_config.bin
KERNEL=snake_os/release/mtd2_kernel.bin
ROOTFS=snake_os/release/mtd3_rootfs.bin

RESULTDIR=snake_os/release

FIRMWARE=snakeos-${CURRVER}-${CURRDATE}-from-original.bin
RESULT=${RESULTDIR}/${FIRMWARE}

KERNEL_IN=output/zImage.8132
ROOTFS_IN=output/rootfs.jffs2
ROOTFS_SUMMED=output/jffs2.img.sum
ROOTFS_SUMMED=output/jffs2.img

# pad kernel to partition size (960K)
KSIZE=$(stat -c%s "$KERNEL_IN")
if [ ${KSIZE} -le $(printf "%d" ${KERNEL_SIZE}) ] ; then 
	echo "KERNEL WILL FIT: ${KSIZE} bytes LESS THAN $(printf "%d" ${KERNEL_SIZE}) bytes"
	dd if=/dev/zero of=${KERNEL} bs=$(printf "%d" ${KERNEL_SIZE}) count=1
	dd if=${KERNEL_IN} of=${KERNEL} conv=notrunc
else
	echo "KERNEL TOO BIG: ${KSIZE} bytes BIGGER THAN $(printf "%d" ${KERNEL_SIZE}) bytes - ABORTING"
	exit
fi

# copy rootfs to destination (Check early dgazineu...)
RSIZE=$(stat -c%s "$ROOTFS_SUMMED")
if [ ${RSIZE} -le $(printf "%d" ${ROOTFS_SIZE}) ] ; then
	echo "ROOTFS FIT: ${RSIZE} bytes LESS THAN $(printf "%d" ${ROOTFS_SIZE}) bytes"
else
	echo "ROOTFS TOO BIG: ${RSIZE} bytes BIGGER THAN $(printf "%d" ${ROOTFS_SIZE}) bytes - ABORTING"
	exit
fi
cp ${ROOTFS_IN} ${ROOTFS}

# CONFIG SETUP
# create empty config partition
dd if=/dev/zero of=${CONFIG} bs=$(printf "%d" ${CONFIG_SIZE}) count=1
# make the config file contain an empty filesystem
mke2fs -i 1024 -F ${CONFIG}

sync
# create initial firmware without checksum
cat ${BOOTIMG} ${CONFIG} ${KERNEL} ${ROOTFS} > ${RESULT}

# write zeroes to end of firmware
dd if=/dev/zero of=${RESULT} count=1 bs=16 seek=258047

# calculate checksum
md5sum ${RESULT} | cut -d ' ' -f 1 > ${RESULT}.md5

# turn checksum into binary
# LC_ALL=C fixes unicode issue with awk
cat ${RESULT}.md5 | sed 's/../& /g' |tr '[a-z]' '[A-z]' |sed 's/ / p /g' |sed 's/$/ p/'|awk '{print "16i "$0}'|dc |tr ' ' '\n' | LC_ALL=C awk '{printf("%c",$0)}' | dd of=${RESULT}.md5tail bs=16 count=1

# strip empty tail from firmware
dd if=${RESULT} of=${RESULT}.notail bs=16 count=258047

# concatenate firmware and checksum tail
cat ${RESULT}.notail ${RESULT}.md5tail > ${RESULT}


# Remove temp files
rm -f ${RESULT}.notail
rm -f ${RESULT}.md5tail
rm -f ${RESULT}.md5

cd ${RESULTDIR}
md5sum ${FIRMWARE} > ${FIRMWARE}.md5
chmod a+rw *
cd -

#Create firmware image without boot partition for updates from snake OS

FIRMWARE=snakeos-${CURRVER}-${CURRDATE}-from-snake.bin
RESULT=${RESULTDIR}/${FIRMWARE}

#sync
# create initial firmware without checksum
cat ${CONFIG} ${KERNEL} ${ROOTFS} > ${RESULT}

# write zeroes to end of firmware
dd if=/dev/zero of=${RESULT} count=1 bs=16 seek=241663

# calculate checksum
md5sum ${RESULT} | cut -d ' ' -f 1 > ${RESULT}.md5

# turn checksum into binary
# LC_ALL=C fixes unicode issue with awk 
cat ${RESULT}.md5 | sed 's/../& /g' |tr '[a-z]' '[A-z]' |sed 's/ / p /g' |sed 's/$/ p/'|awk '{print "16i "$0}'|dc |tr ' ' '\n' | LC_ALL=C awk '{printf("%c",$0)}' | dd of=${RESULT}.md5tail bs=16 count=1

# strip empty tail from firmware
dd if=${RESULT} of=${RESULT}.notail bs=16 count=241663

# concatenate firmware and checksum tail
cat ${RESULT}.notail ${RESULT}.md5tail > ${RESULT}

# Remove temp files
rm -f ${RESULT}.notail
rm -f ${RESULT}.md5tail
rm -f ${RESULT}.md5

cd ${RESULTDIR}
md5sum ${FIRMWARE} > ${FIRMWARE}.md5
chmod a+rw *
cd -

chown -R ${PREVIOUS_OWNER} rootfs/default
chown -R ${PREVIOUS_OWNER} etcfs

