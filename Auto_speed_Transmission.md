Setting Auto speed for Transmission

# Introduction #

When you appear on your local network, this script automatically decrease transmission speed.

Developped by Rodrigo Lanes, published by _Robert Szabo_


# Details #

**crontab**

  * 10   sh /usb/hd1/scripts/auto\_speed\_TR.sh`

the script runs every 10 minutes.

**auto\_speed\_TR.sh**  - Shell scipt :
```
more auto_speed_TR.sh 
  found=0;
  computers=/usb/hd1/scripts/computers.txt;

  for ip in `cat $computers`; do ping -c 1 $ip>/dev/null; [ $? -eq 0 ] && echo "$ip UP" &&   found=1 && break || echo "$ip DOWN" ; done

  if [ $found -eq 1 ] 
  then
    echo "Transmission speed down" >> /var/messages;
    /usb/hd1/transmission-remote localhost:9091 -n admin:pass -d 10 -u 10

  else
       echo "Transmission speed up" >> /var/messages
       /usb/hd1/transmission-remote localhost:9091 -n admin:pass -D -U
  fi;
```


**exmalpe of computers.txt**
```
#more computers.txt
10.0.0.10
10.0.0.100
10.0.0.101
10.0.0.102
```


**Download Transmission Remote**

Console based Transmission Remote can be downloaded from here:
[transmission-remote](http://snake-os.googlecode.com/files/transmission-remote)
```
cd /usb/hd1
wget http://snake-os.googlecode.com/files/transmission-remote 
```