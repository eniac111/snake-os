# Unbricking procedure for WLX-652 and NS-K330 #

**READ**: This unbricking procedure is intended to work when you have a serial connection AND your NAS enter at least into bootloader. It will rewrite the default stock firmware back to your WLX-652 or NS-K330 NAS.
If your NAS is unresponsible with the serial connection, chances are that you will have to [rewrite the flash](UnbrickAgestarNsb3ast.md).


To download the files, right-click on the links and use "Save as".

  1. Get a serial interface connected through a RS232-TTL converter. The serial parameters are 38400-N-1. The board have some holes in which the pins can be connected, next to the processor.
  1. Set your computer IP address to 192.168.0.100, and have tftp server running (I do like tftpd32 in Windows or tftpd-hpa in Linux) with the file [recovery.8132](http://snake-os.googlecode.com/files/recovery.8132) in the root directory.
  1. Start the system and interrupt the boot process with any key to get in the "Star Equuleus #" uboot prompt.
  1. Use these commands:
```
  setenv ipaddr 192.168.0.10
  setenv serverip 192.168.0.100
  tftpboot 0x1000000 recovery.8132
  go 0x1000000
```
> > The recovery image should boot. You must be able to use the NAS prompt now.
  1. Download and unpack [fw\_def\_mtd.zip](http://snake-os.googlecode.com/files/fw_def_mtd.zip). Copy the files fw\_def\_mtd1\_config.bin , fw\_def\_mtd2\_kernel.bin and fw\_def\_mtd3\_initrd-and-web.bin to an usb drive (prefer fat, ext2 or ext3) and connect it to the NAS. It will mount automatically.
  1. Type the command "mount", to check that the pendrive got mounted on /usb/sda1. We will assume this mount point for the remaining of procedure, change it accordingly, if necessary.
  1. Write the files back to the correct flash partitions with the following commands (the kernel and initrd-web will take some time):
```
/bin/dd of=/dev/mtdblock1 if=/usb/sda1/fw_def_mtd1_config.bin
/bin/dd of=/dev/mtdblock2 if=/usb/sda1/fw_def_mtd2_kernel.bin
/bin/dd of=/dev/mtdblock3 if=/usb/sda1/fw_def_mtd3_initrd-and-web.bin
```
  1. Remove and reconnect the power cable. Remember that your NAS will return to the default factory settings.


NOTE: Depending on the working state of your NAS, this procedure may not work. Feel free to ask for info.

Gazineu