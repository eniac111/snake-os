# Overview #

Snake OS has basic support for installing Opkg packages. This makes it possible to easily and safely add new programs to the system.

Note: Package support is still fairly early and subject to changes. Suggestions and critique welcome.

# Usage #

To enable the Opkg service, navigate to Services -> Opkg in the web interface, check the "Enable opkg" box and select the disk that will hold the package data. Packages will be installed to the .optware directory on the specified disk.

Save your config to make the changes persistent.


## Package installation ##

To install a package download its .opk file and upload it from System -> Opkg. It may be necesary to reboot the system to afterwards.

**Warning: Packages have to be specifially built for Snake OS. Installing packages for other systems can break things!**

## Package removal ##

Uninstall packages from System -> Opkg.

## Troubleshooting ##

Reseting the Snake OS config will disable any Opkg services.

To get rid of all packages delete the .optware directory on the Opkg disk.


# Package creation #

The opk format is based on the one used by Debian. It's an "ar" archive containing:
  * data.tar.gz - the files to install
  * control.tar.gz - package information and installation scripts
  * debian-binary - a text file with the string "2.0"


## control.tar.gz ##

Here you have a bunch of scripts and control information. Check
http://inportb.com/2010/10/19/making-an-opkg-package/ to understand how it
works. Control.tar.gz must have all scripts and files on its root, which means that, when extracting it, no other folders should be created (all files must be extract to the current folder). It must have inside of it a file called "control" with this information (just an
example):

```
Package: dnsmasq
Version: 2.59
Description: Dnsmasq is a lightweight, easy to configure DNS forwarder and
DHCP server.
Section: snakeos/applications
Priority: optional
Maintainer:
Architecture: all
Homepage: http://thekelleys.org.uk/dnsmasq/doc.html
Source:
Depends:
```

## data.tar.gz ##

Mainly this is all files as if they were in /. For example, if something
should be put in /etc/ (in a normal environment), you should create a
folder called etc/ and put your file inside of it. Binaries should be put
inside a bin/ folder. The result could be something like this:

```
bin/dnsmasq
etc/dnsmasq.conf
etc/init.d/dnsmasq
```

All files will be inside /opt/ after they're loaded into SnakeOS. If you need to, for example reference some configuration file, you must use /opt/etc/file.conf.


### Script directories ###

Packages can have scripts in certain directories to better integrate into SnakeOS.

  * etc/init.d/ - Files inside this directory are startup scripts, and they should answer to "start", "stop" and "restart" arguments.

  * etc/automount.d/ - Files in this directory will be run when disks are added/removed. The parameters are "mountpoint add|remove"

  * share/www-service/ - Service pages for the web interface. These are haserl scripts that output html pages. Very similar to the ones in /usr/share/www/cgi-bin/.
> When a menu entry is clicked opkg-config-service.cgi is run with the name of the script as parameter. Like this: http://192.168.0.240/cgi-bin/opkg-config-service.cgi?page=dnsmasq

> Forms in a service script must pass the script name to the action parameter. For example for dnsmasq that that would be:
> 

&lt;form id=dnsmasq name=dnsmasq action="&lt;%= ${SCRIPT\_NAME} %&gt;?page=dnsmasq" method="POST"&gt;



  * share/www/ - Toplevel menu entries in the web interface. Same as www-service.


## debian-binary ##

A simple file with "2.0" inside of it.


## Making it easy ##

Assuming that you already have all the necessary files, you can use the following
Makefile to help you creating the package.
```
DATADIR=_data
CONTROLDIR=_control
PKGNAME=dnsmasq-2.59.opk

all: clean package

clean:
	@echo " CLEAN"
	@rm -rf data.tar.gz control.tar.gz "$(PKGNAME)"

package: data control
	@echo " AR   $(PKGNAME)"
	@ar -r "$(PKGNAME)" debian-binary control.tar.gz data.tar.gz

data: $(DATADIR)
	@echo " TAR  data.tar.gz"
	@tar -czf data.tar.gz -C $(DATADIR) .

control: $(CONTROLDIR)
	@echo " TAR  control.tar.gz"
	@tar -czf control.tar.gz -C $(CONTROLDIR) .

```

On top of it you have some setup to do:

  * DATADIR: directory with the contents of data.tar.gz
  * CONTROLDIR: directory with the contents of control.tar.gz
  * PKGNAME: the result package file (with the .opk extension)

So, after you put everything you need inside the correct folders and
changed the variables in the Makefile, you can just type "make" and it'll
do the entire job for you. The result will be a simple .opk file which you
can upload to your NAS in the end.

---

Thanks to Ricardo Gomes da Silva for expanding the initial howto.