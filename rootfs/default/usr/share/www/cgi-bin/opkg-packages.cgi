#!/bin/haserl --upload-dir=/opt/tmp/ --upload-limit=10000
Content-type: text/html

<html xml:lang="en" xmlns="http://www.w3.org/1999/xhtml" lang="en">   
<%in funcs.cgi %>
<%
if [ "${REQUEST_METHOD}" = "POST" ]
then
    ACTION=$(echo ${FORM_action} | cut -d ' ' -f 1)

    case "$ACTION" in
	Uninstall)
      	    UPACKAGE=$(echo ${FORM_upackage} | cut -d ' ' -f 1)
 	    PKGLOG=$(opkg-cl remove "$UPACKAGE" | tail -c 1000)
    	    ;;
	Install)
      	    IPACKAGE=$(echo ${FORM_ipackage} | cut -d ' ' -f 1)
# packages must have opk extension 
	    cp $IPACKAGE $IPACKAGE.opk
      	    PKGLOG=$(opkg-cl install "$IPACKAGE".opk | tail -c 1000)
	    rm $IPACKAGE.opk
            ;;
	*)
    esac
fi
%>


<%
package_form(){
%>
  <form action="<%= ${SCRIPT_NAME} %>" method="POST" enctype="multipart/form-data">
    <b>Install Package</b>
    <input type="file" name="ipackage">
    <input type="submit" name="action" value="Install">
  </form>

  <form action="<%= ${SCRIPT_NAME} %>" method="POST">
    <b>Installed packages:</b>
    <select name="upackage">
<%    
    OPTIONS=""
    for package in $(opkg-cl list-installed | sed s/" - .*$"/""/) ; do
        OPTIONS=${OPTIONS}"<option value="${package}">${package}</option>"
    done
    echo ${OPTIONS}
%>
    </select>
    <input type="submit" name="action" value="Show" onclick="btnAction=this">
    <input type="submit" name="action" value="Uninstall" onclick="btnAction=this">
  </form>

  <pre style="width: 80ex; text-align:left">
<%
    if [ "${REQUEST_METHOD}" = "POST" ]
    then
        ACTION=$(echo ${FORM_action} | cut -d ' ' -f 1)
        case "$ACTION" in
            Uninstall)
	        echo "$PKGLOG"
    	        ;;
            Install)
                echo "$PKGLOG"
                ;;
            Show)
	        UPACKAGE=$(echo ${FORM_upackage} | cut -d ' ' -f 1)
 	        opkg-cl info "$UPACKAGE"
	        ;;
            *)
        esac
    fi
%>
  </pre>
<%
}
%>


</head>
<body>
<center>
<% 
if [ $(get_config use_opkg) = 1 ]
then
    if [ -d /opt/opkg ]
    then
        package_form
    else
        echo "<b>Packages unavailable.</b><br><br>The <a href="opkg.cgi">opkg service</a> is enabled but the package directory on the specified disk cannot be accessed."
    fi
else
    echo "<b>Packages unavailable.</b><br><br>Please enable the <a href="opkg.cgi">opkg service</a> before accessing this page."
fi
%>
</center>
</body>
</html>
