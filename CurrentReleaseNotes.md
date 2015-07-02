# SNAKE firmware for Str8132-Based NAS #

The different firmware types where renamed to **"from-original"** and **"from-snake"**, to better indicate when to use each one. If unsure, use **"from-original"**.

The more feedback is given, the better can be worked out on this project.

## ADVICE: ##
This firmware is experimental, use at your own risk. If you get stuck somewhere, a serial connection may be needed to debug it!!!

We will not be responsible for data loss of any kind. Understand what you are doing to not brick your device.



### Release info: ###
Version 1.1.0 (2010-04-18)
### What's new: ###

### IMPROVEMENTS ###
  * Enhancement [issue #9](https://code.google.com/p/snake-os/issues/detail?id=#9): Added syslog option and changed default system log view on web interface to it.
    * Services enabled to log some messages to syslog
    * As Syslog startup is delayed to have mounted disks and/or network available, boot messages are recovered and may appear out of order.
  * Enhancement [issue #12](https://code.google.com/p/snake-os/issues/detail?id=#12): Allow for more static DNS servers
  * Transmission updated to version 1.92 (10363)
  * Changed string checking on config backup/restore. Will allow to restore config from SAME Snake OS VERSION, even if saved from firmware with different build dates.
    * Eg.: Backup from V1.1.0 (2010-04-10) and restore to V1.1.0 (2010-04-12) is possible, but from V1.1.0 (whatever) to V1.2.0 (whatever) will NOT be possible yet.
### BUG FIXES ###
  * Fix for [issue #3](https://code.google.com/p/snake-os/issues/detail?id=#3): Multiple shared printers not working

  * Fix for [issue #5](https://code.google.com/p/snake-os/issues/detail?id=#5): Added time zone for Adelaide ( Australia/South: CST-9:30CST-10:30,M10.4.0/2,M3.3.0/3 )

  * Fix for [issue #13](https://code.google.com/p/snake-os/issues/detail?id=#13): Webserver root folder cannot be configured in web interface


### Features: ###
  * GNU utils: Busybox 1.14.2 patched.

  * Device management: Busybox 1.14.2 mdev.

  * Samba server: Samba 2.0.10 patched.

  * FTP server: Vsftpd 2.2.0 patched.

  * Disk Management: E2fsprogs 1.41.8.

  * Print Server: p910nd 0.93. The printer processes are dinamically started/stopped when a printer is connected and disconnected.

  * SFTP Daemon: OpenSSH sftp-server version 5.2p1

  * SSH Daemon: Dropbear sshd version 0.52, with rsa and dss keys

  * SCP Daemon: Dropbear scp version 0.52

  * Telnet Daemon: Busybox 1.14.2 telnetd

  * Torrent: Transmission daemon 1.92. Encryption and default web interface enabled.

  * Syslog Daemon: Busybox 1.14.2 syslogd

  * Web Server: Busybox 1.14.2 httpd

  * Filesystems recognized: ext2, ext3, fat, vfat, ntfs, jffs2.

  * NTP client: ntpclient 2007.

  * DDNS client: inadyn 1.96.2.


  * Swap and Torrent automagically started when a disk with correct config is inserted (already had torrent/swap in use on it and the current configuration points to that location. When removal is detected, those services are stopped too.



### Known Bugs/Issues: ###
  * Swapfile: It is NOT recommended to use swapfile on ntfs/vfat partitions, because of mounting and speed issues.
  * Recommended filesystems to use, in order (security): 1 - EXT3, 2 - EXT2, 3 - FAT32, 4 - NTFS.
  * Recommended filesystems to use, in order (speed): 1 - EXT2, 2 - VFAT, 3 - EXT3, 4 - NTFS.
  * Samba codepage issues with some eastern languages.


### TODO: ###
  * Filesystems: ext4 and RAID support.

  * Servers: NFS support.

  * Samba: Codepage support.



### Installation: ###
> Both types of upgrade took up to 5 minutes in the tests. Don't reset or power off the device before this time.



### => FROM ORIGINAL FIRMWARE: USE "**FROM-ORIGINAL**" VERSION ONLY!!! <= ###



> The upgrade from the FUKBOON firmware can be done direct to the SNAKE OS beta 2, through it's web firmware upgrade page.

> It's also possible to revert back to the original firmware in the same way later, if you wish ;-)




### => FROM SNAKE OS FIRMWARE: PREFER "**FROM-SNAKE**" VERSION. INFO: BOTH TYPES WILL WORK. <= ###



In the SNAKE firmware, it will ask you to have a drive connected as sda1 for the update to work, also remember to have swapping on.

> Apparently, in FUKBOON firmware a drive connected as C may be needed too.



> After installation, the device will reboot itself and revert back to the default settings:



  * Initial IP address: DHCP or 192.168.0.240 ( same as original firmware)

  * Web admin and Transmission user/password: admin/snake

  * SSH user/password: root/snake


  * After the first boot, the system will be slow for 1 to 3 minutes, because it is creating the initial SSH keys. Save the config  (wich contains the SSH key) before rebooting and after the CPU gets low again, and this will not occur anymore. To do this, go to System->Config in web page.



### Web admin: ###

Access the device from your browser: http://IP or http://IP/index.html , where IP is initially 192.168.0.240 or another IP address, if you had changed it...



> ### Web admin sections: ###



  * **My NAS:** General device status and reboot.



  * **Device->Hostname:** Change system name.

  * **Device->Admin:**Change administrative password.

  * **Device->LAN Settings:**Manual or automatic IP Address settings. Manual MAC address management. When the device is flashed, the MAC will change.

  * **Device->Web Interface:**Change default http port assignment for the administrative interface. Use with caution.


  * **Device->Time:**Manual date/time setting or ntp config. It will try
to connect every hour to keep time current. Remember to adjust your
timezone here, too.



  * **Disks->Diskname:** Disk n<span>aming option, it will recognize a disk "uuid+label" (will call it signature) and <b>optionally</b> specify a mount point with a custom name. It will ease with sharing setup, when the order that disks are mounted used to matter.</li></ul>

<ul><li><b>Disks->Format:</b> The format setting used will be ext3, with only one partition across<br>
the entire disk. It has be chosen because of performance and stability<br>
in tests. Note: If you wish a different partitioning, will have to<br>
format outside the device in the moment.</li></ul>

<ul><li><b>Disks->Partition:</b> To verify on which mount point one partition is/will be mounted. If you wish to properly disconnect a device, please umount it here first. A running service may avoid one partition to be umounted. Stop these services and try again, if possible. If a mounted partition use ext3/ext2 filesystem, error checking and correction options will be available.</li></ul>



<ul><li><b>Services->DDNS:</b> Manage the dynamic dns client service.</li></ul>

<ul><li><b>Services->FTP:</b> Manage the ftp sharing service and set permission mode.</li></ul>

<ul><li><b>Services->Samba:</b> Manage the file sharing service and set permission mode. All files will be owned by root on ext2/3 filesystems.</li></ul>

<ul><li><b>Services->SFTP/SSHD:</b>Manage the SFTP/SSH Daemon service and optionally regenerate the keys, if they where compromised, for example. Note: A new key is generated on the<br>
first device boot, so dealing with this will not be needed, generally.</li></ul>

<ul><li><b>Services->Swapfile:</b> Manage the Swapping service. <b>ALWAYS USE IT</b>.</li></ul>

<ul><li><b>Services->Telnet:</b> Manage the telnetd for remote access to shell of the NAS.</li></ul>

<ul><li><b>Services->Transmission:</b>Manage the BitTorrent service. Create your folder before setting it. The Transmission service will create a drop folder inside called "<i>torrentwatch</i>", where you can copy a .torrent file and the files will be automatically<br>
downloaded. Also possible to control startup and alternate transmission speed from here (will restart the service).</li></ul>

<ul><li><b>Services->Syslog:</b> Manage internal/external Syslog daemon.</li></ul>

<ul><li><b>Services->WebServer:</b> Manage the additional NAS Web Server.</li></ul>




<ul><li><b>Sharing->Printers:</b> View of shared printers.</li></ul>

<ul><li><b>Sharing->Shares:</b> Manage shared samba folders.</li></ul>

<ul><li><b>Sharing->Users:</b> Manage users and passwords.</li></ul>




<ul><li><b>System->Config:</b> Save device config to file/flash or restore the default  configuration.</li></ul>

<ul><li><b>System->Status:</b> General device status and reboot.</li></ul>

<ul><li><b>System->Log:</b> System message log.</li></ul>

<ul><li><b>System->Monitor:</b> Monitor CPU, memory and disk space usage.</li></ul>

<ul><li><b>System->Firmware:</b> Upgrade (or downgrade) your firmware. The process check the file integrity before write.</li></ul>



<ul><li><b>About:</b> People that have helped so far. Of course everyone with ideas is helping, but getting the hands dirty counts, too.</li></ul>



<ul><li><b>Transmission:</b> Link to Transmission web admin. As a separate process, will ask for login info again. Will default to <a href='http://IP:9091'>http://IP:9091</a>. When transmission saves config, it keeps the changes only in memory until transmission restart. You will need to save it,<br>
back to Services->Transmission, hit Apply and go to System->Config and Save.</li></ul>


<h3>Other considerations: ###
  * As space permits only very stripped down packages, it's hard to beat Debian usability with just flash space.

  * There are some other apps enabled in Busybox, like wget, top, tftp, ftpget, ftpput, etc. play with those too.

  * A good streaming server would use too many resources. May work on it if sufficient interest or as a separate sub-project.

  * To avoid internal space constraints, one possible approach is to use external "modules", containing software packages. It's on study for future versions.



### Download: ###
**If you are already running**SNAKE OS beta2 or later**, prefer the from-snake version, otherwise, use the from-original version.**

Grab it from:  <font size='4'><a href='http://code.google.com/p/snake-os'><a href='http://code.google.com/p/snake-os'>http://code.google.com/p/snake-os</a></a>