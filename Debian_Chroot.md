Howto Debian environment within SnakeOs.

Written by Robert Szabo.

# Introduction #

Very simple way to have Debian (and its huge number of packages).
Chroot is a "jailed environment" (embedded system within a linux) where you have debian libraries, packages and the whole deb environment within SnakeOS host.)

You only need to have an USB device attached. (No serial connection needed.)

Use ext2 or ext3 file system to extract files below.
(On FAT it will be extracted, but wont work!)

# Details #

  1. ssh to NAS - or putty from win
  1. `#`cd /usb/sda1
  1. Download chroot debian with wget:
  1. `#` wget http://snake-os.googlecode.com/files/debian_chroot.tgz
  1. backup your earlier debian config files if needed.
  1. `#` rm -R debian  **! This will erase previous debian directory!**
  1. `#`tar -xvf debian.tgz
  1. `#`mount -o bind /proc /usb/sda1/debian/proc
  1. `#`mount -o bind /dev /usb/sda1/debian/dev
  1. `#`mount -o bind /usb/sda1 /usb/sda1/debian/mnt/sda1    - this is not mandatory
  1. edit /usb/sda1/debian/etc/resolv.conf   -Set your name server, if you need.    Default is 192.168.1.1
  1. `#`chroot /usb/sda1/debian/

Now you are in chrooted debian shell. To exit debian use "exit" command or reboot.


## Debianctl ##

Steps eight to eleven can be automated with the debianctl package. Once installed go to Services->Debianctl and set the directory where Debian was extracted, the disks to mount and the services to start. From then on debianctl will automatically bring up Debian at system startup.

Current version: http://code.google.com/p/snake-os/downloads/detail?name=debianctl-0.3-1.opk

**Some useful - "must have"  thing:**


  * to have tons of debian packages use APT:)
  * apt-get install packagename
  * mc  - Midnight Commander - feature full file commander. Just run: mc
  * plowdown <--- downloading from hosting servicees like rapdishare and megaupload ( from http://code.google.com/p/plowshare/downloads/list. apt-get install plowdown.
  * screen <--- It allows you to run "virtual terminals" with the added benefit that closing the connection the process keep running, so no need to do 10 ssh sessions, or using nohup.  apt-get install screen.



  * Mount dev and proc, you can use fstab if yo whish.