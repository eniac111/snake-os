#!/bin/haserl
Content-type: text/html

<html xml:lang="en" xmlns="http://www.w3.org/1999/xhtml" lang="en">   
<%in funcs.cgi %>

<script language="JavaScript">
<!-- //

function validateAction(form) {                                             
    var action = btnAction.value;
    var question = "";
    if ( action == 'Start' )
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
     <TR><TH>Status:</TH><TD><% /etc/init.d/sshd webstatus %></TD></TR>
</TABLE>

   <input type="submit" name="action" value="Start" onclick="btnAction=this">
   <input type="submit" name="action" value="Stop" onclick="btnAction=this">
   <input type="submit" name="action" value="Restart" onclick="btnAction=this">
   <input type="submit" name="action" value="Recreate keys" onclick="btnAction=this">
      
</form>


</body>
</html>
