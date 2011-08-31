#!/bin/sh

SERVICE=$(echo $QUERY_STRING | cut -d "=" -f 2)
if [ \( $(echo -n $SERVICE | sed s/[a-zA-Z0-9]*/""/g | wc -c) -eq 0 \) -a \( -f /opt/etc/wwwcfg/$SERVICE \) ]; then
   /bin/haserl /opt/etc/wwwcfg/$SERVICE
else
	echo "Content-type: text/html"
	echo ""
	echo "<html>opkg-config: bad service parameter</html>"
fi
