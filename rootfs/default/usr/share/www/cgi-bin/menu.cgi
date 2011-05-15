#!/bin/haserl
Content-type: text/html

<html xml:lang="en" xmlns="http://www.w3.org/1999/xhtml" lang="en">   
<%in funcs.cgi %>
<head>
	<title>SNAKE</title>

	<link rel="StyleSheet" href="../css/dtree.css" type="text/css" />
	<script type="text/javascript" src="../js/dtree.js"></script>

</head>

<body>

<div class="dtree">
	<p><a href="javascript: d.openAll();">open all</a> | <a href="javascript: d.closeAll();">close all</a></p>

	<script type="text/javascript">
		<!--
		d = new dTree('d');
		d.config.target = "main";
		d.add(0,-1,'My NAS','status.cgi','SNAKE OS device status','','');
		
		d.add(1,0,'Device','','','','');
			d.add(2,1,'Hostname','hostname.cgi','Change hostname','','');
			d.add(3,1,'Admin','admin.cgi','Change admin password','','');
			d.add(4,1,'LAN Settings','network.cgi','Network settings','','');
			d.add(5,1,'Web Interface','http.cgi','Change web interface settings','','');
			d.add(6,1,'Time','time.cgi','Timezone, NTP and manual time config','','');

		d.add(10,0,'Disks','','','','');
			d.add(11,10,'Diskname','diskname.cgi','Manage disk or partition names','','');
			d.add(12,10,'Format','format.cgi','Format disks','','');
			d.add(13,10,'Partition','partition.cgi','Partition utilities','','');
		
		d.add(20,0,'Services','','','','');
			d.add(21,20,'DDNS','mg-ddns.cgi','Manage DDNS service','','');
			d.add(22,20,'FTP','mg-ftp.cgi','Manage FTP service','','');
			d.add(23,20,'Samba','mg-samba.cgi','Manage Samba service','','');
			d.add(24,20,'SFTP/SSH','mg-ssh.cgi','Manage SFTP and SSH daemons','','');
			d.add(25,20,'Swapfile','mg-swap.cgi','Manage swapping','','');
			d.add(26,20,'Syslog','mg-syslog.cgi','Manage System Logging','','');
			d.add(27,20,'Telnet','mg-telnet.cgi','Manage telnet service','','');
			d.add(28,20,'Transmission','mg-trans.cgi','Manage Transmission service','','');
			d.add(29,20,'WebServer','mg-webserver.cgi','Manage HTTP services','','');

		
		d.add(30,0,'Sharing','','','','');
			d.add(31,30,'Printers','printer.cgi','View shared printers','','');
			d.add(32,30,'Shares','share.cgi','Manage samba shares','','');
			d.add(33,30,'Users','user.cgi','Manage samba users','','');
		
		d.add(40,0,'System','','','','');
			d.add(41,40,'Config','config.cgi','Manage system config','','');
			d.add(42,40,'Status','status.cgi','System status','','');
			d.add(43,40,'Log','syslog.cgi','System log','','');
			d.add(44,40,'Monitor','sysmon.cgi','System monitoring','','');
			d.add(45,40,'Firmware','firmware.cgi','Manage system firmware','','');
		d.add(90,0,'About','credits.cgi','Firmware credits','','');	
		d.add(99,0,'Transmission','http://<% get_ipaddress %>:9091','Transmission BitTorrent software','_blank','../img/trans.png');

		document.write(d);

		//-->
	</script>

</div>

</body>

</html>
