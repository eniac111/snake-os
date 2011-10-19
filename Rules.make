#
# This file include all rules to make the BSP.
#


#
# variable for ramdisk size setting
#   ramdisk size = BLOCK_SIZE * COUNTS
#            = 512 * 32k = 16 MB
#
UCLIBC_BLOCK_SIZE := 512    # Block size for ramdisk (uclibc)
UCLIBC_COUNTS := 24k        # Block size for ramdisk (uclibc)

TOPDIR = $(PWD)

OUTPUT_PATH := $(TOPDIR)/output
TOOLS_DIR := $(TOPDIR)/tools

###################################################
#
# Default setting path
#
###################################################
ROOTFS_DIR := $(TOPDIR)/rootfs
TARGET_ROOT := $(ROOTFS_DIR)/target
DEF_DIR := $(ROOTFS_DIR)/default
DEF_ROOT_DIR := $(ROOTFS_DIR)/default/root

###################################################
#
# Kernel folder
#
###################################################
KER_DIR := $(TOPDIR)/kernels/linux
BOOTLOADER_DIR := $(TOPDIR)/boot/u-boot-1.1.4

ROOTFS_SIZE=0x2a0000
#ROOTFS_SIZE=0xe00000

RAMDISK_NAME := $(OUTPUT_PATH)/ramdisk.img
JFFS2_NAME := $(OUTPUT_PATH)/jffs2.img
JFFS2_IMAGE := $(OUTPUT_PATH)/jffs2Image
ROOTFS_JFFS2 := $(OUTPUT_PATH)/rootfs.jffs2
USB_DEV := $(TOOLS_DIR)/usb_device.bin
FULL_IMAGE := $(OUTPUT_PATH)/fullimage

###################################################
#
# application source tree sub-folder
#
###################################################
APP_DIR := $(TOPDIR)/apps
BUSYBOX_DIR := $(APP_DIR)/busybox
SAMBA_DIR := $(APP_DIR)/samba-3.0.22-star
ROOTFS_DIR := $(TOPDIR)/rootfs
TARGET_ROOT := $(ROOTFS_DIR)/target
DEF_ETC_DIR := $(ROOTFS_DIR)/default/etc
DEF_USR_DIR := $(ROOTFS_DIR)/default/usr

TOOLCHAIN?=$(TOPDIR)/tools/arm-uclibc-3.4.6
PATH+=:$(TOOLCHAIN)/bin
export PATH
