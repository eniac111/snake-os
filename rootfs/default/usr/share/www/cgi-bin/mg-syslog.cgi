#!/bin/haserl
Content-type: text/html

<html xml:lang="en" xmlns="http://www.w3.org/1999/xhtml" lang="en">   
<%in funcs.cgi %>
<%

if [ "${REQUEST_METHOD}" = "POST" ]
then
	ACTION=$(echo ${FORM_action} | cut -d ' ' -f 1)
    case "$ACTION" in
		Apply)

		    SYSLOG_PATH=$(echo ${FORM_syslog_path} | cut -d ' ' -f 1)
	    	set_config syslog_path ${SYSLOG_PATH}

			SYSLOG_NET=$(echo ${FORM_syslog_net} | cut -d ' ' -f 1)
	    	set_config syslog_net ${SYSLOG_NET:-0}

			if [ ${SYSLOG_NET} = "1" ] ; then
				SYSLOG_ADDR=$(echo ${FORM_syslog_addr} | cut -d ' ' -f 1)
		    	set_config syslog_addr ${SYSLOG_ADDR}
			fi

			/etc/init.d/syslog restart > /dev/null
			sleep 1
		;;
		Start)
	      	/etc/init.d/syslog start > /dev/null
			sleep 1
    	;;
		Stop)
      		/etc/init.d/syslog stop > /dev/null
			sleep 1
		;;
		Restart)
      		/etc/init.d/syslog restart > /dev/null
			sleep 1
		;;
		*)
	esac
fi

SYSLOG_PATH=$(get_config syslog_path)
SYSLOG_NET=$(get_config syslog_net)
SYSLOG_ADDR=$(get_config syslog_addr)

%>

<script language="JavaScript">
<!-- //
function checkSyslogNet(){                                                           
    if ( <% get_config syslog_net %> == 0 ){
        document.forms[0].syslog_addr.disabled = true;
    }
}

function setReadSyslog(obj){
    if(obj.checked)
    {
        document.forms[0].syslog_addr.disabled = false;
        } 
    else {
        document.forms[0].syslog_addr.disabled = true;
        }
} 

function validateAll(form) {
	var syslog_path = form.syslog_path.value;
	var syslog_net = form.syslog_net.value;
	var syslog_addr = form.syslog_addr.value;
	if (syslog_path == '') {
		alert('Please enter a valid Syslog local path. If unsure, set it to: /var/messages');
		return false;
	}
	if (syslog_net == 1){
		if (syslog_addr == '') {
			alert('Enter an IP -preferred- or HOSTNAME address, or specify the port using ADDRESS:PORT to send syslog messages to. If unsure, unckeck the Network Syslog Enable checkbox.');
			return false;
		}
	}
	return true; 
}

// -->
</script>
</head>
<body onload="checkSyslogNet();">                            

<center>
<TABLE border="0" >
<form id=syslog name=syslog action="<%= ${SCRIPT_NAME} %>" method="POST" onsubmit="return validateAll(this);">

	<TR><TH>Syslog Local Path:</TH><TD><input type="text" name="syslog_path" size=30 value="<% get_config syslog_path %>" <% is_checked $(get_config webserver_enable) %> "/></TD></TR>
	<TR><TH>Network Syslog Enable:</TH><TD><input type="checkbox" name="syslog_net" value="1" <% is_checked $(get_config syslog_net) %> onclick="setReadSyslog(this);"/></TD></TR>
	<TR><TH>Network Syslog Address:</TH><TD><input type="text" name="syslog_addr" size=30 value="<% get_config syslog_addr %>" title="Enter an IP -preferred- or HOSTNAME address, or specify the port using ADDRESS:PORT to send syslog messages to."></TD></TR>

	<TR><TH>Syslog status: </TH><TD><% /etc/init.d/syslog webstatus %></TD></TR>
    </TABLE>
	<input type="submit" name="action" value="Apply" onclick="btnAction=this">
	<input type="submit" name="action" value="Start" onclick="btnAction=this">
	<input type="submit" name="action" value="Stop" onclick="btnAction=this">
	<input type="submit" name="action" value="Restart" onclick="btnAction=this">
</form>
</body>
</html>
