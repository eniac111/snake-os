#!/bin/haserl
Content-type: text/html

<html xml:lang="en" xmlns="http://www.w3.org/1999/xhtml" lang="en">   
<%in funcs.cgi %>
</head>
<body>                            
<%
if [ "${REQUEST_METHOD}" = "POST" ]
then
    NEWHOSTNAME=$(echo ${FORM_hostname} | cut -d ' ' -f 1)
    set_hostname $NEWHOSTNAME 
    echo "Storage Name changed to <b>$NEWHOSTNAME</b>.<br> "                        
    echo "Please restart the SNAKE device.<br>"   
fi
%>
<center>

   <form action="<%= ${SCRIPT_NAME} %>" method="POST">
     <TABLE border="0" >
     <TR><TH>Storage Name:</TH><TD><input type="text" name="hostname" size=20 value=<% hostname %></TD></TR>
     </TABLE>
   <input type="submit" name="newhost" value="Apply">
</form>
</body>
</html>
