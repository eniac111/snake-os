#!/bin/haserl
Content-type: text/html

<html xml:lang="en" xmlns="http://www.w3.org/1999/xhtml" lang="en">   
<%in funcs.cgi %>

<script language="JavaScript">
<!-- //

var http_port = "";

function validateAction(form) {                                             
	http_port = form.http_port.value;
    var action = btnAction.value;
    var question = "";

	if ( action == 'Apply' ){		
		if (!validatePort(form.ssh_port.value,'SSH Server Port'))
			return false;

		question = "This will restart ssh server service. Are you sure?";
	}
    else if ( action == 'Start' )
    	return true;
    else if ( action == 'Stop' )
        question = "This action could disconnect remote users. Are you sure that you want to stop SFTP/SSH service?";
    else if  ( action == 'Restart' )
        question = "This action could disconnect remote users. Are you sure that you want to restart SFTP/SSH service?";
    else if  ( action == 'Recreate keys' )
        question = "This action will invalidate previous ssh keys, and may take a few minutes to complete. Are you sure?";
    var answer = confirm (question);
    if (answer)                                                      
        return true;                                                 
    else                                                            
        return false;                                           
} 

function validatePort(port,description){
	var re = /^\d{1,5}$/;

	if (port == '') {
		alert('Please enter a valid port for '+description+'. Accepted port numbers are between 1 and 65535.');
		return false;
	}
	if (!re.test(port)) { 
		alert('Please enter a valid port for '+description+'. Accepted port numbers are between 1 and 65535.');
		return false;
	}
	if (port < 1 || port > 65535) { 
		alert('Please enter a valid port for '+description+'. Accepted port numbers are between 1 and 65535.');
		return false;
	}
	if (port == http_port) { 
		alert('You have to use a different port for '+description+' and web admin interface.');
		return false;
	}
	return true;                                                 
}

// -->
</script>
</head>
<meta http-equiv='refresh' content='90'>
<body>                            

<center>

<%
if [ "${REQUEST_METHOD}" = "POST" ]
then
    ACTION=$(echo ${FORM_action} | cut -d ' ' -f 1)
    case "$ACTION" in
      Apply)
      	NEWPORT=$(echo ${FORM_ssh_port} | cut -d ' ' -f 1)
      	set_config ssh_port ${NEWPORT}
	
      	/etc/init.d/sshd restart > /dev/null
      	sleep 2		
    	;;
      Start)
      	/etc/init.d/sshd start > /dev/null
      	sleep 2
    	;;
      Stop)
      	/etc/init.d/sshd stop > /dev/null
      	sleep 2
    	;;
      Restart)
      	/etc/init.d/sshd restart > /dev/null
      	sleep 2
    	;;
      Recreate)
      	echo "<center><h2>Regeneration of SFTP/SSH keys in progress... Please wait for the page to reload, it may take up to several minutes...</h2></center>"
      	echo "<center>If the page stops without showing the service status, reload and try again.</center>"
      	/etc/init.d/sshd recreate-key > /dev/null
      	sleep 2
    	;;
    	
      *)
    esac
fi
%>
<form action="<%= ${SCRIPT_NAME} %>" method="POST"  onsubmit="return validateAction(this);">
     <TABLE border="0" >
	<input type="hidden" name="http_port" value="<% get_config http_port %>" >
	<TR><TH>SSH Server Port:</TH><TD><input type="text" name="ssh_port" size=4 value="<% get_config ssh_port %>" title="Enter a the SSH Server port. Accepted port numbers are between 1 and 65535."></TD></TR>     
	<TR><TH>Status:</TH><TD><% /etc/init.d/sshd webstatus %></TD></TR>
</TABLE>

   <input type="submit" name="action" value="Apply" onclick="btnAction=this">
   <input type="submit" name="action" value="Start" onclick="btnAction=this">
   <input type="submit" name="action" value="Stop" onclick="btnAction=this">
   <input type="submit" name="action" value="Restart" onclick="btnAction=this">
   <input type="submit" name="action" value="Recreate keys" onclick="btnAction=this">
      
</form>


</body>
</html>
