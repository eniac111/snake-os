#!/bin/haserl
Content-type: text/html

<html xml:lang="en" xmlns="http://www.w3.org/1999/xhtml" lang="en">   
<%in funcs.cgi %>
<%

get_tz_sel(){
    if [ "$CUR_TZNUM" = "$1" ] ; then
        echo "selected"
    fi
}

get_year(){
    echo -n ${NOW:0:4}
}

get_month(){
    echo -n ${NOW:5:2}
}

get_day(){
    echo -n ${NOW:8:2}
}

get_hour(){
    echo -n ${NOW:11:2}
}

get_min(){
    echo -n ${NOW:14:2}
}

get_sec(){
    echo -n ${NOW:17:2}
}

set_tz(){
    case "$1" in
	 -11)
	set_config tznum -11
	set_config tzgeo Pacific/Apia
	set_config tzposix WST11
	;;
	 -10)
	set_config tznum -10
	set_config tzgeo Pacific/Honolulu
	set_config tzposix HST10
	;;
	 -9)
	set_config tznum -9
	set_config tzgeo America/Anchorage
	set_config tzposix AKST9AKDT,M4.1.0,M10.5.0
	;;
	 -8)
	set_config tznum -8
	set_config tzgeo America/Los_Angeles
	set_config tzposix PST8PDT,M4.1.0,M10.5.0
	;;
	 -7)
	set_config tznum -7
	set_config tzgeo America/Denver
	set_config tzposix MST7MDT,M4.1.0,M10.5.0
	;;
	 -6)
	set_config tznum -6
	set_config tzgeo America/Chicago
	set_config tzposix CST6CDT,M4.1.0,M10.5.0
	;;
	 -5)
	set_config tznum -5
	set_config tzgeo America/New_York
	set_config tzposix EST5EDT,M4.1.0,M10.5.0
	;;
	 -4)
	set_config tznum -4
	set_config tzgeo America/Asuncion
	set_config tzposix PYT4PYST,M10.3.0/0,M3.2.0/0
	;;
	 -3)
	set_config tznum -3
	set_config tzgeo America/Sao_Paulo
	set_config tzposix BRT3BRST,M10.3.0/0,M2.3.0/0
	;;
	 -2)
	set_config tznum -2
	set_config tzgeo America/Noronha
	set_config tzposix FNT2
	;;
	 -1)
	set_config tznum -1
	set_config tzgeo Atlantic/Azores
	set_config tzposix AZOT1AZOST,M3.5.0/0,M10.5.0/1
	;;
	 0)
	set_config tznum 0
	set_config tzgeo Europe/London
	set_config tzposix GMT0BST,M3.5.0/1,M10.5.0
	;;
	 1)
	set_config tznum 1
	set_config tzgeo Europe/Paris
	set_config tzposix CET-1CEST,M3.5.0,M10.5.0/3
	;;
	 2)
	set_config tznum 2
	set_config tzgeo Europe/Athens
	set_config tzposix EET-2EEST,M3.5.0/3,M10.5.0/4
	;;
	 3)
	set_config tznum 3
	set_config tzgeo Europe/Moscow
	set_config tzposix MSK-3MSD,M3.5.0,M10.5.0/3
	;;
	 4)
	set_config tznum 4
	set_config tzgeo Asia/Dubai
	set_config tzposix GST-4
	;;
	 5)
	set_config tznum 5
	set_config tzgeo Asia/Karachi
	set_config tzposix PKT-5
	;;
	 53)
	set_config tznum 53
	set_config tzgeo Asia/Calcutta
	set_config tzposix IST-5:30
	;;
	 54)
	set_config tznum 54
	set_config tzgeo Asia/Katmandu
	set_config tzposix NPT-5
	;;
	 6)
	set_config tznum 6
	set_config tzgeo Asia/Omsk
	set_config tzposix OMST-6OMSST,M3.5.0,M10.5.0/3
	;;
	 63)
	set_config tznum 63
	set_config tzgeo Asia/Rangoon
	set_config tzposix MMT-6
	;;
	 7)
	set_config tznum 7
	set_config tzgeo Asia/Bangkok
	set_config tzposix ICT-7
	;;
	 8)
	set_config tznum 8
	set_config tzgeo Asia/Hong_Kong
	set_config tzposix HKT-8
	;;
	 9)
	set_config tznum 9
	set_config tzgeo Asia/Tokyo
	set_config tzposix JST-9
	;;
	 93)
	set_config tznum 93
	set_config tzgeo Australia/Darwin
	set_config tzposix CST-9
	;;
	 931)
	set_config tznum 931
	set_config tzgeo Australia/South
	set_config tzposix CST-9:30CST-10:30,M10.4.0/2,M3.3.0/3
	;;
	 10)
	set_config tznum 10
	set_config tzgeo Australia/Sydney
	set_config tzposix EST-10EST,M10.5.0,M3.5.0/3
	;;
	 11)
	set_config tznum 11
	set_config tzgeo Asia/Magadan
	set_config tzposix MAGT-11MAGST,M3.5.0,M10.5.0/3
	;;
	 12)
	set_config tznum 12
	set_config tzgeo Pacific/Auckland
	set_config tzposix NZST-12NZDT,M10.1.0,M3.3.0/3
	;;
	 13)
	set_config tznum 13
	set_config tzgeo Pacific/Tongatapu
	set_config tzposix TOT-13
	;;
      *)
    NOACTION=1
    esac
    
    if [ -n $NOACTION ] ; then
      /usr/share/snake/settz 
    fi
   
}

if [ "${REQUEST_METHOD}" = "POST" ]
then
    USE_NTP=$(echo ${FORM_use_ntp} | cut -d ' ' -f 1)
    NTP_SERVER=$(echo ${FORM_ntp} | cut -d ' ' -f 1)
   	set_config ntp_enable ${USE_NTP:-0} 
    NEWTZ=$(echo ${FORM_tz} | cut -d ' ' -f 1)
    set_tz ${NEWTZ}
    if [ ${USE_NTP:-0} -eq 0 ] ; then
        YEAR=$(echo ${FORM_year} | cut -d ' ' -f 1)
        MONTH=$(echo ${FORM_month} | cut -d ' ' -f 1)
        DAY=$(echo ${FORM_day} | cut -d ' ' -f 1)
        HOUR=$(echo ${FORM_hour} | cut -d ' ' -f 1)
        MINUTE=$(echo ${FORM_minute} | cut -d ' ' -f 1)
        SECOND=$(echo ${FORM_second} | cut -d ' ' -f 1)
        date "$YEAR-$MONTH-$DAY $HOUR:$MINUTE:$SECOND" > /dev/null
    else
		set_config ntp_server $NTP_SERVER
        /usr/share/snake/setntp
    fi
fi
export TZ=$(cat /etc/TZ)
NOW=$(date -I'hours' -I'minutes' -I'seconds')
CUR_TZNUM=$(get_config tznum)

%>
<script language="JavaScript">
<!-- //
function checkNTP(){                                                           
    if ( <% get_use_ntp %> == 0 ){                                                                      
        document.forms[0].ntp.disabled = true;                             
    } 
    else{
        document.forms[0].year.readOnly = true;
        document.forms[0].month.readOnly = true;
        document.forms[0].day.readOnly = true;
        document.forms[0].hour.readOnly = true;
        document.forms[0].minute.readOnly = true;
        document.forms[0].second.readOnly = true;
    }                                                                          
}
function setReadNTP(obj){
    if(obj.checked)
    {
        document.forms[0].ntp.disabled = false;
        document.forms[0].year.readOnly = true;
        document.forms[0].month.readOnly = true;
        document.forms[0].day.readOnly = true;
        document.forms[0].hour.readOnly = true;
        document.forms[0].minute.readOnly = true;
        document.forms[0].second.readOnly = true;
        } 
    else {
        document.forms[0].ntp.disabled = true;
        document.forms[0].year.readOnly = false;
        document.forms[0].month.readOnly = false;
        document.forms[0].day.readOnly = false;
        document.forms[0].hour.readOnly = false;
        document.forms[0].minute.readOnly = false;
        document.forms[0].second.readOnly = false;
        }
} 

function daysInFebruary (year){
    return (((year % 4 == 0) && ( (!(year % 100 == 0)) || (year % 400 == 0))) ? 29 : 28 );
}
function DaysArray(n) {
	for (var i = 1; i <= n; i++) {
		this[i] = 31;
		if (i==4 || i==6 || i==9 || i==11) {
            this[i] = 30;
        }
		if (i==2) {
            this[i] = 29;
        }
   } 
   return this;
}

function isInteger(s){
	var i;
    for (i = 0; i < s.length; i++){   
        var c = s.charAt(i);
        if (((c < "0") || (c > "9"))) 
            return false;
    }
    return true;
}

function validateAction(form) {
    minyear = 2009;
    maxyear = 2037;
    var daysInMonth = DaysArray(12);
    year = form.year.value;
    month = form.month.value;
    day = form.day.value;
    hour = form.hour.value;
    minute = form.minute.value;
    second = form.second.value;
    if(form.use_ntp.checked){
        if ( form.ntp.value == ''){
            alert("The ntp server cannot be empty");
            return false;
        }
    }
    else
        {
        if ( isInteger(year) == false || year<minyear || year>maxyear){
    		alert("Please enter a valid year: from 2009 to 2037");
    		return false;
    	}
        if ( isInteger(month) == false || month<1 || month>12){
    		alert("Please enter a valid month");
    		return false;
    	}
        if ( isInteger(day) == false || day<1 || day>31 || (month==2 && day>daysInFebruary(year)) || day > daysInMonth[month]){
    		alert("Please enter a valid day");
    		return false;
    	}
        if ( isInteger(hour) == false || hour<0 || hour>23){
    		alert("Please enter a valid hour");
    		return false;
    	}
        if ( isInteger(minute) == false || minute<0 || minute>59){
    		alert("Please enter a valid minute");
    		return false;
    	}
        if ( isInteger(second) == false || second<0 || second>59){
    		alert("Please enter a valid second");
    		return false;
    	}
    }
    return true;
} 

// -->
</script>
</head>
<body onload="checkNTP();">   
<center>
<form action="<%= ${SCRIPT_NAME} %>" method="POST" onsubmit="return validateAction(this);">
<TABLE border="0">
<TR>
<TH>Select Timezone:</TH>
<TD>
<select name="tz">
<option value="-11" <%= $(get_tz_sel "-11") %>>Pacific/Apia</option>
<option value="-10" <%= $(get_tz_sel "-10") %>>Pacific/Honolulu</option>
<option value="-9" <%= $(get_tz_sel "-9") %>>America/Anchorage</option>
<option value="-8" <%= $(get_tz_sel "-8") %>>America/Los_Angeles</option>
<option value="-7" <%= $(get_tz_sel "-7") %>>America/Denver</option>
<option value="-6" <%= $(get_tz_sel "-6") %>>America/Chicago</option>
<option value="-5" <%= $(get_tz_sel "-5") %>>America/New_York</option>
<option value="-4" <%= $(get_tz_sel "-4") %>>America/Asuncion</option>
<option value="-3" <%= $(get_tz_sel "-3") %>>America/Sao_Paulo</option>
<option value="-2" <%= $(get_tz_sel "-2") %>>America/Noronha</option>
<option value="-1" <%= $(get_tz_sel "-1") %>>Atlantic/Azores</option>
<option value="0" <%= $(get_tz_sel "0") %>>Europe/London</option>
<option value="1" <%= $(get_tz_sel "1") %>>Europe/Paris</option>
<option value="2" <%= $(get_tz_sel "2") %>>Europe/Athens</option>
<option value="3" <%= $(get_tz_sel "3") %>>Europe/Moscow</option>
<option value="4" <%= $(get_tz_sel "4") %>>Asia/Dubai</option>
<option value="5" <%= $(get_tz_sel "5") %>>Asia/Karachi</option>
<option value="53" <%= $(get_tz_sel "53") %>>Asia/Calcutta</option>
<option value="54" <%= $(get_tz_sel "54") %>>Asia/Katmandu</option>
<option value="6" <%= $(get_tz_sel "6") %>>Asia/Omsk</option>
<option value="63" <%= $(get_tz_sel "63") %>>Asia/Rangoon</option>
<option value="7" <%= $(get_tz_sel "7") %>>Asia/Bangkok</option>
<option value="8" <%= $(get_tz_sel "8") %>>Asia/Hong_Kong</option>
<option value="9" <%= $(get_tz_sel "9") %>>Asia/Tokyo</option>
<option value="93" <%= $(get_tz_sel "93") %>>Australia/Darwin</option>
<option value="931" <%= $(get_tz_sel "931") %>>Australia/South</option>
<option value="10" <%= $(get_tz_sel "10") %>>Australia/Sydney</option>
<option value="11" <%= $(get_tz_sel "11") %>>Asia/Magadan</option>
<option value="12" <%= $(get_tz_sel "12") %>>Pacific/Auckland</option>
<option value="13" <%= $(get_tz_sel "13") %>>Pacific/Tongatapu</option>
</select>
</TD>
</TR>
<TR><TH>Use NTP:</TH><TD><input type="checkbox" name="use_ntp" value="1" <% is_checked $(get_config ntp_enable) %> onclick="setReadNTP(this);"/></TD></TR>
<TR><TH>NTP server:</TH><TD><input type="text" name="ntp" size=20  value=<% get_config ntp_server %> title="Enter a NTP server name or IP address. Ex.: pool.ntp.org"></TD></TR>

<TR>
    <TD COLSPAN=2>Date:
    <INPUT class=text id=year maxLength=4 name=year size=4 value="<%= $(get_year)%>" title="Year">
    /
    <INPUT class=text id=month maxLength=2 name=month size=2 value="<%= $(get_month)%>" title="Month">
    / 
    <INPUT class=text id=day maxLength=2 name=day size=2 value="<%= $(get_day)%>" title="Day">
    (yyyy/mm/dd)</TD>
</TR>
<TR>
    <TD COLSPAN=2>Time:
    <INPUT class=text id=hour maxLength=2 name=hour size=2 value="<%= $(get_hour)%>" title="Hour">
    : 
    <INPUT class=text id=minute maxLength=2 name=minute size=2 value="<%= $(get_min)%>" title="Minute">
    :
    <INPUT class=text id=second maxLength=2 name=second size=2 value="<%= $(get_sec)%>" title="Second">
    (hh:mm:ss) </TD>
</TR>
<TR><TD COLSPAN=2><input type="submit" name="action" value="Apply"></TD></TR>
</TABLE>
</form>
</body>
</html>

