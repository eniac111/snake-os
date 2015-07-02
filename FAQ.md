# Introduction #

This is the Open Source SnakeOS FAQ.

SnakeOS is the best firmware for NS-K330 / WLX-652 and similar small NAS devices.

This device usually have 2 USB and one RJ45 Network connector.

_Written by Robert Szabo_


# Questions and answers #

**Q: Where is the firmware /binary ?**

> Answer:

> Use download section.
> Actual one is:  http://code.google.com/p/snake-os/downloads/detail?name=snakeos-V1.1.0-20100418-from-snake.zip&can=2&q=

Be aware: **from snake and from original stock firm is different! Many people reporting that they upgraded with wrong firmware and then it is bricked.**

snakeos-V1.2.0-20100621-from-original.zip means upgrade this firmware if you have original stock firmware.
snakeos-V1.2.0-20100621-from-snake.zip use this if you have a previous version of snake already installed.

**Q: Can I upgrade my NS-K330 or WLX-652 with SnakeOS ?**

> Answer: Yes. Definitely. Recommended. More stable, and more functions.

**Q: Where is user documentation?**

> Answer1:    You can read it in download section.

> Authors are: Douglas Gazineu,  Jarko, José Maciel,  Miguel Moreto



**Q: Where to ask questions?**

> Answer: Read the forum and PAGES on the right side of

> http://groups.google.com/group/dealextreme-nas-/topics?hl=en

**Q: Device lost all connectivity, no answer for ping, ssh, http, ftp. What can I do?**

> Answer1:  Hard Reset it.

> NS-K330 has a reset button. Push it while powering on, with the reset button
> held in for 45 seconds (counted to 60 just in case).
> After it showed up under the default IP (or at least the one assigned by the
> DHCP server), by the MAC address.
> Some people reported that router must be power off also during NAS reset.
> Some other people suggest to directly connect NAS to PC via the ethernet cable, not via router.

**Q: Serial connect**

> If hard reset was not enough - device not respond yet.

> Answer2: If you are not lucky with hard reset, second chance you are able to serial connect to device and telnet via the RS232-TTL converter direct connect.
> You also have to connect nas with the rj45 cable for TFTP access.

> Picture and description here:
> http://groups.google.com/group/dealextreme-nas-/web/serial-connection-how-to
> or
> http://groups.google.com/group/dealextreme-nas-/web/serialconnectionhowto-ns-k330?hl=en

**Q: FLASHing -**

> Serial connect was not allowed - device not respond/boot. Seems bricked. :(

> Answer3: If you not enough lucky with serial connection either, next chance to write
> and replace Flash. Flash contains the boot, and root partitions.
> (BIOS and Operation system together.)

> There is a smart (and cheap) flash writer using parallel port + 4 resistor + 1 condensator.
> See detailes here http://rayer.ic.cz/elektro/spipgm.htm

> With this you can reprogram your flash. (Or ask guys from the forum here to do for you.)

  1. flash write,
> http://groups.google.com/group/dealextreme-nas-/browse_thread/thread/08df34f7df625832?hl=en

> 2. And replace flash
> http://groups.google.com/group/dealextreme-nas-/web/nas-832-flash-chip-relocation-tutorial

> Most of the user don't need flash replace. Most of them have local network related problems. (ex.:  router don't give new IP address) So try first to shut down all network elements before try to replace flash.

**Q: Default user and password for the web interface ?**

> Answer: admin/snake


**Q: Compile - dependences. What are known external dependencies for the source compile?**

  1. upx-nrv, libcurl3-openssl-dev, liblzo2-2, build-essential, kernel-headers, mtd-tools, ncurses-dev, autoconf

**Q: Compile. Hey guys it is great, How can I make my own one?**

> Answer:
  1. Download source, (from Subversion at http://code.google.com/p/snake-os/source)
  1. sudo su  - became root

  * **Optional** To adjust your kernel config:
    1. Backup the "configs" folder
    1. cp configs/str8132\_defconfig\_jffs2 kernels/linux/.config
    1. cd kernels/linux folder
    1. Export PATH to point to arm compiler. export PATH="path-to-source"/tools/arm-uclibc-3.4.6/bin:$PATH
    1. make clean && make menuconfig
    1. Adjust your kernel settings.
    1. Copy back kernels/linux/.config file to configs/str8132\_defconfig\_jffs2 file.

  1. Execute "sh ./mk\_snakeos\_image.sh" in the top level directory

> If the script aborts with any error messages you probably need some packages installed to have development environment. See "Compile - dependencies" above for a list of required packages.

> mk\_snakeos\_image.sh script was made by Douglas which compile the kernel and make root file system for JFFS, compress the binaries with UPX (which makes on the fly decompiled binary)

> Compiled firmware is here:

> snake\_os/release/ and tftp-capable image on output/

> You could upgrade your NAS with your newly created custom firmware, under the http://nasaddress/ under system/firmware menu

**Q: FTP Write permissions problem for anonymous.**

> I have activated FTP. In FTP with anonymous only with write permssion.

> Through samba i can read and write but by FTP i can only read. I haven't got write permissions even though I have configured it in Snake OS.

**Answer:**

> VSFTP root folder must have 755 but any sub folder for ftp write should have 777.

> This can be done by using ssh to access the NAS and

> `chmod 777/usb/PATH-TO-FOLDER`


**Q: How to copy files from one mounted partition to another mounted partition?**
> Answer:
> Connect with SSH and run the command below. It will copy the files of source\_folder to dest\_folder and log the output to log.txt (place outside the involved folders).

> `nohup cp -av /usb/diska/source_folder  /usb/diskb/dest_folder > /usb/diska/log.txt `

> This way, you can even disconnect the SSH client and the transfer will continue.

**Q: Where is the source code / SDK of the STAR STR8131.**

> Answer:
> LSDK sources from cnusers forum: http://www.cnusers.org/

> It needs free registration, which takes sometimes a few days.

> Or download from here:
> http://www.megafileupload.com/en/file/206928/CNS2100-LSDK-6-8-2-zip.html


**Q: How to acces data of USB hard drive?**

  1. SMB via NAS (hdd is attached to NAS USB and NAS is accessed via SMB)
> 2. FTP - with any ftp klient. ex. Fireftp.
> 3. direct access via PC USB.  use http://sourceforge.net/projects/ext2fsd/ or    http://sourceforge.net/projects/ext2read/ which can read even ext4 extents!
> 4. SCP - ssh based file management ex.: Winscp.

**Q: What is the FTP/SMB performance on ext3 filesystem?**

| Type | Write MB/s| Read MB/s|
|:-----|:----------|:---------|
| FTP: | 3 - 4     | 4 - 6    |
| SMB: | 2 - 3     | 3 - 4,5  |

> On NTFS the speed is _MUCH\_SLOWER!_.

> Speed on FAT almost as ext3, but not recommended.

> Speed is much worst via wlan!  less than 1MB/s !!

> _Use ext3_ because it is fastest and safest, and SMB will hide which filesystem is under it.


**Q: How can unrar files? Can I leave ssh session during unrar?**

> Yes. There is working ARM UNRAR for SnakeOs:
> [ftp://priede.bf.lu.lv/pub/Arhivatori/RAR/unrar/unrar-arm.tar.bz2](ftp://priede.bf.lu.lv/pub/Arhivatori/RAR/unrar/unrar-arm.tar.bz2)


> or here in download section:


  1. tar -xvf unrar-arm.tar.bz2
  1. copy unrar to nas sda1 root (/usb/sda1/) via windows share
  1. ssh to snake
  1. cd to directory where do you want to unrar files
  1. nohup /usb/sda1/unrar e -r `*.r*`

> it will extract all rar files from all subdirectories! nohup allows to leave ssh session (without needing to be online.)

> 6. exit    -  leaving ssh.

**Q: What is chroot? Why I need?**

> You can run hundreds of linux applications which are missing in the
> present SnakeOs.  (chroot is shell (a kind of sandbox) where you have debian libraries,    and environment within SnakeOS host.)

> How can I do?:

> Installation needs ~200MB disk space on attached USB and only 5 easy steps to set up.

> See documents here: http://code.google.com/p/snake-os/wiki/Debian_Chroot

**Q: How to save config in command line?**


/usr/share/snake/config save


**Q: How to make OPKG package ?**


http://code.google.com/p/snake-os/wiki/Packaging