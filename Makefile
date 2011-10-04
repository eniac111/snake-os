#
#		StarSemi BSP Makefile
#		
#
CC=arm-linux-gcc
include Rules.make
include $(KER_DIR)/.config # for CONFIG_BLK_DEV_RAM checking

all: clean-output pre-build-kernel root_fs kernel print-output-result

rootfs: root_fs

root_fs: make_rootfs slim_rootfs gen_rootfs

make_rootfs:
	arm-linux-gcc -v 
	# remove target directory's all files
	rm -rf $(TARGET_ROOT)
	mkdir $(TARGET_ROOT) -p
	# build base file system
	cp -a $(DEF_DIR)/* $(TARGET_ROOT)/ 
	tar -jxvf $(ROOTFS_DIR)/rootfs_dev.tar.bz2 -C $(TARGET_ROOT)
	
	make -j3 modules -C $(KER_DIR)
	make -j3 modules_install -C $(KER_DIR) INSTALL_MOD_PATH=$(TARGET_ROOT)

	#make -j3 -C $(KER_DIR) CONFIG_NTFS_FS=m CONFIG_NTFS_DEBUG=n CONFIG_NTFS_RW=y M=fs/ntfs
	#mkdir -p $(TARGET_ROOT)/lib/modules/2.6.16-gazineu/kernel/fs/ntfs
	#cp -a $(KER_DIR)/fs/ntfs/ntfs.ko $(TARGET_ROOT)/lib/modules/2.6.16-gazineu/kernel/fs/ntfs
	
	make -C $(APP_DIR) install CC=${CC}
	
	for i in `seq 3`; do ./cp_lib_to_target.sh ${TOOLCHAIN} $(TARGET_ROOT); done

	# the transmission package requires libm
	cp ${TOOLCHAIN}/lib/libm-*.so $(TARGET_ROOT)/lib -afv
	cp ${TOOLCHAIN}/lib/libm.so* $(TARGET_ROOT)/lib -afv

	#cp ${TOOLCHAIN}/lib/libnsl* $(TARGET_ROOT)/lib -afv
	#cp ${TOOLCHAIN}/lib/libresolv* $(TARGET_ROOT)/lib -afv
	#cp ${TOOLCHAIN}/lib/librt* $(TARGET_ROOT)/lib -afv
	#cp ${TOOLCHAIN}/lib/libutil* $(TARGET_ROOT)/lib -afv

slim_rootfs:
	-find $(TARGET_ROOT) -name \.svn -exec rm -rf {} \;	
	-find $(TARGET_ROOT) -name *\.a -exec rm -rf {} \;
	-find $(TARGET_ROOT) -name *\.la -exec rm -rf {} \;
	-find $(TARGET_ROOT) -name *\.h -exec rm -rf {} \;
	-find $(TARGET_ROOT) -name *\Makefile* -exec rm -rf {} \;
	-find rootfs/target -type f | egrep -v "\.htm[~]{0,1}|target/dev|\.(o|ko|gif|script|css|xml|sh|js)|target/usr/etc" | xargs arm-linux-strip -v
	-target=man;rm $(TARGET_ROOT)/$${target} $(TARGET_ROOT)/share/$${target} $(TARGET_ROOT)/usr/local/$${target} $(TARGET_ROOT)/usr/share/$${target} $(TARGET_ROOT)/usr/local/share/$${target} -rvf
	-target=doc;rm $(TARGET_ROOT)/$${target} $(TARGET_ROOT)/share/$${target} $(TARGET_ROOT)/usr/local/$${target} $(TARGET_ROOT)/usr/share/$${target} $(TARGET_ROOT)/usr/local/share/$${target} -rvf
# Added by gazineu
	-upx -7 rootfs/target/bin/ntpclient rootfs/target/bin/smbd rootfs/target/bin/nmbd rootfs/target/bin/dropbearmulti rootfs/target/bin/sftp-server rootfs/target/bin/rsync rootfs/target/bin/smbencrypt rootfs/target/bin/ntfs-3g rootfs/target/bin/p910nd rootfs/target/sbin/mke2fs rootfs/target/sbin/e2fsck rootfs/target/bin/vsftpd rootfs/target/bin/inadyn rootfs/target/bin/opkg-cl
	#rootfs/target/bin/hd-idle

gen_rootfs:
	@echo ==================================
	du -sk $(TARGET_ROOT)
	@echo ==================================
#
# JFFS2
#
	sudo mkfs.jffs2 -d $(TARGET_ROOT) -o $(JFFS2_NAME) -p ${ROOTFS_SIZE} -X zlib 
# Added by gazineu
	sudo $(TOOLS_DIR)/sumtool -n -i $(JFFS2_NAME) -o $(JFFS2_NAME).sum 
	dd if=/dev/zero of=$(ROOTFS_JFFS2) bs=$(shell printf "%d" ${ROOTFS_SIZE}) count=1
	# dd if=$(JFFS2_NAME).sum of=$(ROOTFS_JFFS2) conv=notrunc
	dd if=$(JFFS2_NAME) of=$(ROOTFS_JFFS2) conv=notrunc

#
# Generate both fs no matter what is configured
#
#
# Ramdisk
#
#ifeq ($(strip $(CONFIG_BLK_DEV_RAM)), y)
	-umount $(TARGET_ROOT)_MOUNT
	-rm $(TARGET_ROOT)_MOUNT -rf
	
	mkdir -p $(TARGET_ROOT)_MOUNT
	dd if=/dev/zero of=$(RAMDISK_NAME) bs=$(UCLIBC_BLOCK_SIZE) count=$(UCLIBC_COUNTS)
	#losetup /dev/loop0 $(RAMDISK_NAME)
	#mkfs.ext2 -i 1024 /dev/loop0
	#losetup -d /dev/loop0
	mkfs.ext2 -i 1024 $(RAMDISK_NAME) -F
	rm -rf $(TARGET_ROOT)_MOUNT/lost*
	mount -o loop $(RAMDISK_NAME) $(TARGET_ROOT)_MOUNT
	cp -a $(TARGET_ROOT)/* $(TARGET_ROOT)_MOUNT
	umount $(TARGET_ROOT)_MOUNT
	gzip -f9 $(RAMDISK_NAME)
	rmdir $(TARGET_ROOT)_MOUNT
	
	@echo ==================================
	@ls output -al;
	@echo ==================================

pre-build-kernel:
	make -j3 zImage -C $(KER_DIR)

make_kernel_modules:
	make -j3 modules -C $(KER_DIR) 
	make -j3 modules_install -C $(KER_DIR) INSTALL_MOD_PATH=$(TARGET_ROOT)


kernel:
ifeq ($(strip $(CONFIG_BLK_DEV_RAM)), y)
	cd $(KER_DIR) ;\
	echo $(shell pwd);\
	rm -fv arch/arm/boot/bootp/*.o arch/arm/boot/bootp/bootp arch/arm/boot/Image arch/arm/boot/zImage arch/arm/boot/bootpImage; \
	make -j3 bootpImage INITRD=$(RAMDISK_NAME).gz && cp -vf arch/arm/boot/bootpImage $(OUTPUT_PATH)
else
	cd $(KER_DIR) ;\
	echo $(shell pwd);\
	rm -fv arch/arm/boot/bootp/*.o arch/arm/boot/bootp/bootp arch/arm/boot/Image arch/arm/boot/zImage arch/arm/boot/bootpImage; \
	make -j3 zImage && cp -vf arch/arm/boot/zImage $(OUTPUT_PATH)
	cd $(TOPDIR)
	dd if=$(OUTPUT_PATH)/zImage of=$(JFFS2_IMAGE)
	#dd if=$(JFFS2_NAME) of=$(JFFS2_IMAGE) obs=1M seek=2
	dd if=$(ROOTFS_JFFS2) of=$(JFFS2_IMAGE) obs=1M seek=2
	#rm -f $(JFFS2_NAME) $(OUTPUT_PATH)/zImage
endif
	
#full_image:
#	dd if=$(USB_DEV) of=$(FULL_IMAGE)
#ifeq ($(strip $(CONFIG_BLK_DEV_RAM)), y)
#	dd if=$(OUTPUT_PATH)/bootpImage of=$(FULL_IMAGE) obs=64K seek=1
#else
#	dd if=$(JFFS2_IMAGE) of=$(FULL_IMAGE) obs=64K seek=1
#endif

clean_kernel:
	make -C $(KER_DIR) clean

oldconfig_kernel:
	make -C $(KER_DIR) oldconfig

bootloader:
#	make -C $(BOOTLOADER_DIR) star_equuleus_8181_config
#	make -C $(BOOTLOADER_DIR) 
#	cp -v $(BOOTLOADER_DIR)/u-boot.bin $(TOPDIR)/output/u-boot.equuleus.8181.bin
#	make -C $(BOOTLOADER_DIR) star_equuleus_8182_config
#	make -C $(BOOTLOADER_DIR) 
#	cp -v $(BOOTLOADER_DIR)/u-boot.bin $(TOPDIR)/output/u-boot.equuleus.8182.bin
	make -C $(BOOTLOADER_DIR) star_equuleus_8132_config
	make -C $(BOOTLOADER_DIR) 
	cp -v $(BOOTLOADER_DIR)/u-boot.bin $(TOPDIR)/output/u-boot.equuleus.8132.bin
#	make -C $(BOOTLOADER_DIR) star_equuleus_8133_config
#	make -C $(BOOTLOADER_DIR) 
#	cp -v $(BOOTLOADER_DIR)/u-boot.bin $(TOPDIR)/output/u-boot.equuleus.8133.bin

clean_bootloader:
	make -C $(BOOTLOADER_DIR) clean

distclean: clean
	rm -rf .config ./config/*.tmp

clean-output:
	rm -rf $(TARGET_ROOT)/* $(OUTPUT_PATH)/* 

clean: clean-output
	make -C $(APP_DIR) clean

#test:
#	-find ./ -name \.svn -exec rm -rf {} \;

test:
	./cp_lib_to_target.sh ${TOOLCHAIN} $(TARGET_ROOT)

print-output-result:
	@echo ==================================
	@ls output -al;
	@echo ==================================

