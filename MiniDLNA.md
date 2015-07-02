David Mills compiled mini DLNA for SNAKE V1.3

Thanks David :)


He compiled minidlna as He found it to be more useful than Ushare and mediatomb (mainly due to thumbnails on PS3).


Compiled [MINIDLNA](http://snake-os.googlecode.com/files/minidlna) file is here: in Download section and here http://www.megaupload.com/?d=FLU54WJI


Source:
http://sourceforge.net/projects/minidlna/

Reasons why its better/worse than media tomb:
http://blog.flexion.org/2009/12/18/mediatomb-minidlna/


Setup:

Copy MiniDLNA [file](http://snake-os.googlecode.com/files/minidlna) to your HDD.


Use putty or SSH to access your NAS, ssh into it.


ssh 192.168.1.20
cd /usb/hda1/
chmod +x minidlna
chmod +x start
./start
or ./minidlna -f minidlna.conf

Force Scan
./minidlna -f minidlna.conf -R

{FILENAME.EXT}.cover.jpg works as well as all those in the config file.

Static Libs:
vorbis
ogg
sqlite3
exif
jpeg
FLAC
id3tag
avformat
avutil
avcodec

### Minidlna Package ###

Minidlna is now also available as an opk package which can be installed from the SnakeOS interface. The packages install a startup script and a basic configuration page from which the shared directory can be set.

Current release:
http://code.google.com/p/snake-os/downloads/detail?name=minidlna-1.0.24-1.opk

Development version:
http://code.google.com/p/snake-os/downloads/detail?name=minidlna-cvs-20110611.opk

Both packages should work with snakeos-V1.3.2-20111019.